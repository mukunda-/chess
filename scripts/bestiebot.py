#!/usr/bin/env python

from dataclasses import dataclass
import asyncio
from functools import wraps
import math
import sys

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
    engine: chess.engine.UciProtocol
    game: chess.pgn.Game
    game_node: chess.pgn.GameNode


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
    candidates = await get_candidates(app.game_node.board(), app.engine)
    candidate = select_candidate(candidates)

    return candidate.move

def is_game_over(app: App) -> bool:
    return app.game_node.board().is_game_over()

def is_draw(app: App) -> bool:
    return app.game_node.board().can_claim_draw()

def declare_draw(app: App) -> None:
    app.game.headers["Result"] = "1-2/1-2"

def push_move(app: App, move: chess.Move) -> None:
    app.game_node = app.game_node.add_variation(move)

async def dispatch(app: App, cmd: Command) -> App:
    if cmd.name == "m":
        try:
            push_move(app, chess.Move.from_uci(cmd.args[0]))
        except chess.IllegalMoveError:
            print("Illegal move")
    elif cmd.name == "n":
        move = await get_bot_move(app)
        push_move(app, move)
    elif cmd.name == "q":
        app.is_running = False
    else:
        print("Unrecognized command")

    if is_draw(app):
        print("Draw!")
        declare_draw(app)
        app.is_running = False

    if is_game_over(app):
        print("Game over!")
        app.is_running = False

    print(app.game)

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
        eval = await engine.analyse(board, chess.engine.Limit(nodes=3000))

        if "score" not in eval:
            continue

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
    line = sys.stdin.readline()
    print(line)

    return None

def write_move(move: chess.Move):
    result = chess.engine.PlayResult(move, None)
    print(result)
    

@click.command()
@click.argument("engine_path", type=click.Path(exists=True))
@coro
async def main(engine_path):
    _, engine = await chess.engine.popen_uci([engine_path, "--backend=cuda-auto", "--threads=8"])

    game = chess.pgn.Game()
    game_node = game
    app = App(is_running=True, engine=engine, game=game, game_node=game_node)
    while app.is_running:
        get_command()

        candidates = await get_candidates(game_node.board(), app.engine)
        candidate = select_candidate(candidates)

        write_move(candidate.move)

    await engine.quit()


if __name__ == "__main__":
    main()
