#!/usr/bin/env python

from io import StringIO
import os
from dataclasses import dataclass

import click
import psycopg
import polars as pl
import chess
import chess.pgn

config = {k: os.environ[k] for k in ["DB_USERNAME", "DB_PASSWORD", "DB_NAME"]}


@dataclass
class LineProbability:
    line: tuple
    board_hash: int = 0
    prob_for_white: float = 1
    prob_for_black: float = 1


@dataclass
class Opening:
    name: str | None


def read_opening_book(path: str):
    df = pl.read_csv(path, separator="\t").select("name", "pgn")
    book = {}
    for name, pgn in df.iter_rows():
        pgn_stream = StringIO(pgn)
        game = chess.pgn.read_game(pgn_stream)
        if game is None:
            break

        line = tuple()
        for move in game.mainline_moves():
            line = line + tuple([move.uci()])
            if book.get(line) is None:
                book[line] = None

        book[line] = name

    return book


def should_prune(book, line: LineProbability):
    return line.line not in book or (
        line.prob_for_white < 0.01 and line.prob_for_black < 0.01
    )


@click.command()
@click.argument("prune-path", type=click.Path(exists=True))
def main(prune_path: str):
    db_uri = f"postgresql://{config['DB_USERNAME']}:{config['DB_PASSWORD']}@127.0.0.1:5432/{config['DB_NAME']}"

    book = read_opening_book(prune_path)

    with psycopg.connect(db_uri, autocommit=True) as conn:
        with conn.cursor() as cur:
            print_headers()
            calc_book_chances(cur, book, 15)


def calc_book_chances(cur, book, depth: int):
    cur.execute(
        f"select next_board_hash, move_uci, count(*) from continuations where ply = 1 group by 1, 2"
    )

    probs = read_probabilities(cur)
    probs = [p for p in probs if not should_prune(book, p)]
    for prob in probs:
        print_row(book, prob)

    todo = probs
    for _ in range(depth - 1):
        next_todo = []
        for prob in todo:
            children = [
                p for p in read_children(cur, prob) if not should_prune(book, p)
            ]
            next_todo.extend(children)

            for prob in children:
                print_row(book, prob)

        todo = next_todo


def read_probabilities(
    cur, line: LineProbability | None = None
) -> list[LineProbability]:
    if line is None:
        line = LineProbability(line=tuple())

    counts_by_move = {}
    board_hash_by_move = {}
    total = 0
    for board_hash, move_uci, count in cur:
        counts_by_move[move_uci] = count
        board_hash_by_move[move_uci] = board_hash
        total += count

    probs = []
    for move, count in counts_by_move.items():
        ratio = count / total

        if len(line.line) % 2 == 0:
            prob_for_black = ratio
            prob_for_white = 1
        else:
            prob_for_black = 1
            prob_for_white = ratio

        prob = LineProbability(
            line=line.line + tuple([move]),
            board_hash=board_hash_by_move[move],
            prob_for_black=line.prob_for_black * prob_for_black,
            prob_for_white=line.prob_for_white * prob_for_white,
        )

        probs.append(prob)

    return probs


def print_headers():
    print("ply", "opening", "line", "prob_for_white", "prob_for_black", sep="\t")


def print_row(book, prob: LineProbability):
    opening_name = book.get(prob.line)
    if book.get(prob.line) is None:
        return

    print(
        len(prob.line),
        opening_name,
        " ".join(prob.line),
        prob.prob_for_white,
        prob.prob_for_black,
        sep="\t",
    )


def read_children(cur, line: LineProbability):
    cur.execute(
        f"select next_board_hash, move_uci, count(*) from continuations where last_board_hash = %s group by 1, 2",
        [
            line.board_hash,
        ],
    )

    return read_probabilities(cur, line)


if __name__ == "__main__":
    main()
