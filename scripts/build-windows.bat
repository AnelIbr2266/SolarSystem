@echo off
chcp 65001 >nul
rem Сборка .exe на Windows.
rem 1. Установите Qt 6 (компонент MinGW 64-bit) и CMake.
rem 2. Исправьте QT_DIR ниже на свою версию Qt.
rem 3. Запустите этот файл двойным кликом.

set QT_DIR=C:\Qt\6.7.2\mingw_64

if not exist "%QT_DIR%" (
    echo Не найден Qt по пути: %QT_DIR%
    echo Откройте файл build-windows.bat и поправьте переменную QT_DIR.
    pause
    exit /b 1
)

cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%QT_DIR%
if errorlevel 1 goto error

cmake --build build
if errorlevel 1 goto error

echo.
echo Копируем библиотеки Qt рядом с exe...
"%QT_DIR%\bin\windeployqt.exe" build\SolarSystem.exe

echo.
echo Готово! Запускайте build\SolarSystem.exe
echo Для переноса на другой компьютер скопируйте всю папку build.
pause
exit /b 0

:error
echo.
echo Сборка завершилась с ошибкой.
pause
exit /b 1
