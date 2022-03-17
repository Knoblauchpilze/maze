
target_sources (main-app_lib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Cell.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Opening.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Maze.cc

	${CMAKE_CURRENT_SOURCE_DIR}/Triangle.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Square.cc
	${CMAKE_CURRENT_SOURCE_DIR}/Hexagon.cc
	)

target_include_directories (main-app_lib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	)
