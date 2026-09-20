# Projeto 1 - Processamento de Imagens em C com SDL3
# Compilado e testado com gcc (MinGW-w64, UCRT) no Windows.

CC := gcc
STD := -std=c17
WARN := -Wall -Wextra
SRC_DIR := src
BUILD_DIR := build

SDL3_DIR := libs/SDL3-3.4.16/x86_64-w64-mingw32
SDL3_IMAGE_DIR := libs/SDL3_image-3.4.6/x86_64-w64-mingw32
SDL3_TTF_DIR := libs/SDL3_ttf-3.2.2/x86_64-w64-mingw32

INCLUDES := -I$(SDL3_DIR)/include -I$(SDL3_IMAGE_DIR)/include -I$(SDL3_TTF_DIR)/include
LIBDIRS := -L$(SDL3_DIR)/lib -L$(SDL3_IMAGE_DIR)/lib -L$(SDL3_TTF_DIR)/lib
LIBS := -lSDL3_ttf -lSDL3_image -lSDL3 -lm

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET := $(BUILD_DIR)/proj1.exe

DLLS := $(SDL3_DIR)/bin/SDL3.dll $(SDL3_IMAGE_DIR)/bin/SDL3_image.dll $(SDL3_TTF_DIR)/bin/SDL3_ttf.dll

.PHONY: all clean run dlls assets

all: $(TARGET) dlls assets

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBDIRS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(STD) $(WARN) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

dlls: | $(BUILD_DIR)
	cp -u $(DLLS) $(BUILD_DIR)/

assets: | $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/assets/fonts
	cp -u assets/fonts/DejaVuSans.ttf $(BUILD_DIR)/assets/fonts/

run: all
	cd $(BUILD_DIR) && ./proj1.exe

clean:
	rm -rf $(BUILD_DIR)
