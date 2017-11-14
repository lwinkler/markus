# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(editor
Editor.cpp
QWebPage2.cpp
)
 
add_custom_target(prepare
	COMMENT "Install Angular components for web editor"
	COMMAND npm install
)

# Only used for development purpose
add_custom_target(start_server
	COMMENT "Start server"
	COMMAND npm run start
)

add_custom_target(tslint
	COMMENT "Typescript lint"
	COMMAND npm run lint
)
