# Скользящая средняя
Для запуска проекта после скачивания нужно выполнить следующие действия:
- Перейти в папку проекта STC_SMA
- Скачать необходимые библиотеки для создания xlsx-файла командой:
```sh
-git submodule update --init --recursive
```
- Запустить скрипт:

Для Windows
```sh
build.bat
```
Для Unix
```sh
./build.sh
```
- Собрать проект:
```sh
mkdir build
cd build
```
Для Windows:
```sh
cmake .. -G "MinGW Makefiles"
cmake --build
```
Для Unix:
```sh
cmake ..
cmake --build .
```
