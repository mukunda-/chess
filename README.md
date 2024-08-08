# chess-endgame-frequency
Calculates the frequency of each kind of endgame

## Snippets
```
LICHESS_ARCHIVE=lichess_db_standard_rated_2024-07
mkdir data/$LICHESS_ARCHIVE
cd data/$LICHESS_ARCHIVE
wget https://database.lichess.org/standard/$LICHESS_ARCHIVE.pgn.zst
zstdcat $LICHESS_ARCHIVE.pgn.zst | ../../external/pgn-extract/pgn-extract -R ../../external/pgn-extract/roster -#1000000 --minply 3 --tsv 2> progress
```
