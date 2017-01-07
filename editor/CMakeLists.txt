add_library(editor
Editor.cpp
QWebPage2.cpp
)

add_custom_target(prepare
	COMMENT "Install Angular components for web editor"
	COMMAND npm install
)

add_custom_target(start_server
	COMMENT "Start server"
	COMMAND npm run start
)
