set(SIZES   86  108  128 172)
set(SCALES 1.0 1.25 1.49 2.0)

foreach(S Z IN ZIP_LISTS SIZES SCALES)
  set(PROF   ${S}x${S})
  set(OUTPUT ${PROF}/${PROJECT_NAME}.png)
  list(APPEND ICONS ${OUTPUT})
  add_custom_command(
    OUTPUT          ${OUTPUT}
    COMMAND         sailfish_svg2png -z ${Z} ${CMAKE_CURRENT_SOURCE_DIR} ${PROF}
    MAIN_DEPENDENCY ${PROJECT_NAME}.svg
  )
  install(
    FILES       "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}"
    DESTINATION share/icons/hicolor/${PROF}/apps
  )
endforeach()

add_custom_target(icons
  ALL
  DEPENDS ${ICONS}
)
