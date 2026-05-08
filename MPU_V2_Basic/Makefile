##############################################################################
# Makefile for MPU_V2_Basic — STM32F411 / GCC / LL drivers only
# Replaces IAR EWARM (.ewp) build system
#
# Usage:
#   make          — build .elf + .bin
#   make flash    — flash via ST-LINK (requires openocd or st-flash)
#   make clean    — remove build artefacts
#   make size     — show memory usage
#
# Requirements (Pop!_OS / Ubuntu):
#   sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi openocd
#   STM32CubeF4 SDK cloned/placed at $(CUBE_ROOT) below
##############################################################################

# ── Target & output ─────────────────────────────────────────────────────────
TARGET      := MPU_V2_Basic
BUILD_DIR   := build

# ── MCU ─────────────────────────────────────────────────────────────────────
# Board says STM32F411; .ewp listed F401RB — adjust if needed.
# F411CE is the most common 411 in these form factors (100 MHz capable, run at 84)
MCU_FAMILY  := STM32F4xx
MCU_DEVICE  := STM32F401xC        # Change to STM32F401xC if your chip is F401

CPU         := -mcpu=cortex-m4
FPU         := -mfpu=fpv4-sp-d16
FLOAT_ABI   := -mfloat-abi=softfp
ARCH_FLAGS  := $(CPU) $(FPU) $(FLOAT_ABI) -mthumb

# ── Paths ────────────────────────────────────────────────────────────────────
# STM32CubeF4 SDK root — clone from:
#   git clone --depth 1 https://github.com/STMicroelectronics/STM32CubeF4 ~/STM32CubeF4
CUBE_ROOT   := $(HOME)/STM32CubeF4

CMSIS_CORE  := $(CUBE_ROOT)/Drivers/CMSIS/Core/Include
CMSIS_DEV   := $(CUBE_ROOT)/Drivers/CMSIS/Device/ST/STM32F4xx/Include
LL_DRIVER   := $(CUBE_ROOT)/Drivers/STM32F4xx_HAL_Driver/Inc
STARTUP_DIR := $(CUBE_ROOT)/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc

# Project source root (where this Makefile lives)
SRC_ROOT    := $(CURDIR)

