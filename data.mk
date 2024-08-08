ECO_CODES = data/openings/a.tsv data/openings/b.tsv data/openings/c.tsv data/openings/d.tsv data/openings/e.tsv
ECO_CODES_ALL = data/openings/eco.json
BOARD_IMAGES_DIR = data/openings/images

data: openings

%.endgames: %.epd
	./bin/release/hello classify-endgames -o $@ $< 

$(ECO_CODES): 
	wget https://raw.githubusercontent.com/lichess-org/chess-openings/master/$(notdir $@) -O $@

data/openings/all.tsv: $(ECO_CODES)
	cat $(ECO_CODES) > data/openings/all.tsv

$(ECO_CODES_ALL): $(ECO_CODES)
	scripts/tsv-to-json.py -o $(ECO_CODES_ALL) $(ECO_CODES)

$(BOARD_IMAGES_DIR): $(ECO_CODES_ALL)
	mkdir $(BOARD_IMAGES_DIR)
	python main.py board-images -o $(BOARD_IMAGES_DIR) $<

openings: $(ECO_CODES_ALL)
