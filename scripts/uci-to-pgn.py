#!/usr/bin/env python

import click

import chess.pgn


@click.command()
@click.argument("in-file", type=click.File(mode="r"))
def main(in_file):
    moves = [m.strip() for m in in_file.read().split(" ")]
    game = chess.pgn.Game()
    node = game
    for move in moves:
        node = node.add_variation(chess.Move.from_uci(move))

    print(game)


if __name__ == "__main__":
    main()
