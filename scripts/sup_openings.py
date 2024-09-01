#!/usr/bin/env python

import dataclasses
import io
from pathlib import Path
import sys
import re
from dataclasses import dataclass

from chess import PIECE_TYPES
import chess
import click
import polars as pl
import chess.pgn
from tqdm import tqdm

LIGHT_SQUARES = set(
    s for s in chess.SQUARES if chess.square_rank(s) + chess.square_file(s) % 2 != 0
)
DARK_SQUARES = set(s for s in chess.SQUARES if s not in LIGHT_SQUARES)


PIECE_NAMES = {
    chess.BISHOP: "bishop",
    chess.KNIGHT: "knight",
    chess.ROOK: "rook",
    chess.KING: "king",
    chess.QUEEN: "queen",
    chess.PAWN: "pawn",
}


def square_to_words(square):
    return " ".join(list(chess.square_name(square)))


def piece_to_words(square, piece):
    return f"{PIECE_NAMES[piece.piece_type]} on {square_to_words(square)}"


@dataclass
class BoardAnalysis:
    square_control: dict[chess.Square, int]


@dataclass
class BoardAnalysisDiff:
    became_defended_black: dict[chess.Square, bool] = dataclasses.field(
        default_factory=dict
    )
    became_attacked_black: dict[chess.Square, bool] = dataclasses.field(
        default_factory=dict
    )
    became_defended_white: dict[chess.Square, bool] = dataclasses.field(
        default_factory=dict
    )
    became_attacked_white: dict[chess.Square, bool] = dataclasses.field(
        default_factory=dict
    )


def analyze_board(board: chess.Board) -> BoardAnalysis:
    return BoardAnalysis(
        square_control={
            square: board.attackers(chess.WHITE, square)
            - board.attackers(chess.BLACK, square)
        }
    )


def diff_board_analysis(a: BoardAnalysis, b: BoardAnalysis) -> BoardAnalysisDiff:
    diff = BoardAnalysisDiff()

    for square in chess.SQUARES:
        attackers = square_control.get(square, 0)


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

    df = df.select("name", "family", "lvl_1", "lvl_2", "lvl_3", "pgn", "ply")

    # df.write_csv(out, separator="\t")

    df_family = df.group_by("family").len(name="count").sort("count")

    # df_family.write_csv(sys.stdout, separator="\t")

    notations = set()

    for name, pgn in tqdm(zip(df["name"], df["pgn"])):
        game = chess.pgn.read_game(io.StringIO(pgn))
        if not game:
            raise "womp womp???"

        board = chess.Board()
        descriptions = []
        for move in game.mainline_moves():
            description = []

            player = board.turn
            notations.add(board.san(move))
            board.push(move)

            to_square = move.to_square
            piece = board.piece_at(to_square)
            if not piece:
                raise "wtf"

            piece_type = piece.piece_type

            move_text = "White" if player == chess.WHITE else "Black"
            move_text += " plays "

            piece_name = PIECE_NAMES[piece_type]
            square_name = square_to_words(to_square)
            square_color_text = "light" if to_square in LIGHT_SQUARES else "dark"
            square_color_text += " square"
            move_text += f"{piece_name} to the {square_color_text} {square_name}"

            if board.is_check():
                move_text += " check"

            move_text += "."

            # TODO: Get a map of attackers counts per square and defender counts per square
            # Speak the difference.
            defended = []
            attacked = []
            for seen_square in board.attacks(to_square):
                seen_piece = board.piece_at(seen_square)
                if not seen_piece:
                    continue

                if seen_piece.color == player:
                    defended.append((seen_square, seen_piece))
                else:
                    attacked.append((seen_square, seen_piece))

            attacks_desc = ", and ".join(
                f"the {piece_to_words(attacked_square, attacked_piece)}"
                for attacked_square, attacked_piece in attacked
            )

            defends_desc = ", and ".join(
                f"the {piece_to_words(defended_square, defended_piece)}"
                for defended_square, defended_piece in defended
            )

            if attacks_desc or defends_desc:
                move_text += "\n\n"
                move_text += "Of this move we can say the following:\n"

                if attacks_desc:
                    move_text += "* It attacks " + attacks_desc + ".\n"

                if defends_desc:
                    move_text += "* It defends " + defends_desc + ".\n"

            descriptions.append(move_text)

        dir = Path("data/openings/text") / Path(name)
        dir.mkdir(exist_ok=True, parents=True)
        header_path = "".zfill(5) + ".txt"
        with open(dir / header_path, "w") as f:
            f.write(name + ".\n")

        for idx, description in enumerate(descriptions):
            with open(dir / f"{str(idx + 1).zfill(5)}.txt", "w") as f:
                f.write(description + "\n")

        with open(out, "w") as f:
            f.write("\n".join(list(notations)))


if __name__ == "__main__":
    main()
