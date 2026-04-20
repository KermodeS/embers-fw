#!/bin/bash
# setup_toolchain.sh — one-time setup for MPU_V2_Basic GCC build
# Run once before first make:  bash setup_toolchain.sh
set -e

echo "=== MPU_V2_Basic GCC Toolchain Setup ==="
echo ""

# 1. Install system packages
echo "[1/3] Installing arm-none-eabi toolchain and openocd..."
sudo apt-get update -qq
sudo apt-get install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    openocd \
    git

# Verify
arm-none-eabi-gcc --version
echo ""

# 2. Clone STM32CubeF4 (shallow — saves ~1.5 GB)
CUBE_DIR="$HOME/STM32CubeF4"
if [ -d "$CUBE_DIR" ]; then
    echo "[2/3] STM32CubeF4 already at $CUBE_DIR — skipping clone"
else
    echo "[2/3] Cloning STM32CubeF4 SDK (shallow, ~150 MB)..."
    git clone --depth 1 \
        https://github.com/STMicroelectronics/STM32CubeF4.git \
        "$CUBE_DIR"
fi
echo ""

# 3. Copy linker script and Makefile into project
PROJECT_DIR="$HOME/embers_fw/MPU_V2_Basic"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "[3/3] Copying build files into $PROJECT_DIR ..."
mkdir -p "$PROJECT_DIR"
cp "$SCRIPT_DIR/Makefile"              "$PROJECT_DIR/"
cp "$SCRIPT_DIR/STM32F411XE_FLASH.ld" "$PROJECT_DIR/"

echo ""
echo "=== Setup complete ==="
echo ""
echo "Next steps:"
echo "  cd $PROJECT_DIR"
echo "  make"
echo "  make flash     # with ST-LINK V2 connected"
echo ""
echo "If your chip is STM32F401 (not F411), edit Makefile:"
echo "  MCU_DEVICE  := STM32F401xC"
echo "  ASM_SOURCES := ... startup_stm32f401xc.s"
echo "  LDSCRIPT    := STM32F401XC_FLASH.ld"
echo "  (and create matching STM32F401XC_FLASH.ld with 128K/64K sizes)"