# ── Source files ─────────────────────────────────────────────────────────────
# All .c files in Src/ are auto-discovered.
# Startup file selected by MCU family.
C_SOURCES := $(SRC_ROOT)/Src/main.c \
  $(SRC_ROOT)/Src/stm32f4xx_it.c \
  $(SRC_ROOT)/Src/system_stm32f4xx.c \
  $(wildcard $(HOME)/embers_fw/LLU_V2_Common/*.c) \
  $(HOME)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c \
  $(HOME)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c \
  $(HOME)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
  $(HOME)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
  $(HOME)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c

# Startup assembly (GCC variant from CubeF4)
# F411 and F401 both use stm32f4xx startup — device distinction is in the .h
ASM_SOURCES := $(STARTUP_DIR)/startup_stm32f401xc.s
# If using F401: ASM_SOURCES := $(STARTUP_DIR)/startup_stm32f401xc.s

# Linker script — copy the template from CubeF4 and customise flash/ram sizes
# F411CE: FLASH=512K, RAM=128K | F411RE: FLASH=512K, RAM=128K
# F401RB: FLASH=128K, RAM=64K  | F401RE: FLASH=512K, RAM=96K
LDSCRIPT    := $(SRC_ROOT)/STM32F401XC_FLASH.ld
# If using F401RB: LDSCRIPT := $(SRC_ROOT)/STM32F401XC_FLASH.ld

# ── Include paths ─────────────────────────────────────────────────────────────
INC := \
  -I$(SRC_ROOT)/Inc \
  -I$(HOME)/embers_fw/LLU_V2_Common \
  -I$(HOME)/embers_fw/LLU_V2_Common/Inc \
  -I$(HOME)/STM32CubeF4/Drivers/BSP/STM32F401-Discovery \
  -I$(CMSIS_CORE) \
  -I$(CMSIS_DEV) \
  -I$(LL_DRIVER)

# ── Preprocessor defines ─────────────────────────────────────────────────────
DEFS := \
  -D$(MCU_DEVICE) \
  -DUSE_FULL_LL_DRIVER \
  -DHSE_VALUE=8000000U \
  -DHSE_STARTUP_TIMEOUT=100U \
  -DLSE_STARTUP_TIMEOUT=5000U \
  -DLSE_VALUE=32768U \
  -DEXTERNAL_CLOCK_VALUE=12288000U \
  -DHSI_VALUE=16000000U \
  -DLSI_VALUE=32000U \
  -DVDD_VALUE=3300U \
  -DPREFETCH_ENABLE=1U

# ── Toolchain ────────────────────────────────────────────────────────────────
PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
AS      := $(PREFIX)gcc -x assembler-with-cpp
CP      := $(PREFIX)objcopy
SZ      := $(PREFIX)size
HEX     := $(CP) -O ihex
BIN     := $(CP) -O binary -S

# ── Compiler flags ───────────────────────────────────────────────────────────
OPT     := -O2          # Use -Og for debug builds, -O2 for production

# C standard — IAR defaults to C99; main.c uses C99 constructs (//comments, bool)
CSTD    := -std=gnu99

CFLAGS  := $(ARCH_FLAGS) $(OPT) $(CSTD) $(INC) $(DEFS) \
           -Wall \
           -Wextra \
           -Wno-unused-parameter \
           -fdata-sections \
           -ffunction-sections \
           -fstack-usage \
           --specs=nano.specs \
           -MMD -MP          # Auto-generate dependency files

# Assembly flags
ASFLAGS := $(ARCH_FLAGS) $(INC) $(DEFS)

# ── Linker flags ─────────────────────────────────────────────────────────────
LDFLAGS := $(ARCH_FLAGS) \
           -T$(LDSCRIPT) \
           --specs=nano.specs \
           --specs=nosys.specs \
           -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
           -Wl,--gc-sections \
           -Wl,--print-memory-usage \
           -lc -lm -lnosys

# ── Object lists ─────────────────────────────────────────────────────────────
OBJECTS     := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS     += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

# Tell make where to find source files (VPATH for pattern rules)
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# ── Default target ────────────────────────────────────────────────────────────
.PHONY: all clean flash size help

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
	@echo ""
	@echo "  ✓ Build complete: $(BUILD_DIR)/$(TARGET).elf"
	@$(MAKE) --no-print-directory size

# ── Compile C ────────────────────────────────────────────────────────────────
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "  CC  $<"
	@$(CC) -c $(CFLAGS) $< -o $@

# ── Assemble startup ─────────────────────────────────────────────────────────
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "  AS  $<"
	@$(AS) -c $(ASFLAGS) $< -o $@

# ── Link ─────────────────────────────────────────────────────────────────────
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@echo "  LD  $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# ── Post-build: binary ───────────────────────────────────────────────────────
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	@echo "  BIN $@"
	@$(BIN) $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	@echo "  HEX $@"
	@$(HEX) $< $@

# ── Build directory ──────────────────────────────────────────────────────────
$(BUILD_DIR):
	@mkdir -p $@

# ── Size report ──────────────────────────────────────────────────────────────
size: $(BUILD_DIR)/$(TARGET).elf
	@echo ""
	@echo "Memory usage:"
	@$(SZ) --format=berkeley $<

# ── Flash via ST-LINK ────────────────────────────────────────────────────────
# Option A — openocd (recommended, handles F4 variants automatically)
flash: $(BUILD_DIR)/$(TARGET).elf
	openocd \
	  -f interface/stlink.cfg \
	  -f target/stm32f4x.cfg \
	  -c "program $< verify reset exit"

# Option B — st-flash (uncomment if you prefer stlink-tools over openocd)
# flash: $(BUILD_DIR)/$(TARGET).bin
# 	st-flash write $< 0x08000000

# ── Clean ────────────────────────────────────────────────────────────────────
clean:
	@rm -rf $(BUILD_DIR)
	@echo "  Cleaned $(BUILD_DIR)/"

# ── Dependency includes ──────────────────────────────────────────────────────
-include $(wildcard $(BUILD_DIR)/*.d)

# ── Help ─────────────────────────────────────────────────────────────────────
help:
	@echo ""
	@echo "  make          Build .elf and .bin"
	@echo "  make flash    Flash via openocd + ST-LINK V2"
	@echo "  make size     Show flash/RAM usage"
	@echo "  make clean    Remove build directory"
	@echo ""
	@echo "  Key variables you can override on the command line:"
	@echo "    MCU_DEVICE=STM32F401xC    (if chip is F401)"
	@echo "    OPT=-Og                   (debug-friendly optimisation)"
	@echo "    CUBE_ROOT=/path/to/cube   (CubeF4 SDK location)"
	@echo ""
