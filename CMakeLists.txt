cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(BikeLightsFirmware)

target_sources(app PRIVATE src/main.c src/ble_log.c)
target_include_directories(app PRIVATE src)
