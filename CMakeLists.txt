cmake_minimum_required (VERSION 2.8)
project (CPlugin)

set (CMAKE_CXX_FLAGS "-Wno-write-strings")

add_subdirectory (SimpleModel)
add_subdirectory (SampleModel)
add_subdirectory (SphereModel)

##############################
# Copy python files:
file(GLOB PYTHON_FILES "*.py")
# This works only when cmake is executed, not make!!
# file(COPY ${PYTHON_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# This works for make with symbolic links :)
foreach( file_i ${PYTHON_FILES})
	# get just the filename:
	GET_FILENAME_COMPONENT(filename ${file_i} NAME)
	SET( original_file  ${file_i}  )
 	SET( link_to_original_file "${CMAKE_BINARY_DIR}/${filename}" )
 	ADD_CUSTOM_COMMAND( 
 		OUTPUT ${link_to_original_file}
        COMMAND ln -s ${original_file} ${link_to_original_file}
        #DEPENDS ${original_file} 
        COMMENT "Generating symbolic link: ${original_file} --> ${link_to_original_file}"
    )
	LIST(APPEND list_of_linked_files ${link_to_original_file} )
endforeach( file_i )

ADD_CUSTOM_TARGET(copy_files ALL DEPENDS ${list_of_linked_files})

 