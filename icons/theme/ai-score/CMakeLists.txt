# hack: use qmake to build icon files via sailfish_svg2png
# do a make install so files are actually generated
# do a make uninstall so we don't pollute the build dir
add_custom_target(pngicons ALL
    COMMENT "Generating theme icons via qmake"
    COMMAND qmake -makefile -o Makefile.pngicons
    COMMAND make --makefile=Makefile.pngicons INSTALL_ROOT=${CMAKE_CURRENT_BINARY_DIR}/.icons install
    COMMAND make --makefile=Makefile.pngicons INSTALL_ROOT=${CMAKE_CURRENT_BINARY_DIR}/.icons uninstall
    COMMAND rm -f Makefile.pngicons
    BYPRODUCTS Makefile.pngicons
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ai-score.pro
)

set(THEMENAME sailfish-default)
set(THEME_PATH share/themes/${THEMENAME}/silica)
install(DIRECTORY z1.0
        DESTINATION ${THEME_PATH})
install(DIRECTORY z1.25
        DESTINATION ${THEME_PATH})
install(DIRECTORY z1.5
        DESTINATION ${THEME_PATH})
install(DIRECTORY z1.5-large
        DESTINATION ${THEME_PATH})
install(DIRECTORY z1.75
        DESTINATION ${THEME_PATH})
install(DIRECTORY z2.0
        DESTINATION ${THEME_PATH})
install(DIRECTORY z2.5
        DESTINATION ${THEME_PATH})
