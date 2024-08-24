#!/usr/bin/env python

import click

import polars as pl
import sys


@click.command()
@click.argument("in-files", nargs=-1, type=click.File(mode="r"))
@click.option("-o", "--out", help="output file", type=click.Path())
def main(in_files, out):
    if not in_files:
        return

    if out is None:
        out = sys.stdout

    dfs = [pl.read_csv(file, separator="\t") for file in in_files]
    df = pl.concat(dfs)
    df = df.with_columns(pl.col("pgn").str.split(" ").list.len().alias("ply"))

    df = df.with_columns(
        pl.col("name")
        .str.splitn(": ", 2)
        .struct.rename_fields(["family", "child"])
        .alias("name_split")
    ).unnest("name_split")

    df = df.with_columns(
        pl.col("child")
        .str.splitn(", ", 3)
        .struct.rename_fields(["lvl_1", "lvl_2", "lvl_3"])
        .alias("child_split")
    ).unnest("child_split")

    df = df.select("family", "lvl_1", "lvl_2", "lvl_3", "pgn", "ply")

    df.write_csv(out, separator="\t")

    df_family = df.group_by("family").len(name="count").sort("count")

    df_family.write_csv(sys.stdout, separator="\t")


if __name__ == "__main__":
    main()
