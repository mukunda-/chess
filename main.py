#!/usr/bin/env python

import os

import click

import polars as pl


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


if __name__ == "__main__":
    cli()
