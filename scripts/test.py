#!/usr/bin/env python

import click

import polars as pl


@click.command()
@click.argument("in-files", nargs=-1, type=click.File(mode="r"))
@click.option("-o", "--out", help="output file", type=click.Path())
def main(in_files, out):
    seen = set()
    for file in in_files:
        for line in file:
            count = str.count(line, "\t")
            if count not in seen:
                print(line)
                seen.add(count)

    print(seen)


if __name__ == "__main__":
    main()
