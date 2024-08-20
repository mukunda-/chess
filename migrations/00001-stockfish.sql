alter table boards drop column stockfish_cp_score;
alter table boards drop column stockfish_mate_score;
alter table boards add column stockfish_eval smallint;
