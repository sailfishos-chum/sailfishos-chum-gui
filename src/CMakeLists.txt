add_compile_options(
        -DREPO_ALIAS="${REPO}")

add_executable(${PROJECT_NAME}
  chum.cpp
  chum.h
  chumabstractpackagemodel.cpp
  chumabstractpackagemodel.h
  chumavailablepackagesmodel.cpp
  chumavailablepackagesmodel.h
  chumpackage.cpp
  chumpackage.h
  chumupdatesmodel.cpp
  chumupdatesmodel.h
  main.cpp
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
