set(EASTL_SOURCES
	${CMAKE_SOURCE_DIR}/libs/eastl/allocator_eastl.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/hashtable.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/EACType.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/EAMemory.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/EASprintf.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/EASprintfCore.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/EAStdC.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/EAString.cpp
	${CMAKE_SOURCE_DIR}/libs/eastl/assert.cpp
)
add_library(eastl STATIC ${EASTL_SOURCES})
target_include_directories(eastl PUBLIC ${CMAKE_SOURCE_DIR}/libs/eastl)
find_package(Threads REQUIRED)
target_link_libraries(eastl PUBLIC Threads::Threads)
add_executable(Server
	eastl_allocators.cpp
	src/Application.cpp
	src/Core/Intrusive.cpp
	src/Core/QEventBus.cpp
	src/Network/Http.cpp
	src/Network/Socket.cpp
	src/Platform.cpp
	src/Network/TCPClient.cpp
	src/Network/TCPServer.cpp
	src/Services/WebServer/WebServer.cpp
)
target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Server PRIVATE eastl)
if(MSVC)
	target_compile_options(Server PRIVATE /W3 /wd4996)
	target_compile_options(eastl PRIVATE /W3 /wd4996)
endif()
if(WIN32)
	target_compile_definitions(Server PRIVATE UNICODE _UNICODE)
	target_link_libraries(Server PRIVATE ws2_32)
endif()
if(MINGW)
	target_link_options(Server PRIVATE -static)
endif()
add_custom_command(TARGET Server POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/index.html $<TARGET_FILE_DIR:Server>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/dashboard.html $<TARGET_FILE_DIR:Server>
)
