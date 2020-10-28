set (BINARY ${CMAKE_PROJECT_NAME}_client)

set(HEADERS
    operator.h
    UImenu.h
	)
set(SOURCES
       main.cpp	
	)


find_package(Boost REQUIRED)
find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})


add_executable(${BINARY} ${HEADERS} ${SOURCES})
set_target_properties(${BINARY} PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${BINARY} ${CURSES_LIBRARIES})
target_link_libraries(${BINARY} ${Boost_LIBRARIES})
