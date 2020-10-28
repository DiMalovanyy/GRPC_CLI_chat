set(BINARY ${CMAKE_PROJECT_NAME}_server)
set(HEADERS
    MongoDB_Manager.h
    server_operator.h
	)
set(SOURCES
	main.cpp
	)
 
 
find_package(Boost REQUIRED)
find_package(mongocxx REQUIRED)

 
add_executable(${BINARY} ${HEADERS} ${SOURCES})
set_target_properties(${BINARY} PROPERTIES
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${BINARY} ${Boost_LIBRARIES})
target_link_libraries(${BINARY} mongo::mongocxx_shared)
