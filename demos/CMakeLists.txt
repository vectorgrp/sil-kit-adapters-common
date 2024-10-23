# Function for creating a single demo
function( build_common_demo 
    DemoFileName 
    ExecOutputName 
    #DemoOutputDirectory - optional
)
  add_executable( ${ExecOutputName} "${DemoFileName}")
  target_include_directories( ${ExecOutputName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include )
  target_link_libraries( ${ExecOutputName}
    PRIVATE
      asio
      SilKit::SilKit
      sil-kit-adapters-common
  )
  if( ${ARGC} GREATER 2 )
    set( OPT_ARGS ${ARGN} )
    list(GET OPT_ARGS 0 DemoOutputDirectory)
    set_target_properties( ${ExecOutputName}
	    PROPERTIES
	    #ensure SilKit shared libraries can be loaded
        INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN"
        BUILD_RPATH "$ORIGIN"
        RUNTIME_OUTPUT_DIRECTORY $<$<BOOL:ALWAYS>:${DemoOutputDirectory}>
    )
  endif()
endfunction()

# building all demos if compiling only this repo
if( "${CMAKE_SOURCE_DIR}/demos" STREQUAL "${CMAKE_CURRENT_LIST_DIR}" )
  file( GLOB DEMO_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" )
  foreach( DEMO_SOURCE_FULL_PATH ${DEMO_SOURCES} )
    get_filename_component( DEMO_SOURCE_FILE "${DEMO_SOURCE_FULL_PATH}" NAME )
    get_filename_component( DEMO_NAME "${DEMO_SOURCE_FILE}" NAME_WE )
    build_common_demo( ${DEMO_SOURCE_FILE} ${DEMO_NAME} )
  endforeach()
endif()