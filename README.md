# Research Mono Repo

I have decided to live out the remainder of my years meaningfully.

This is part of a long standing effort to group my research projects.

Eventually I would like to pour the last 10 years of my work as a computer scientist into this repo.

As well as the next 10! (from the date of you reading this - help me live forever).

## Pgne3k - Chess PGN to TSV Conversion

Convert Chess games stored in PGNs to TSVs (CSVs with tabs instead of comas). Supports extracting clock times and filtering on minimum ply and tag values.

Supports filtering games based on a "tag specification" which describes what PGN tags to include and what values they are allowed to have for a game to be included.

Here is an example tagspec.

```
Site
WhiteElo
BlackElo
TimeControl
Ply > 3
Result
WhiteClock
BlackClock
WhiteTitle ! BOT
BlackTitle ! BOT
Event - Bullet
Result ! *
Moves
```

In addition to including the given tags and pseudo-tags (like "Moves" and "WhiteClock"), the following contract must be satisfied for a game to be printed.

- More than 3 moves were played.
- Don't include Lichess Bot accounts.
- Exclude events with the word "Bullet" in them.
- Exclude incomplete games.
