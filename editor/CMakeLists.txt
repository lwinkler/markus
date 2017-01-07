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
