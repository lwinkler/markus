#set (MOC_HEADERS
#gui/markus.h
#gui/QModuleViewer.h
#)

#qt4_wrap_cpp (MOC_OUTFILES ${MOC_HEADERS})

qt4_automoc(markus.cpp QModuleViewer.cpp QParameterSlider.cpp)

add_library(gui
markus.cpp
QModuleViewer.cpp
QParameterSlider.cpp
QModuleTimer.cpp
)

