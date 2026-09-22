# SailfishOS Chum GUI uses cmake to build.
# However, I didn't feel like fishting with this, when there's a ready to use qmake template.
# You don't need to run this on build, as we store the PNGs in the source repo.

TEMPLATE  = aux
THEMENAME = sailfish-default
CONFIG   += sailfish-svg2png
# Make sure not to collide with a cmake-generated Makefile
MAKEFILE = Makefile.pngicons
