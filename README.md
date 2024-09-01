# chess-endgame-frequency
Calculates the frequency of each kind of endgame

## Snippets

```bash
YEAR=2024
MONTH=07
LICHESS_ARCHIVE=lichess_db_standard_rated_$YEAR\_$MONTH
mkdir -p data/$LICHESS_ARCHIVE
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

```sql
select
    games.site,
    continuations.next_board_hash,
    b.stockfish_eval - a.stockfish_eval as eval_diff,
    count(*) c
from
    continuations,
    games,
    boards a,
    boards b
where
    continuations.last_board_hash = a.hash and
    continuations.next_board_hash = b.hash and
    games.id = continuations.game_id and
    a.stockfish_eval is not null and
    b.stockfish_eval is not null
order by eval_diff asc, c desc;
```

```sql
select
    continuations.last_board_hash,
    continuations.move_uci,
    boards.stockfish_cp_score,
    boards.stockfish_mate_score
from
    continuations,
    boards
where
    continuations.last_board_hash = boards.hash
```

We want opening sequences that result in a loss of X centipawns.

First we need to get entire games sequences

```sql
select 
    games.id,
    GROUP_CONCAT ( DISTINCT mov_uci ORDER BY ply  SEPARATOR ' ')  as "Move UCIs" 
from continuations, games
where continuations.game_id = game.id;

```sql
select
    continuations.ply,
    count(*) as c
from
    continuations,
    boards
where
    continuations.last_board_hash = boards.hash and
    (boards.stockfish_cp_score is not null or
    boards.stockfish_mate_score is not null)
group by 1
order by ply asc
```

# Scratch

Read an initial position. Choose the top three most common responses in the opening book. Print if terminal. Continue.

select next_board_hash, move_uci, count(*) from continuations where ply = 1 group by 1, 2 order by count desc limit 4;

```sql
select
    next_board_hash,
    move_uci,
    count(*)
from continuations
where ply = 1
group by 1, 2
order by count desc
limit 3;

select
    next_board_hash,
    move_uci,
    count(*)
from continuations
where ply = 2 and last_board_hash in ('', '')
group by 1, 2
order by count desc
limit 2;

select
    next_board_hash,
    move_uci,
    count(*)
from continuations
where ply = 3 and last_board_hash in ('', '')
group by 1, 2
order by count desc
limit 2;

select
    next_board_hash,
    move_uci,
    count(*)
from continuations
where ply = 4 and last_board_hash in ('', '')
group by 1, 2
order by count desc
limit 2;

select
    next_board_hash,
    move_uci,
    count(*)
from continuations
where ply = 5 and last_board_hash in ('', '')
group by 1, 2
order by count desc
limit 2;

select
    next_board_hash,
    move_uci,
    count(*)
from continuations
where ply = 6 and last_board_hash in ('', '')
group by 1, 2
order by count desc
limit 2;
```

```bash
find data/openings/text/ -type f -name "*.txt" -exec dotenvx run -- poetry run scripts/tts.py {} -o {}.mp3 ";"
```

```bash
sox -n -r 44100 -c 2 silence-2s.wav trim 0.0 2.0
```
