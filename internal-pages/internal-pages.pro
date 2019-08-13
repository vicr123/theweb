TEMPLATE = lib
CONFIG += static

geninternalpages.target = .geninternalpages
geninternalpages.commands = npm --prefix $$PWD/theweb run build

genresources.target = $$PWD/scheme.qrc
genresources.commands = node $$PWD/genresources.js $$PWD scheme.qrc theweb/build/
genresources.depends = .geninternalpages FORCE

qtPrepareTool(RCC, rcc)

genrc.target = .genqrccpp
genrc.commands = $$RCC -name scheme $$PWD/scheme.qrc -o qrc_scheme.cpp
genrc.depends = $$PWD/scheme.qrc

QMAKE_EXTRA_TARGETS = geninternalpages genresources genrc
PRE_TARGETDEPS = .genqrccpp

RESOURCES += scheme.qrc
