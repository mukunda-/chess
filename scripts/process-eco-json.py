#!/usr/bin/env python

import json
import re

import click


@click.command()
@click.argument("in-files", nargs=-1, type=click.File(mode="r"))
@click.option("-o", "--out", help="output file", type=click.Path())
def main(in_files, out):
    rows = []
    for file in in_files:
        rows += json.load(file)

    by_eco = {}
    move_number_re = re.compile(r"^\d+\.")
    for row in rows:
        eco = row["eco"]
        if eco not in by_eco:
            by_eco[eco] = []
        by_eco[eco].append(row)
        row["length"] = len(row["pgn"].split(" "))

    ecos = []
    missing = []
    for eco, lines in by_eco.items():
        candidate = sorted(lines, key=lambda line: line["length"])[0]
        for variation in lines:
            if not variation["pgn"].startswith(candidate["pgn"]):
                missing.append(eco)
                candidate = None
                break

        if candidate is not None:
            ecos.append(candidate)

    print("Missing:")
    for eco in missing:
        print(eco)


if __name__ == "__main__":
    main()
