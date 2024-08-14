#!/usr/bin/env python

import sys
import re
import io
import os
import uuid

import click
from click.utils import LazyFile
from tqdm import tqdm

import polars as pl

import psycopg


WHITE_WIN = "1-0"
DRAW = "1/2-1/2"
BLACK_WIN = "0-1"
INCOMPLETE = "*"

HEADER_NAMES = [
    "White",
    "WhiteElo",
    "WhiteFideId",
    "WhiteRatingDiff",
    "WhiteTeam",
    "WhiteTitle",
    "Black",
    "BlackElo",
    "BlackFideId",
    "BlackRatingDiff",
    "BlackTeam",
    "BlackTitle",
    "Annotator",
    "Board",
    "Date",
    "ECO",
    "Event",
    "Opening",
    "Result",
    "Round",
    "Site",
    "Termination",
    "TimeControl",
    "UTCDate",
    "UTCTime",
    "PGN",
]

HEADERS_IDX = {name: idx for idx, name in enumerate(HEADER_NAMES)}

HEADER_COLUMNS = [
    "white",
    "white_elo",
    "white_fide_id",
    "white_rating_diff",
    "white_team",
    "white_title",
    "black",
    "black_elo",
    "black_fide_id",
    "black_rating_diff",
    "black_team",
    "black_title",
    "annotator",
    "board",
    "date",
    "eco",
    "event",
    "opening",
    "result",
    "round",
    "site",
    "termination",
    "time_control",
    "utc_date",
    "utc_time",
    "pgn",
]

HEADER_TO_COLUMN = {HEADER_NAMES[idx]: col for idx, col in enumerate(HEADER_COLUMNS)}

ALL_MOVE_COLUMNS = [f"ply_{ply}" for ply in range(30)]

config = {k: os.environ[k] for k in ["DB_USERNAME", "DB_PASSWORD", "DB_NAME"]}


def scan_tsv(sources, headers):
    df = pl.scan_csv(sources, separator="\t", has_header=False, new_columns=headers)

    return df


def stream_columns(sources):
    for source in sources:
        with open(source, "r") as f:
            for line in f:
                line.strip()
                columns = line.split("\t")
                yield columns


@click.group()
def cli():
    pass


@cli.command("eval", help="evaluate positions in db")
def cli_serve():
    import chess.engine
    import chess

    engine = chess.engine.SimpleEngine.popen_uci(
        "external/stockfish/stockfish-ubuntu-x86-64-avx2"
    )

    db_uri = f"postgresql://{config['DB_USERNAME']}:{config['DB_PASSWORD']}@127.0.0.1:5432/{config['DB_NAME']}"
    max_depth = 20
    with psycopg.connect(db_uri, autocommit=True) as conn:
        with conn.cursor() as cur:
            cur.execute(
                "select last_board_hash, next_board_hash, move_uci from continuations where ply = 1 group by 1,2,3 having count(*) > 10000 order by count(*) desc",
            )

            todo = [
                [chess.Board(), *continuation, max_depth]
                for continuation in cur.fetchall()
            ]

            seen = {}
            while todo:
                print(len(todo))

                board, last_board_hash, next_board_hash, move_uci, depth = todo.pop(0)
                if depth <= 0 or (last_board_hash, move_uci) in seen:
                    continue

                seen[(last_board_hash, move_uci)] = True

                board.push_uci(move_uci)
                info = engine.analyse(board, chess.engine.Limit(depth=20))
                score = info["score"].white()
                numeric_score = score.score()
                if score.is_mate():
                    stockfish_cp_score = None
                    stockfish_mate_score = numeric_score
                else:
                    stockfish_cp_score = numeric_score
                    stockfish_mate_score = None

                cur.execute(
                    "update boards set stockfish_cp_score = %s, stockfish_mate_score = %s where hash = %s",
                    (stockfish_cp_score, stockfish_mate_score, next_board_hash),
                )

                cur.execute(
                    "select last_board_hash, next_board_hash, move_uci from continuations where last_board_hash = %s group by 1,2,3 having count(*) > 5000 order by count(*) desc limit 20",
                    (next_board_hash,),
                )

                todo.extend(
                    [board.copy(stack=False), *continuation, depth - 1]
                    for continuation in cur.fetchall()
                )

        engine.quit()


