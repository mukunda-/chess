#!/usr/bin/env python

import click
from pathlib import Path
from openai import OpenAI


@click.command()
@click.argument("in-file", type=click.File(mode="r"))
@click.option("-o", "--out-path", help="output file", type=click.Path())
@click.option(
    "-m", "--multi-voice", help="produce one of each voice", default=False, type=bool
)
@click.option("-v", "--voice", help="the voice to use", default="nova")
def main(in_file, out_path, multi_voice, voice):
    text = "[pause]... " + in_file.read()

    if multi_voice:
        voices = ["alloy", "echo", "fable", "onyx", "nova", "shimmer"]
        for voice in voices:
            render(text, out_path + "." + voice + ".wav", voice)
    else:
        render(text, out_path, voice)


def render(text, out_path, voice):
    client = OpenAI()

    response = client.audio.speech.create(
        model="tts-1-hd",
        voice=voice,
        response_format="wav",
        input=text,
    )

    response.stream_to_file(out_path)


if __name__ == "__main__":
    main()
