# CMakeList.txt: проект CMake для TransportCatalogue; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project ("TransportCatalogue")

# Добавьте источник в исполняемый файл этого проекта.
set(CMAKE_CXX_STANDARD 17)
add_executable (TransportCatalogue "main.cpp" "input_reader.cpp" "stat_reader.cpp" "transport_catalogue.cpp" "json.h" "json.cpp" "svg.h" "svg.cpp" "domain.h"  "json_reader.h" "json_reader.cpp" "request_handler.h" "request_handler.cpp" "map_renderer.h" )

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
