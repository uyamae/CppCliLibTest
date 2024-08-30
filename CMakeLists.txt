# CMAKE_CXX_SCAN_FOR_MODULES は3.28 以降
cmake_minimum_required(VERSION 3.28)
project(CppCliLibTest)
# C++ モジュールのサポートはC++ 20 から
set(CMAKE_CXX_STANDARD 20)
# CMake 3.28(?) 以降でvcxproj にScanSourceForModuleDependencies が出力される
# ScanSourceForModuleDependencies がtrue の場合、コンパイルオプションに/ifcOutput が追加される
# /clr と/ifcOutput を同時に指定するとコンパイルエラーになる
# CMAKE_CXX_SCAN_FOR_MODULES をOFF にしておくとエラーを回避できる
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_subdirectory(lib)
add_subdirectory(app)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cliapp)
