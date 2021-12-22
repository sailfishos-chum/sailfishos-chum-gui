cmake_minimum_required(VERSION 3.11)

project(sailfishos-chum-gui
  LANGUAGES CXX
  VERSION   ${CHUMGUI_VERSION}
)

set(REPO "sailfishos-chum" CACHE STRING "Alias of the repository")
set(GITHUB_TOKEN "unset" CACHE STRING "GitHub access token for GraphQL queries")

include(FindPkgConfig)

find_package(Qt5
  COMPONENTS Quick DBus LinguistTools
  REQUIRED
)

find_package(packagekitqt5 REQUIRED)
find_package(yaml-cpp REQUIRED)

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
  qml/pages/IssuePage.qml
  qml/pages/IssuesListPage.qml
  qml/pages/MainPage.qml
  qml/pages/PackagePage.qml
  qml/pages/PackagesListPage.qml
  qml/pages/ReleasePage.qml
  qml/pages/ReleasesListPage.qml
  qml/pages/ScreenshotPage.qml
  qml/components/ChumDetailItem.qml
  qml/components/FancyPageHeader.qml
  qml/components/ImageLabel.qml
  qml/components/MainPageButton.qml
  qml/components/MoreButton.qml
  qml/components/ScreenshotsBox.qml
  mapplauncherd/privileges.d/${PROJECT_NAME}
  LICENSE
  README.md
  ${PROJECT_NAME}.desktop
  ${RPM_FILES}
  ${TS_FILES}
)
