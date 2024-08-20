CREATE TABLE IF NOT EXISTS games (
    id serial PRIMARY KEY,
    white text,
    white_elo text,
    white_fide_id text,
    white_rating_diff text,
    white_team text,
    white_title text,
    black text,
    black_elo text,
    black_fide_id text,
    black_rating_diff text,
    black_team text,
    black_title text,
    annotator text,
    board text,
    date text,
    eco text,
    event text,
    opening text,
    result text,
    round text,
    site text NOT NULL UNIQUE,
    termination text,
    time_control text,
    utc_date text,
    utc_time text
);

CREATE TABLE IF NOT EXISTS endgames (
    game_id integer NOT NULL REFERENCES games (id),
    endgame text NOT NULL,
    PRIMARY KEY (game_id, endgame)
);

CREATE TABLE IF NOT EXISTS boards (
    hash numeric not null unique primary key,
    white_bishops bit(64) not null,
    white_rooks bit(64) not null,
    white_queens bit(64) not null,
    white_knights bit(64) not null,
    white_king bit(64) not null,
    white_pawn bit(64) not null,
    black_bishops bit(64) not null,
    black_rooks bit(64) not null,
    black_queens bit(64) not null,
    black_knights bit(64) not null,
    black_king bit(64) not null,
    black_pawn bit(64) not null,
    white_to_move boolean not null,
    castle_rights varchar(10) not null,
    enpassant_sq smallint not null,
    stockfish_cp_score int,
    stockfish_mate_score int
);

CREATE TABLE IF NOT EXISTS continuations (
    game_id integer not null,
    ply smallint not null,
    last_board_hash numeric not null,
    next_board_hash numeric not null,
    move_uci varchar(10) NOT NULL,
    FOREIGN KEY (game_id) REFERENCES games (id),
    FOREIGN KEY (last_board_hash) REFERENCES boards (hash),
    PRIMARY KEY (game_id, last_board_hash, move_uci, ply)
);
