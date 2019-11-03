BUILD_DIR=Default
PGM=$(BUILD_DIR)/rhythmCoach
AUDIO_DIR=../audio
OUTPUT_PNG=combFilter.png
SOURCES=aubio_cpp.h comb_filter.h delay_line.h main.cpp source.h
WIN_SIZE?=1024
OPTS1?=
OPTS2?=
X?=3

all: $(PGM)

$(PGM): $(SOURCES)
	@$(MAKE) -C $(BUILD_DIR) rhythmCoach

clean:
	@$(MAKE) -C $(BUILD_DIR) clean

test1: $(PGM)
	$(PGM) -w $(WIN_SIZE) -o excerpt-bass-1.png $(AUDIO_DIR)/excerpt-bass-1.wav \
		-w $(WIN_SIZE) -o excerpt-kick-1.png $(AUDIO_DIR)/excerpt-kick-1.wav
	open excerpt-*-1.png

test2: $(PGM)
	$(PGM) -w $(WIN_SIZE) -o garageband-bass.png $(AUDIO_DIR)/garageband-bass.wav \
		-w $(WIN_SIZE) -o garageband-drum.png $(AUDIO_DIR)/garageband-drum.wav
	open garageband-*.png

test3: $(PGM)
	$(PGM) -w $(WIN_SIZE) -o excerpt-bass-2.png $(AUDIO_DIR)/excerpt-bass-2.wav \
		-w $(WIN_SIZE) -o excerpt-kick-2.png $(AUDIO_DIR)/excerpt-kick-2.wav
	open excerpt-*-2.png

test: $(PGM)
	$(PGM) -w $(WIN_SIZE) $(OPTS1) -o excerpt-bass-$(X).png $(AUDIO_DIR)/excerpt-bass-$(X).wav \
		-w $(WIN_SIZE) $(OPTS2) -o excerpt-kick-$(X).png $(AUDIO_DIR)/excerpt-kick-$(X).wav
	open excerpt-*-$(X).png

help: $(PGM)
	@-$(PGM) -h

gdb: $(PGM)
	gdb $(PGM)

format: $(SOURCES)
	clang-format -i -style=file $(SOURCES)

.PHONY: all clean test rhythmCoach test2 gdb
