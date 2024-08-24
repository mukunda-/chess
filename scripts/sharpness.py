#!/usr/bin/env python

import click

import polars as pl
import sys


@click.command()
@click.argument("in-files", nargs=-1, type=click.File(mode="r"))
@click.option("-o", "--out", help="output file", type=click.Path())
@click.option("--odd", help="print rows with an odd value of this column")
@click.option("--even", help="print rows with an even value of this column")
def main(in_files, out, odd, even):
    if out is None:
        out = sys.stdout

    if not in_files:
        return

    dfs = [pl.read_csv(file, separator="\t") for file in in_files]
    df = pl.concat(dfs)

    if odd:
        df = df.filter(pl.col(odd).mod(2).eq(1))

    if even:
        df = df.filter(pl.col(even).mod(2).eq(0))

    df = df.with_columns(
        sharpness(pl.col("win"), pl.col("loss"), pl.col("draw")).alias("sharpness")
    ).sort("sharpness")

    df.write_csv(out, separator="\t")


def sharpness(w, l, d):
    a = pl.min_horizontal(w, l) / 50
    b = 333 / d
    c = -(w + d) / 1000
    return a * b / (1 + c.exp())


if __name__ == "__main__":
    main()
