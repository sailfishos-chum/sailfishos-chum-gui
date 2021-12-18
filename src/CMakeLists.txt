add_compile_options(
        -DREPO_ALIAS="${REPO}")

add_executable(${PROJECT_NAME}
  chum.cpp
  chum.h
  chumpackage.cpp
  chumpackage.h
  chumpackagesmodel.cpp
  chumpackagesmodel.h
  projectabstract.cpp
  projectabstract.h
  projectgithub.cpp
  projectgithub.h
  main.cpp
  main.h
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

set_source_files_properties(main.cpp PROPERTIES
  COMPILE_DEFINITIONS CHUMGUI_VERSION=\"${CHUMGUI_VERSION}\"
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Quick
  Qt5::DBus
  PK::packagekitqt5
  PkgConfig::sailfishapp
  yaml-cpp
)

install(TARGETS ${PROJECT_NAME}
  RUNTIME
    DESTINATION bin
)
