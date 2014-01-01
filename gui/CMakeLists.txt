#set (MOC_HEADERS
#gui/markus.h
#gui/QModuleViewer.h
#)

#qt4_wrap_cpp (MOC_OUTFILES ${MOC_HEADERS})

#qt5_automoc(markus.cpp QModuleViewer.cpp QParameterControlBoard.cpp QParameterSlider.cpp)

add_library(gui
markus.cpp
MarkusApplication.cpp
QModuleViewer.cpp
QParameterControlBoard.cpp
QParameterSlider.cpp
Control.cpp
ParameterControl.cpp
InputStreamControl.cpp
EventControl.cpp
)

