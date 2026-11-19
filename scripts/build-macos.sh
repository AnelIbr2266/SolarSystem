#!/bin/bash
# Сборка приложения на macOS.
#
# Qt можно установить одним из способов:
#   1) brew install qt ninja
#   2) aqtinstall (без Homebrew):
#      python3 -m venv .aqt && .aqt/bin/pip install aqtinstall
#      .aqt/bin/aqt install-qt mac desktop 6.8.3 clang_64 -O "$HOME/Qt"
#
# Скрипт сам находит Qt, но путь можно задать вручную:
#   QT_PREFIX=~/Qt/6.8.3/macos ./scripts/build-macos.sh

set -e

if [ -z "$QT_PREFIX" ]; then
    if [ -d "$HOME/Qt/6.8.3/macos" ]; then
        QT_PREFIX="$HOME/Qt/6.8.3/macos"
    else
        # Берём самую новую версию из ~/Qt/*/macos
        QT_PREFIX="$(ls -d "$HOME"/Qt/*/macos 2>/dev/null | sort -V | tail -1)"
    fi
fi

if [ -z "$QT_PREFIX" ]; then
    QT_PREFIX="$(brew --prefix qt)"
fi

echo "Qt найден в: $QT_PREFIX"

GENERATOR="Ninja"
if ! command -v ninja >/dev/null 2>&1; then
    GENERATOR="Unix Makefiles"
fi

cmake -S . -B build -G "$GENERATOR" -DCMAKE_PREFIX_PATH="$QT_PREFIX"
cmake --build build

echo ""
echo "Готово!"
echo "Запуск: open build/SolarSystem.app"
echo "Или:    ./build/SolarSystem.app/Contents/MacOS/SolarSystem"