@cli.command("endgame-wdl", help="Calculate WDL per endgame classification")
@click.option("--pretty", is_flag=True)
@click.argument(
    "out-path",
    type=click.Path(),
)
@click.argument(
    "in-paths",
    type=click.Path(exists=True),
    nargs=-1,
)
def cli_endgames_wdl(pretty, out_path, in_paths):
    df = pl.scan_csv(
        in_paths,
        has_header=False,
        separator="\t",
        new_columns=[
            "fen",
            "blank",
            "eco",
            "white_elo",
            "black_elo",
            "time_control",
            "site",
            "date_utc",
            "time_utc",
            "result",
            "endgame",
        ],
    )

    rating_bins = [
        1035,
        1100,
        1165,
        1225,
        1290,
        1350,
        1415,
        1475,
        1575,
        1675,
        1750,
        1825,
        1900,
        2000,
        2075,
        2150,
        2225,
        2300,
        2375,
        2450,
        2525,
        2600,
    ]

    rating_bin_labels = [
        "0-300",
        "300-400",
        "400-500",
        "500-600",
        "600-700",
        "700-800",
        "800-900",
        "900-1000",
        "1000-1100",
        "1100-1200",
        "1200-1300",
        "1300-1400",
        "1400-1500",
        "1500-1600",
        "1600-1700",
        "1700-1800",
        "1800-1900",
        "1900-2000",
        "2000-2100",
        "2100-2200",
        "2200-2300",
        "2300-2400",
        "2400+",
    ]

    df = (
        df.filter(
            (
                (pl.col("result") == "1/2-1/2")
                | (pl.col("result") == "1-0")
                | (pl.col("result") == "0-1")
            )
            & (pl.col("time_control") != "-")
        )
        .with_columns(
            pl.col("time_control")
            .str.split(by="+")
            .list.get(0)
            .alias("initial")
            .cast(pl.Int16),
            pl.col("time_control")
            .str.split(by="+")
            .list.get(1)
            .alias("increment")
            .cast(pl.Int16),
        )
        .filter((pl.col("initial") + (pl.col("increment") * 40)) >= 1500)
        .with_columns(
            pl.col("white_elo")
            .cut(rating_bins, labels=rating_bin_labels)
            .alias("white_elo_band")
        )
    )

    endgames = {}
    for row in (
        df.group_by("white_elo_band", "endgame", "result")
        .len()
        .collect()
        .rows(named=True)
    ):
        band = row["white_elo_band"]
        if band not in endgames:
            endgames[band] = {}

        endgame = row["endgame"]
        if endgame not in endgames[band]:
            endgames[band][endgame] = {
                "band": band,
                "endgame": endgame,
                WHITE_WIN: 0,
                BLACK_WIN: 0,
                DRAW: 0,
            }

        endgames[band][endgame][row["result"]] = row["len"]

    rows = sum(list(list(band.values()) for band in endgames.values()), [])
    for row in rows:
        total = row[WHITE_WIN] + row[BLACK_WIN] + row[DRAW]
        row["total"] = total

        row[WHITE_WIN] /= total
        row[BLACK_WIN] /= total
        row[DRAW] /= total

        row["wdl"] = "/".join(
            str(round(x * 100, 2)) + "%"
            for x in [row[WHITE_WIN], row[DRAW], row[BLACK_WIN]]
        )

    out_df = pl.DataFrame(rows)
    if pretty:
        out_df = out_df.select("total", "band", "endgame", "wdl")
    else:
        out_df = out_df.select("total", "band", "endgame", WHITE_WIN, DRAW, BLACK_WIN)

    out_df.write_csv(out_path, separator="\t")


@cli.command("board-images", help="build openings book")
@click.argument("in-file", type=click.File())
@click.option("-o", "--out", type=click.Path(exists=True), required=True)
def cli_board_images(in_file, out):
    import json
    import chess.pgn
    import chess.svg
    import chess

    unsafe_chars_re = re.compile(r"[^\w]")

    rows = json.load(in_file)
    for row in tqdm(rows):
        pgn = io.StringIO(row["pgn"] + "\n\n")
        game = chess.pgn.read_game(pgn)
        if not game:
            print("Failed on", row)
            sys.exit(1)

        board = game.board()
        for move in game.mainline_moves():
            board.push(move)

        svg = chess.svg.board(board)

        filename = unsafe_chars_re.sub("_", board.epd())
        path = os.path.join(out, filename)
        with open(f"{path}.svg", "w") as f:
            f.write(svg)


if __name__ == "__main__":
    cli()
