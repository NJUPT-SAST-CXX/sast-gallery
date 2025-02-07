cmake_minimum_required(VERSION 3.21)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/.cmake/)
include(Version)

project(sast-gallery VERSION ${VERSION_SEMANTIC} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  set(VERSION_RC_PATH ${CMAKE_SOURCE_DIR}/sast-gallery-version.rc)
  set(WIN32_MANIFEST_PATH ${CMAKE_SOURCE_DIR}/app.manifest)
  configure_file(
      ${CMAKE_SOURCE_DIR}/.cmake/version_exe.rc.in
      ${VERSION_RC_PATH}
  )
  configure_file(
      ${CMAKE_SOURCE_DIR}/.cmake/InstallerScript.iss.in
      ${CMAKE_SOURCE_DIR}/action-cli/InstallerScript.iss
  )
endif()

if(APPLE)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})

    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${spectral_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${spectral_VERSION})

    set(ICON_NAME "res/icon/app.icns")
    set(${PROJECT_NAME}_MAC_ICON "${CMAKE_SOURCE_DIR}/${ICON_NAME}")
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
    set_property(SOURCE "${${PROJECT_NAME}_MAC_ICON}" PROPERTY
        MACOSX_PACKAGE_LOCATION Resources)
endif()

set(Qt6_DIR "/Users/lizengxin/Qt/6.8.2/macos/lib/cmake/Qt6")
find_package(Qt6 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets Concurrent)

file(GLOB_RECURSE PROJECT_SOURCES
    src/*.cpp
    src/*.h
)

qt_add_executable(sast-gallery MACOSX_BUNDLE
    ${${PROJECT_NAME}_MAC_ICON}
    ${PROJECT_SOURCES}
    res/res.qrc
    res/icon/app.rc
    ${VERSION_RC_PATH}
    ${WIN32_MANIFEST_PATH}
)

# Add ElaWidgetTools, use static library
set(ELAWIDGETTOOLS_BUILD_STATIC_LIB "STATIC" CACHE STRING "Build ElaWidgetTools as a static library")
add_subdirectory(3rdpart/ElaWidgetTools)

target_include_directories(sast-gallery PRIVATE
    src
    3rdpart/ElaWidgetTools/src/include
)

target_link_libraries(sast-gallery PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Concurrent
    ElaWidgetTools
    version::version
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(Qt6_VERSION_STRING VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.sast-gallery)
endif()
set_target_properties(sast-gallery PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS sast-gallery
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
