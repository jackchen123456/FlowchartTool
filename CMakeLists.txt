cmake_minimum_required(VERSION 3.10)

get_filename_component(CURRENT_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

get_filename_component(CURRENT_DIR_NAME "${CURRENT_DIR_PATH}" NAME)
project(${CURRENT_DIR_NAME})

set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif ()

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui Svg LinguistTools REQUIRED)

file(GLOB UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
file(GLOB RCC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*qrc")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_BINARY_DIR}/*.h")
file(GLOB CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(TS_FILES ${CMAKE_SOURCE_DIR}/mui/zh_CN/lessoncode.ts)
set(QM_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/mui/zh_CN/)

qt5_create_translation(QM_FILES ${HEADER_FILES} ${CPP_FILES} ${UI_FILES} ${TS_FILES})

add_executable(${PROJECT_NAME} WIN32
        main.cpp
        MainWindow.cpp
        MainWindow.h
        ShapeLibraryWidget.cpp
        ShapeLibraryWidget.h
        DrawArea.cpp
        DrawArea.h
        PropertyPanel.cpp
        PropertyPanel.h
        ShapeBase.cpp
        ShapeBase.h
        RectShape.cpp
        RectShape.h
        EllipseShape.cpp
        EllipseShape.h
        DiamondShape.cpp
        DiamondShape.h
        PentagonShape.cpp
        PentagonShape.h
        HexagonShape.cpp
        HexagonShape.h
        ArrowShape.cpp
        ArrowShape.h
        LineShape.cpp
        LineShape.h
        PolygonShape.cpp
        PolygonShape.h
        LineBaseShape.cpp
        LineBaseShape.h
        MyTextEdit.cpp
        MyTextEdit.h
        FLowLayout.cpp
        FlowLayout.h
        image.qrc
        ${QM_FILES}
        )

# add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${CPP_FILES} ${UI_FILES} ${RCC_FILES} ${QM_FILES})

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        )

add_custom_target(translations DEPENDS ${QM_FILES})
add_dependencies(${PROJECT_NAME} translations)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${QM_OUTPUT_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${QM_FILES} ${QM_OUTPUT_DIR}
        COMMENT "Copying .qm files to ${QM_OUTPUT_DIR}"
        )