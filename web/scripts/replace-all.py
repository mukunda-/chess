#!/usr/bin/env python

# Core
import re
import fileinput
import sys
import pathlib

# Click
import click


INDENT = " " * 4


def replace_inplace(path, regex, replacement, write):
    loglines = []
    for line in fileinput.input(path, inplace=1):
        line = line.rstrip("\n")
        updated_line = regex.sub(replacement, line).rstrip("\n")

        if line != updated_line:
            loglines.append("".join([" " * 4, line]))
            loglines.append("".join([" ", "->", " ", updated_line]))

            if write:
                print(updated_line)
                continue
        print(line)

    if loglines:
        print(f"### {path} ###")
        for line in loglines:
            print(line)
        print()


@click.command()
@click.argument("regex_str")
@click.argument("replacement")
@click.option("--write", is_flag=True, show_default=True, default=False)
def main(regex_str, replacement, write):
    regex = re.compile(regex_str)
    for line in sys.stdin:
        path = pathlib.Path(line.strip())
        if path.is_dir():
            continue
        if not path.exists():
            print("Error! Non-existent path specified:", path)
        replace_inplace(path, regex, replacement, write)


if __name__ == "__main__":
    main()
