#!/usr/bin/env python

import click
from tqdm import tqdm

import sys
import re
import io
import os


WHITE_WIN = "1-0"
DRAW = "1/2-1/2"
BLACK_WIN = "0-1"
INCOMPLETE = "*"


@click.group()
def cli():
    pass


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
    import polars as pl

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
def openings_book_cli(in_file, out):
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


@cli.command("openings-deck", help="dump openings")
@click.argument(
    "in-file",
    type=click.File(),
    default=sys.stdin,
)
def openings_cli(in_file):
    import chess.pgn
    import chess.svg

    openings: dict[str, list[str]] = {}
    openings_filenames: dict[str, str] = {}
    while True:
        game = chess.pgn.read_game(in_file)
        if game is None:
            break

        if "Opening" not in game.headers:
            continue

        opening = game.headers["Opening"]
        if "Variation" in game.headers:
            opening += f", {game.headers['Variation']}"

        board = chess.Board()
        moves: list[str] = []
        for ply, move in enumerate(game.mainline_moves()):
            if ply % 2 == 0:
                moves.append(f"{ply // 2 + 1}.")
            san = board.san_and_push(move)
            moves.append(san)

        #        svg = chess.svg.board(board)
        #        filename = opening.replace("/", "x") + ".svg"
        #        openings_filenames[opening] = filename
        #        with open(f"openings/deck/{filename}", "w") as f:
        #            f.write(svg)
        #
        openings[opening] = moves

    opening_names = sorted(list(openings.keys()), key=lambda name: len(openings[name]))
    for idx, name in enumerate(opening_names):
        print(idx + 1, len(openings[name]))
    for idx, name in enumerate(opening_names):
        moves = openings[name]
        moves_str = " ".join(moves)


#        row = [
#            f"{str(idx).zfill(5)} {name}",
#            moves_str + f'<br><img src="{openings_filenames[name]}"/>',
#        ]
#        print("\t".join(row))


@cli.command("prune", help="Prunes a lichess PGN file")
@click.argument(
    "out-file",
    type=click.File(mode="w"),
    default=sys.stdin,
)
@click.argument(
    "in-file",
    type=click.File(),
    default=sys.stdin,
)
def prune_cli(out_file, in_file):
    buffer = ""
    with tqdm(total=1787158305) as pbar:
        # with tqdm(total=89342529) as pbar:
        for line in in_file:
            if (
                line.startswith('[Termination "Time forfeit"]')
                or line.startswith('[WhiteTitle "BOT"]')
                or line.startswith('[BlackTitle "BOT"]')
            ):
                buffer = None

            if line.startswith("[Event "):
                if "Bullet" in line:
                    buffer = None
                    continue

                if buffer is not None:
                    out_file.write(buffer)

                buffer = ""

            if buffer is not None:
                buffer += line

            pbar.update()
        if buffer is not None:
            out_file.write(buffer)


if __name__ == "__main__":
    cli()
