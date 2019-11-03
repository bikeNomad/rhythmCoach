BUILD_DIR=Default
PGM=$(BUILD_DIR)/rhythmCoach
AUDIO_DIR=../audio
OUTPUT_PNG=combFilter.png
SOURCES=aubio_cpp.h comb_filter.h delay_line.h main.cpp source.h
WIN_SIZE?=1024

all: $(PGM)

$(PGM): $(SOURCES)
	@$(MAKE) -C $(BUILD_DIR) rhythmCoach

clean:
	@$(MAKE) -C $(BUILD_DIR) clean

test: $(PGM)
	$(PGM) -w $(WIN_SIZE) -o excerpt-bass-1.png $(AUDIO_DIR)/excerpt-bass-1.wav \
		-w $(WIN_SIZE) -o excerpt-kick-1.png $(AUDIO_DIR)/excerpt-kick-1.wav
	open excerpt-*-1.png

test2: $(PGM)
	$(PGM) -w $(WIN_SIZE) -o garageband-bass.png $(AUDIO_DIR)/garageband-bass.wav \
		-w $(WIN_SIZE) -o garageband-drum.png $(AUDIO_DIR)/garageband-drum.wav
	open garageband-*.png

test3: $(PGM)
	$(PGM) -w $(WIN_SIZE) -t 0.5 -o test3-t5.png $(AUDIO_DIR)/garageband-bass.wav \
		-w $(WIN_SIZE) -t 0.6 -o test3-t6.png $(AUDIO_DIR)/garageband-bass.wav 
	open test3*.png

gdb: $(PGM)
	gdb $(PGM)

format: $(SOURCES)
	clang-format -i -style=file $(SOURCES)

.PHONY: all clean test rhythmCoach test2 gdb
