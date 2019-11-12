BUILD_DIR=Default
PGM=$(BUILD_DIR)/rhythmCoach
AUDIO_DIR=../audio
OUTPUT_PNG=combFilter.png
SOURCES=aubio_cpp.h comb_filter.h delay_line.h main.cpp source.h
X?=3
SM?=11

all: $(PGM)

$(PGM): $(SOURCES)
	@$(MAKE) -C $(BUILD_DIR) rhythmCoach

clean:
	@$(MAKE) -C $(BUILD_DIR) clean

test1: $(PGM)
	$(PGM)  -x $(SM) -o excerpt-bass-1.png $(AUDIO_DIR)/excerpt-bass-1.wav \
		 -x $(SM) -o excerpt-kick-1.png $(AUDIO_DIR)/excerpt-kick-1.wav
	open excerpt-*-1.png

test2: $(PGM)
	$(PGM)  -x $(SM) -o garageband-bass.png $(AUDIO_DIR)/garageband-bass.wav \
		 -x $(SM) -o garageband-drum.png $(AUDIO_DIR)/garageband-drum.wav
	open garageband-*.png

test3: $(PGM)
	$(PGM)  -x $(SM) -o excerpt-bass-2.png $(AUDIO_DIR)/excerpt-bass-2.wav \
		 -x $(SM) -o excerpt-kick-2.png $(AUDIO_DIR)/excerpt-kick-2.wav
	open excerpt-*-2.png

test: $(PGM)
	$(PGM)  -x $(SM) -o excerpt-bass-$(X).png $(AUDIO_DIR)/excerpt-bass-$(X).wav \
		 -x $(SM) -o excerpt-kick-$(X).png $(AUDIO_DIR)/excerpt-kick-$(X).wav
	open excerpt-*-$(X).png

help: $(PGM)
	@-$(PGM) -h

gdb: $(PGM)
	gdb $(PGM)

format: $(SOURCES)
	clang-format -i -style=file $(SOURCES)

.PHONY: all clean test rhythmCoach test2 gdb
