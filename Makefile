# Projeto 1 - Processamento de Imagens em C com SDL3
# Compilado e testado com gcc (MinGW-w64, UCRT) no Windows.
# No WSL/Linux, assume SDL3 + SDL3_image + SDL3_ttf instaladas no sistema
# (via pkg-config) em vez dos pacotes devel-mingw usados no Windows.

CC := gcc
STD := -std=c17
WARN := -Wall -Wextra
SRC_DIR := src
BUILD_DIR := build

# Comandos de shell/copia diferem entre Windows (cmd.exe) e Linux/WSL (sh).
# $(OS) so existe no Windows; forcamos cmd.exe la para nao depender de
# encontrar (ou nao) um sh.exe no PATH (ex. Git Bash) em cada maquina.
ifeq ($(OS),Windows_NT)
SHELL := cmd.exe
.SHELLFLAGS := /C

SDL3_DIR := libs/SDL3-3.4.16/x86_64-w64-mingw32
SDL3_IMAGE_DIR := libs/SDL3_image-3.4.6/x86_64-w64-mingw32
SDL3_TTF_DIR := libs/SDL3_ttf-3.2.2/x86_64-w64-mingw32

INCLUDES := -I$(SDL3_DIR)/include -I$(SDL3_IMAGE_DIR)/include -I$(SDL3_TTF_DIR)/include
LIBDIRS := -L$(SDL3_DIR)/lib -L$(SDL3_IMAGE_DIR)/lib -L$(SDL3_TTF_DIR)/lib
LIBS := -lSDL3_ttf -lSDL3_image -lSDL3 -lm

SDL3_DLL := $(SDL3_DIR)/bin/SDL3.dll
SDL3_IMAGE_DLL := $(SDL3_IMAGE_DIR)/bin/SDL3_image.dll
SDL3_TTF_DLL := $(SDL3_TTF_DIR)/bin/SDL3_ttf.dll

MKDIR_P = if not exist "$1" mkdir "$1"
RM_RF = if exist "$1" rmdir /s /q "$1"
COPY = copy /Y "$(subst /,\,$1)" "$(subst /,\,$2)" >nul
RUN_CMD = cd $(BUILD_DIR) && proj1.exe
NEEDS_DLLS := 1
else
# Linux/WSL: espera SDL3/SDL3_image/SDL3_ttf instaladas no sistema, com
# arquivos .pc localizaveis por pkg-config (padrao ao instalar via
# 'sudo make install' a partir do codigo-fonte oficial, ou via pacotes
# do sistema quando disponiveis).
INCLUDES := $(shell pkg-config --cflags sdl3 SDL3_image SDL3_ttf 2>/dev/null)
LIBDIRS :=
LIBS := $(shell pkg-config --libs sdl3 SDL3_image SDL3_ttf 2>/dev/null)
ifeq ($(strip $(LIBS)),)
LIBS := -lSDL3_ttf -lSDL3_image -lSDL3
endif
LIBS += -lm

MKDIR_P = mkdir -p "$1"
RM_RF = rm -rf "$1"
COPY = cp -f "$1" "$2"
RUN_CMD = cd $(BUILD_DIR) && ./proj1.exe
NEEDS_DLLS := 0
endif

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET := $(BUILD_DIR)/proj1.exe

.PHONY: all clean run dlls assets

ifeq ($(NEEDS_DLLS),1)
all: $(TARGET) dlls assets
else
all: $(TARGET) assets
endif

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBDIRS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(STD) $(WARN) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	$(call MKDIR_P,$(BUILD_DIR))

dlls: | $(BUILD_DIR)
	$(call COPY,$(SDL3_DLL),$(BUILD_DIR)/)
	$(call COPY,$(SDL3_IMAGE_DLL),$(BUILD_DIR)/)
	$(call COPY,$(SDL3_TTF_DLL),$(BUILD_DIR)/)

assets: | $(BUILD_DIR)
	$(call MKDIR_P,$(BUILD_DIR)/assets/fonts)
	$(call COPY,assets/fonts/DejaVuSans.ttf,$(BUILD_DIR)/assets/fonts/)

run: all
	$(RUN_CMD)

clean:
	$(call RM_RF,$(BUILD_DIR))
