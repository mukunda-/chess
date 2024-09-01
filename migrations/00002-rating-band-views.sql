DROP VIEW IF EXISTS game_details CASCADE;
CREATE VIEW game_details AS
SELECT 
    id as game_id,
    result,
    games.white_elo::int as white_elo,
    games.black_elo::int as black_elo,
    CASE 
        WHEN white_elo::int < 1035 THEN '0-300'
        WHEN white_elo::int < 1100 THEN '300-400'
        WHEN white_elo::int < 1165 THEN '400-500'
        WHEN white_elo::int < 1225 THEN '500-600'
        WHEN white_elo::int < 1290 THEN '600-700'
        WHEN white_elo::int < 1350 THEN '700-800'
        WHEN white_elo::int < 1415 THEN '800-900'
        WHEN white_elo::int < 1475 THEN '900-1000'
        WHEN white_elo::int < 1575 THEN '1000-1100'
        WHEN white_elo::int < 1675 THEN '1100-1200'
        WHEN white_elo::int < 1750 THEN '1200-1300'
        WHEN white_elo::int < 1825 THEN '1300-1400'
        WHEN white_elo::int < 1900 THEN '1400-1500'
        WHEN white_elo::int < 2000 THEN '1500-1600'
        WHEN white_elo::int < 2075 THEN '1600-1700'
        WHEN white_elo::int < 2150 THEN '1700-1800'
        WHEN white_elo::int < 2225 THEN '1800-1900'
        WHEN white_elo::int < 2300 THEN '1900-2000'
        WHEN white_elo::int < 2375 THEN '2000-2100'
        WHEN white_elo::int < 2450 THEN '2100-2200'
        WHEN white_elo::int < 2525 THEN '2200-2300'
        WHEN white_elo::int < 2600 THEN '2300-2400'
        ELSE '2400+'
    END AS white_cohort,
    CASE 
        WHEN black_elo::int < 1035 THEN '0-300'
        WHEN black_elo::int < 1100 THEN '300-400'
        WHEN black_elo::int < 1165 THEN '400-500'
        WHEN black_elo::int < 1225 THEN '500-600'
        WHEN black_elo::int < 1290 THEN '600-700'
        WHEN black_elo::int < 1350 THEN '700-800'
        WHEN black_elo::int < 1415 THEN '800-900'
        WHEN black_elo::int < 1475 THEN '900-1000'
        WHEN black_elo::int < 1575 THEN '1000-1100'
        WHEN black_elo::int < 1675 THEN '1100-1200'
        WHEN black_elo::int < 1750 THEN '1200-1300'
        WHEN black_elo::int < 1825 THEN '1300-1400'
        WHEN black_elo::int < 1900 THEN '1400-1500'
        WHEN black_elo::int < 2000 THEN '1500-1600'
        WHEN black_elo::int < 2075 THEN '1600-1700'
        WHEN black_elo::int < 2150 THEN '1700-1800'
        WHEN black_elo::int < 2225 THEN '1800-1900'
        WHEN black_elo::int < 2300 THEN '1900-2000'
        WHEN black_elo::int < 2375 THEN '2000-2100'
        WHEN black_elo::int < 2450 THEN '2100-2200'
        WHEN black_elo::int < 2525 THEN '2200-2300'
        WHEN black_elo::int < 2600 THEN '2300-2400'
        ELSE '2400+'
    END AS black_cohort,
    CASE
        WHEN position('Rapid' in games.event) > 0 THEN 'rapid'
        WHEN position('Classical' in games.event) > 0 THEN 'classical'
        WHEN position('Blitz' in games.event) > 0 THEN 'blitz'
        WHEN position('Bullet' in games.event) > 0 THEN 'bullet'
        WHEN position('Correspondence' in games.event) > 0 THEN 'bullet'
        ELSE '?'
    END as time_class,
    abs(white_elo::int - black_elo::int) as rating_gap
FROM games;

DROP VIEW IF EXISTS endgame_results_white;
CREATE VIEW endgame_results_white AS
select
    white_cohort,
    endgame,
    sum(case when result = '1-0' then 1 else 0 end) as wins,
    sum(case when result = '1/2-1/2' then 1 else 0 end) as draws,
    sum(case when result = '0-1' then 1 else 0 end) as losses,
    count(*) as total
from game_details, endgames
where
    endgames.game_id = game_details.game_id
group by 1, 2
order by max(white_elo);

DROP VIEW IF EXISTS endgame_result_ratios_white;
CREATE VIEW endgame_result_ratios_white AS
select
    white_cohort,
    endgame,
    wins / total::decimal as win_ratio,
    draws / total::decimal  as draw_ratio,
    losses / total::decimal  as loss_ratio,
    total
from endgame_results_white 
order by endgame;
