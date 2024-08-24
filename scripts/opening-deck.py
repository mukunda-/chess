import json
import sys
import re
import io
import os

import chess.pgn
import chess.svg
import chess

import click

from tqdm import tqdm


@click.command("board-images", help="build openings book")
@click.argument("in-file", type=click.File())
@click.option("-o", "--out", type=click.Path(exists=True), required=True)
def main(in_file, out):
    unsafe_chars_re = re.compile(r"[^\w]")
    manifest = []

    rows = json.load(in_file)
    for row in tqdm(rows):
        pgn = io.StringIO(row["pgn"] + "\n\n")
        game = chess.pgn.read_game(pgn)
        if not game:
            print("Failed on", row)
            sys.exit(1)

        board = game.board()
        for move in game.mainline_moves():
            board.push(move)

        svg = chess.svg.board(board)

        filename = unsafe_chars_re.sub("_", board.epd()) + ".svg"
        path = os.path.join(out, filename)
        with open(path, "w") as f:
            f.write(svg)

        opening = {
            "image_path": path,
            "pgn": row["pgn"],
            "name": row["name"],
        }

        manifest.append(opening)

    manifest_path = os.path.join(out, "manifest.json")
    with open(manifest_path, "w") as f:
        json.dump(manifest, f)


if __name__ == "__main__":
    main()
