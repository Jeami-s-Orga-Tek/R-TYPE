#!/usr/bin/env bash
set -e

DIRS=("Client" "Server" "libengine")

for DIR in "${DIRS[@]}"; do
    if [ -d "$DIR" ]; then
        find "$DIR" -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec clang-format -i {} +
    fi
done

echo "✅ Clang format check passed."
