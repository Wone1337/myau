COMPILER = gcc

SOURCES = sources/*.c 

OPTIONS = -std=c23 -Wall  -lSDL3 -lSDL3_ttf -lSDL3_image

BUILD_DIR = build/

app_build:
		$(COMPILER) -o $(BUILD_DIR)arch_app  $(SOURCES) $(OPTIONS) 

app_execute:
		./$(BUILD_DIR)arch_app
