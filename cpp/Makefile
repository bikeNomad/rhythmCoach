BUILD_DIR=Default
PGM=$(BUILD_DIR)/rhythmCoach
AUDIO_DIR=../audio
OUTPUT_PNG=combFilter.png
SOURCES=aubio_cpp.h comb_filter.h delay_line.h main.cpp

all: $(PGM)

$(PGM): $(SOURCES)
	@$(MAKE) -C $(BUILD_DIR) rhythmCoach

clean:
	@$(MAKE) -C $(BUILD_DIR) clean

test: $(PGM)
	$(PGM) $(AUDIO_DIR)/excerpt-bass-1.wav $(AUDIO_DIR)/excerpt-kick-1.wav
	mv $(OUTPUT_PNG) test.png
	open test.png

test2: $(PGM)
	$(PGM) $(AUDIO_DIR)/garageband-bass.wav $(AUDIO_DIR)/garageband-drum.wav
	mv $(OUTPUT_PNG) test2.png
	open test2.png

gdb: $(PGM)
	gdb $(PGM)

.PHONY: all clean test rhythmCoach test2 gdb
