# chess-endgame-frequency
Calculates the frequency of each kind of endgame

## Snippets

```bash
LICHESS_ARCHIVE=lichess_db_standard_rated_2024-07
mkdir data/$LICHESS_ARCHIVE
cd data/$LICHESS_ARCHIVE
wget https://database.lichess.org/standard/$LICHESS_ARCHIVE.pgn.zst
zstdcat $LICHESS_ARCHIVE.pgn.zst | ../../external/pgn-extract/pgn-extract -R ../../external/pgn-extract/roster -#1000000 --minply 3 --tsv -t tag-criteria.txt  --tagsubstr -Wuci 2> progress
```

And then in a different pane

```bash
tail -f progress tqdm --total 90106180 > /dev/null
```

```sql
select
    sum(pg_column_size(hash)) as total_size,
    avg(pg_column_size(hash)) as average_size,
    sum(pg_column_size(hash)) * 100.0 / pg_total_relation_size('boards') as percentage
from t;
```
