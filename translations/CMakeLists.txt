set(TS_FILES
  sailfishos-chum-gui.ts
  sailfishos-chum-gui-pl.ts
  sailfishos-chum-gui-sv.ts
)

# Implementing custom logic as
# qt5_create_translation doesn't respect lrelease options

# Generate a list of input files
set(LST_FILE "${CMAKE_CURRENT_BINARY_DIR}/sources.txt")
file(GLOB_RECURSE SOURCES
  "${CMAKE_SOURCE_DIR}/qml/*.qml"
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
string(REPLACE ";" "\n" LST_SOURCES "${SOURCES}")
file(GENERATE
  OUTPUT   ${LST_FILE}
  CONTENT "${LST_SOURCES}"
)

foreach(TS ${TS_FILES})
  set(TS "${CMAKE_CURRENT_SOURCE_DIR}/${TS}")
  list(APPEND TS_FILES_ABS "${TS}")
  # Add a command to update ts files
  add_custom_command(
    OUTPUT  ${TS}
    COMMAND Qt5::lupdate
    ARGS    -silent "@${LST_FILE}" -ts ${TS}
    DEPENDS ${SOURCES}
  )
  # Add a command to compile qm files
  get_filename_component(QM ${TS} NAME_WE)
  set(QM ${QM}.qm)
  list(APPEND QM_FILES ${QM})
  add_custom_command(
    OUTPUT          ${QM}
    COMMAND         Qt5::lrelease
    ARGS            -idbased -silent ${TS} -qm ${QM}
    MAIN_DEPENDENCY ${TS}
  )
  # Install qm files
  install(
    FILES       "${CMAKE_CURRENT_BINARY_DIR}/${QM}"
    DESTINATION share/sailfishos-chum-gui/translations
  )
endforeach()

add_custom_target(translations
  ALL
  DEPENDS ${QM_FILES}
)

# Use in distfiles target
set(TS_FILES ${TS_FILES_ABS} PARENT_SCOPE)
