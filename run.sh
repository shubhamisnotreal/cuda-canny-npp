#!/usr/bin/env bash
set -euo pipefail

BIN="bin/canny_npp"
DATA_DIR="data"
OUTPUT_DIR="output"
LOG_FILE="${OUTPUT_DIR}/log.txt"

# Build executable if missing
if [ ! -f "$BIN" ]; then
  echo "Binary $BIN missing. Building project..."
  make build
fi

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Check if data directory contains any supported image files
IMAGE_COUNT=$(find "$DATA_DIR" -type f \( -iname "*.png" -o -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.pgm" \) 2>/dev/null | wc -l)

if [ "$IMAGE_COUNT" -gt 0 ]; then
  echo "Found $IMAGE_COUNT images in $DATA_DIR. Running batch Canny edge detection..."
  "$BIN" --batch "$DATA_DIR" --output "$OUTPUT_DIR" --low 50 --high 150 2>&1 | tee "$LOG_FILE"
  echo "Batch processing complete. Log saved to $LOG_FILE."
else
  echo "No image files found in $DATA_DIR."
  echo "Usage instructions:"
  "$BIN" --help
fi
