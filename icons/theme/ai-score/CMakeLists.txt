# hack: use qmake to build icon files via sailfish_svg2png
# do a make install so files are actually generated
# do a make uninstall so we don't pollute the build dir
add_custom_target(pngs ALL DEPENDS .icons
    COMMENT "Generating theme icons via qmake"
)
add_custom_command(
    OUTPUT .icons
        z1.0 z1.25 z1.5 z1.5-large z1.75 z2.0 z2.5
    COMMAND qmake -makefile -o Makefile.pngicons
    COMMAND make --makefile=Makefile.pngicons INSTALL_ROOT=.icons install
    COMMAND make --makefile=Makefile.pngicons INSTALL_ROOT=.icons uninstall
    COMMAND rm -f Makefile.pngicons
    BYPRODUCTS Makefile.pngicons
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ai-score.pro
    DEPENDS svgs/icons/*.svg
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
