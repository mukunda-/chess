#!/usr/bin/env python

import click

import polars as pl


@click.command()
@click.argument("in-files", nargs=-1, type=click.File(mode="r"))
@click.option("-o", "--out", help="output file", type=click.Path())
def main(in_files, out):
    dfs = [pl.read_csv(file, separator="\t") for file in in_files]

    pl.concat(dfs).write_json(out)


if __name__ == "__main__":
    main()
