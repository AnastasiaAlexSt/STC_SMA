@echo off
set MAIN_PATH=%cd%
set PATH_TO_LIB_XLSX=%MAIN_PATH%\build_libs\libxlsxwriter
set PATH_TO_SRC_LIB_XLSX=%MAIN_PATH%\src_lib\libxlsxwriter
set PATH_TO_LIB_ZLIB=%MAIN_PATH%\build_libs\zlib
set PATH_TO_SRC_LIB_ZLIB=%MAIN_PATH%\src_lib\zlib
set NAME_BUILD_DIR=build

:: zlib build
mkdir "%PATH_TO_LIB_ZLIB%"
cd "%PATH_TO_LIB_ZLIB%"
mkdir "%NAME_BUILD_DIR%"
cd "%NAME_BUILD_DIR%"
cmake "%PATH_TO_SRC_LIB_ZLIB%" -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="%PATH_TO_LIB_ZLIB%" -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install
:: libxlsx build
mkdir "%PATH_TO_LIB_XLSX%"
cd "%PATH_TO_LIB_XLSX%"
mkdir "%NAME_BUILD_DIR%"
cd "%NAME_BUILD_DIR%"
cmake "%PATH_TO_SRC_LIB_XLSX%" -G "MinGW Makefiles" -DZLIB_ROOT="%PATH_TO_LIB_ZLIB%" -DCMAKE_INSTALL_PREFIX="%PATH_TO_LIB_XLSX%" -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install
