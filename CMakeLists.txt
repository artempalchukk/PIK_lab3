cmake_minimum_required(VERSION 3.16)
project(SurveyForm VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(SurveyForm
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/surveydata.h
)

target_link_libraries(SurveyForm PRIVATE Qt6::Widgets)

set_target_properties(SurveyForm PROPERTIES
    WIN32_EXECUTABLE TRUE
)
