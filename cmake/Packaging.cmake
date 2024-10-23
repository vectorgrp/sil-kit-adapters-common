function( declare_package package_name package_description )
    set( CPACK_GENERATOR           "ZIP" )

    set( CPACK_PACKAGE_DESCRIPTION ${package_description} )
    set( CPACK_PACKAGE_NAME        ${package_name} )
    set( CPACK_PACKAGE_VENDOR      "Vector Informatik" )
    set( CPACK_PACKAGE_CONTACT     "support@vector.com" )
    set( CPACK_PACKAGE_FILE_NAME   "${CPACK_PACKAGE_NAME}${PACKAGE_FILENAME_SUFFIX}" )

    set( CPACK_ARCHIVE_COMPONENT_INSTALL            ON )
    set( CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE        ON )
    set( CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON )

    include( CPack )
endfunction()
