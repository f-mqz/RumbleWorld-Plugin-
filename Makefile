# Makefile - Pokemon Rumble World Plugin 3GX
# Compatible CTRPluginFramework

NAME := PokemonRumbleWorld_Cheats
TARGET := $(NAME)
BUILD := build
SOURCES := source
INCLUDES := include

DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM
LIBCTRU ?= $(DEVKITPRO)/libctru
CTRPLUGINFOLDER ?= /tmp/CTRPluginFramework

PATH := $(DEVKITARM)/bin:$(PATH)

CC := arm-none-eabi-gcc
CXX := arm-none-eabi-g++
LD := arm-none-eabi-gcc

ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS := -g -O2 -fomit-frame-pointer $(ARCH) -ffunction-sections -fdata-sections
CFLAGS += -DARM11 -D_3DS
CFLAGS += -I$(LIBCTRU)/include
CFLAGS += -I$(CTRPLUGINFOLDER)/Library/include
CFLAGS += -I$(CTRPLUGINFOLDER)/Library/source

CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

LDFLAGS := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(TARGET).map
LDFLAGS += -L$(CTRPLUGINFOLDER)/Library/lib
LIBS := -lCTRPluginFramework -lctrud -lm

CFILES := $(wildcard $(SOURCES)/*.c)
CPPFILES := $(wildcard $(SOURCES)/*.cpp)
OFILES := $(CFILES:$(SOURCES)/%.c=$(BUILD)/%.o) $(CPPFILES:$(SOURCES)/%.cpp=$(BUILD)/%.o)

.PHONY: all clean

all: $(TARGET).3gx

$(TARGET).3gx: $(OFILES)
	@echo "LINK $@"
	@$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

$(BUILD)/%.o: $(SOURCES)/%.c | $(BUILD)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SOURCES)/%.cpp | $(BUILD)
	@echo "CXX $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD) $(TARGET).3gx $(TARGET).map
