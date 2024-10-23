# Creating pseudo-static library
file( GLOB COMMON_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*/*.cpp" )
add_library( sil-kit-adapters-common OBJECT
  ${COMMON_SOURCES} )
target_include_directories( sil-kit-adapters-common PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/../include )

if( MINGW )
	target_compile_definitions( sil-kit-adapters-common INTERFACE _WIN32_WINNT=0x0601 )
	target_link_libraries( sil-kit-adapters-common PUBLIC -lwsock32 -lws2_32 ) #windows socket/ wsa
endif()

target_link_libraries( sil-kit-adapters-common
  PRIVATE
    asio
    SilKit::SilKit
)