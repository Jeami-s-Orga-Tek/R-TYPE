#!/usr/bin/env bash
set -e

FILES=$(find src include -regex '.*\.\(cpp\|hpp\|h\)$')
clang-format --dry-run --Werror $FILES
