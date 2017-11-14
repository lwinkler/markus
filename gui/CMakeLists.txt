#set (MOC_HEADERS
#gui/markus.h
#gui/QModuleViewer.h
#)

#qt4_wrap_cpp (MOC_OUTFILES ${MOC_HEADERS})

#qt5_automoc(markus.cpp QModuleViewer.cpp QParameterControlBoard.cpp QParameterSlider.cpp)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(gui
Markus.cpp
MarkusApplication.cpp
QModuleViewer.cpp
QControlBoard.cpp
QParameterSlider.cpp
)

