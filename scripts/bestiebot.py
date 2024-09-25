#!/usr/bin/env python

from dataclasses import dataclass
import asyncio
from functools import wraps
import math

import chess
import chess.engine
import chess.pgn

import click


def coro(f):
    @wraps(f)
    def wrapper(*args, **kwargs):
        return asyncio.run(f(*args, **kwargs))

    return wrapper


@dataclass
class App:
    is_running: bool
    board: chess.Board
    engine: chess.engine.UciProtocol


@dataclass
class Candidate:
    move: chess.Move
    eval: chess.engine.InfoDict
    sharpness: float


@dataclass
class Command:
    name: str
    args: list[str]


async def get_bot_move(app: App) -> chess.Move:
    candidates = await get_candidates(app.board, app.engine)
    candidate = select_candidate(candidates)

    return candidate.move


async def dispatch(app: App, cmd: Command) -> App:
    if cmd.name == "m":
        try:
            app.board.push_san(cmd.args[0])
        except chess.IllegalMoveError:
            print("Illegal move")
    elif cmd.name == "n":
        move = await get_bot_move(app)
        app.board.push(move)
    elif cmd.name == "q":
        app.is_running = False
    else:
        print("Unrecognized command")

    print(get_pgn(app.board.move_stack))

    return app


def get_pgn(moves: list[chess.Move]) -> chess.pgn.Game:
    game = chess.pgn.Game()
    node = game
    for move in moves:
        node = node.add_variation(move)

    return game


async def get_candidates(
    board: chess.Board, engine: chess.engine.UciProtocol
) -> list[Candidate]:
    candidates = []
    for move in board.legal_moves:
        board.push(move)
        eval = await engine.analyse(board, chess.engine.Limit(nodes=1000))
        wdl = eval["score"].wdl().white()
        win = wdl.wins
        loss = wdl.losses
        draw = wdl.draws
        if win == 0 or loss == 0:
            sharpness = 0
        else:
            sharpness = (
                (min(win, loss) / 50)
                * (333 / draw)
                * (1 / (1 + math.exp(-(win + loss) / 1000)))
            )

        candidates.append(Candidate(move=move, eval=eval, sharpness=sharpness))
        board.pop()

    return candidates


def select_candidate(candidates: list[Candidate]) -> Candidate:
    selected = candidates[0]
    for candidate in candidates[1:]:
        if candidate.sharpness > selected.sharpness:
            selected = candidate

    return selected


def get_command():
    cmd, *args = input("(m)ove, n(ext), q(uit): ").split(" ")

    return Command(name=cmd, args=args)


@click.command()
@click.argument("engine_path", type=click.Path(exists=True))
@coro
async def main(engine_path):
    _, engine = await chess.engine.popen_uci([engine_path, "--backend=cuda-auto"])

    app = App(is_running=True, board=chess.Board(), engine=engine)
    while app.is_running:
        # cmd = get_command()
        # app = await dispatch(app, cmd)
        app = await dispatch(app, Command(name="n", args=[]))

    await engine.quit()


if __name__ == "__main__":
    main()
