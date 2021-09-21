cmake_minimum_required(VERSION 3.11)

project(sailfishos-chum-gui
  LANGUAGES CXX
  VERSION   ${CHUMGUI_VERSION}
)

include(FindPkgConfig)

find_package(Qt5
  COMPONENTS Quick DBus LinguistTools
  REQUIRED
)

find_package(packagekitqt5 REQUIRED)

pkg_search_module(sailfishapp
  REQUIRED
  IMPORTED_TARGET
  sailfishapp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

configure_file(QtCreatorDeployment.txt.in QtCreatorDeployment.txt)

set(CMAKE_CXX_STANDARD        17)
set(CMAKE_AUTOMOC             ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(translations)

install(DIRECTORY qml
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY mapplauncherd
  DESTINATION share
)

install(FILES ${PROJECT_NAME}.desktop
  DESTINATION share/applications
)

file(GLOB RPM_FILES rpm/*)
add_custom_target(distfiles SOURCES
  qml/sailfishos-chum-gui.qml
  qml/cover/CoverPage.qml
  qml/pages/MainPage.qml
  qml/pages/PackagePage.qml
  qml/components/ChumDetailItem.qml
  mapplauncherd/privileges.d/${PROJECT_NAME}
  LICENSE
  README.md
  ${PROJECT_NAME}.desktop
  ${RPM_FILES}
  ${TS_FILES}
)
