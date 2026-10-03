#!/bin/bash

set -e

TOOLS_DIR="./tools"
mkdir -p "$TOOLS_DIR"

cd "$TOOLS_DIR"

USVG_URL="https://github.com/linebender/resvg/releases/download/v0.47.0/usvg-linux-x86_64.tar.gz"
RESVG_URL="https://github.com/linebender/resvg/releases/download/v0.47.0/resvg-linux-x86_64.tar.gz"


echo "Downloading usvg..."
curl -L "$USVG_URL" -o usvg.tar.gz

echo "Downloading resvg..."
curl -L "$RESVG_URL" -o resvg.tar.gz

echo "Extracting usvg..."
mkdir -p usvg_tmp
tar -xzf usvg.tar.gz -C usvg_tmp

echo "Extracting resvg..."
mkdir -p resvg_tmp
tar -xzf resvg.tar.gz -C resvg_tmp

echo "Installing binaries..."

find usvg_tmp -type f -name "usvg" -exec cp {} ./usvg \;
find resvg_tmp -type f -name "resvg" -exec cp {} ./resvg \;

chmod +x usvg resvg

rm -rf usvg_tmp resvg_tmp usvg.tar.gz resvg.tar.gz

cd ..
