COMPILER = gcc

SOURCES = sources/*.c 

OPTIONS = -std=c2x -Wall  -lSDL3 -lSDL3_ttf -lSDL3_image `pkg-config --cflags --libs gtk+-3.0`

BUILD_DIR = build/

CURRENT_LOG_TIME := $(shell date +'%Y-%m-%d_%H-%M-%S')

CURRENT_LOG_NAME = app_execution_$(CURRENT_LOG_TIME).log

LOG_DIR = logs/

app_build:
		$(COMPILER) -o $(BUILD_DIR)arch_app  $(SOURCES) $(OPTIONS) 

app_execute:
		./$(BUILD_DIR)arch_app &> $(LOG_DIR)$(CURRENT_LOG_NAME)
