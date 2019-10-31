BUILD_DIR=Default
PGM=$(BUILD_DIR)/rhythmCoach

all: $(PGM)

$(PGM):
	@$(MAKE) -C $(BUILD_DIR) rhythmCoach

clean:
	@$(MAKE) -C $(BUILD_DIR) clean

test: $(PGM)
	$(PGM) ../audio/excerpt-bass-1.wav ../audio/excerpt-kick-1.wav

.PHONY: all clean test rhythmCoach
