#!/usr/bin/env python

import random


words = [
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
    "ten",
    "elevent",
    "twelve",
]


@click.command()
@click.option("-o", "--out-path", help="output file", type=click.Path())
@click.option("-c", "--count", help="the number of random numbers", type=click.Path())
def main(out_path, count):
    for _ in range(count):
        idx = random.randint(1, 12)
        print(words[idx])


if __name__ == "__main__":
    main()
