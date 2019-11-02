BUILD_DIR=Default
PGM=$(BUILD_DIR)/rhythmCoach
AUDIO_DIR=../audio
OUTPUT_PNG=combFilter.png
SOURCES=aubio_cpp.h comb_filter.h delay_line.h main.cpp source.h

all: $(PGM)

$(PGM): $(SOURCES)
	@$(MAKE) -C $(BUILD_DIR) rhythmCoach

clean:
	@$(MAKE) -C $(BUILD_DIR) clean

test: $(PGM)
	$(PGM) -o excerpt-bass-1.png $(AUDIO_DIR)/excerpt-bass-1.wav \
		-o excerpt-kick-1.png $(AUDIO_DIR)/excerpt-kick-1.wav
	open excerpt-*-1.png

test2: $(PGM)
	$(PGM) -o garageband-bass.png $(AUDIO_DIR)/garageband-bass.wav \
		-o garageband-drum.png $(AUDIO_DIR)/garageband-drum.wav
	open garageband-*.png

gdb: $(PGM)
	gdb $(PGM)

.PHONY: all clean test rhythmCoach test2 gdb
