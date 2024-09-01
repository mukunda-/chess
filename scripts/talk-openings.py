#!/usr/bin/env python

import os
import glob
import subprocess

import click
from pathlib import Path

HEADER_SILENCE = "./data/util/silence-2s.mp3"
MOVE_SILENCE = "./data/util/silence-1s.mp3"


@click.command()
@click.argument("in-dirs", nargs=-1, type=click.Path(exists=True))
def main(in_dirs):
    for dir in [Path(d) for d in in_dirs]:
        paths = list(sorted(glob.glob(str(dir / "*.mp3"))))
        header_audio, *moves_audio = paths

        args = [
            MOVE_SILENCE,
            header_audio,
            HEADER_SILENCE,
        ]

        for move in moves_audio:
            args.append(move)
            args.append(MOVE_SILENCE)

        args.pop()
        subprocess.run(["mplayer", *args])


if __name__ == "__main__":
    main()
