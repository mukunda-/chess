#!/usr/bin/env python

import click

import polars as pl
import sys


@click.command()
@click.argument("in-files", nargs=-1, type=click.File(mode="r"))
@click.option("-o", "--out", help="output file", type=click.Path())
@click.option("-m", "--moves-col", help="the moves columns")
def main(in_files, moves_col, out):
    if out is None:
        out = sys.stdout

    if not in_files:
        return

    dfs = [pl.read_csv(file, separator="\t") for file in in_files]
    df = pl.concat(dfs)

    df = df.with_columns(pl.col(moves_col).str.split(" ").list.len().alias("ply"))
    df.write_csv(out, separator="\t")


if __name__ == "__main__":
    main()
