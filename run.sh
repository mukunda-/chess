#!/usr/bin/env bash

#dotenvx run -- poetry run python -m cProfile ./main.py "$@"
dotenvx run -- poetry run ./main.py "$@"
