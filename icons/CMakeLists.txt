macro(add_icon SIZE SCALE)
  set(PROF   ${SIZE}x${SIZE})
  set(OUTPUT ${PROF}/${PROJECT_NAME}.png)
  list(APPEND ICONS ${OUTPUT})
  add_custom_command(
    OUTPUT          ${OUTPUT}
    COMMAND         sailfish_svg2png -z ${SCALE} ${CMAKE_CURRENT_SOURCE_DIR} ${PROF}
    MAIN_DEPENDENCY ${PROJECT_NAME}.svg
  )
  install(
    FILES       "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}"
    DESTINATION share/icons/hicolor/${PROF}/apps
  )
endmacro()

add_icon( 86 1.0)
add_icon(108 1.25)
add_icon(128 1.49)
add_icon(172 2.0)

add_custom_target(icons
  ALL
  DEPENDS ${ICONS}
)
