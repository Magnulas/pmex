find_package                (PythonLibs REQUIRED)

include_directories         (${PYTHON_LIBRARIES})
include_directories         (${PYTHON_INCLUDE_DIRS})

if                          (WIN32)
	string(TOUPPER ${PYTHON_LIBRARY} PY_LIB)
	string(REGEX REPLACE ".*PYTHON([0-9]+).*" "\\1" PY_VER ${PY_LIB})
	string(CONCAT PY_LIB "-L" ${PYTHON_LIBRARY} " -lpython" ${PY_VER})
	#message(${PY_LIB})
    link_libraries              (${Boost_PYTHON_LIBRARY} ${PY_LIB})

	#GOOD
    # link_libraries              (${Boost_PYTHON_LIBRARY} "-LC:/Python34/libs -lpython34")
	
	#BAD
	#link_libraries              (${Boost_PYTHON_LIBRARY} "-LC:/Python34/libs/python34.lib")
	
	#BAD
	#link_libraries              (${Boost_PYTHON_LIBRARY} "-LC:/Python34/libs/libpython34.a")
	
endif                          (WIN32)


link_libraries              (${Boost_PYTHON_LIBRARY})

include_directories         (${PYTHON_INCLUDE_DIR})



# currently can't build bindings with counters support, eventually FIXME
remove_definitions          (-DCOUNTERS)
set                         (sources
                                                dionysus.cpp 
                                                filtration.cpp
                                                chain.cpp
                                                static-persistence.cpp
                                                dynamic-persistence.cpp
                                                persistence-diagram.cpp
                                                simplex.cpp
                                                birthid.cpp
                                                zigzag-persistence.cpp
                                                cohomology-persistence.cpp
                                                rips.cpp
                                                distances.cpp
                            )
set                         (bindings_libraries ${libraries})

if                          (CGAL_FOUND)
    include                 (${CGAL_USE_FILE})

    set                     (sources            ${sources}
                                                alphashapes3d.cpp
                                                alphashapes2d.cpp)
    add_definitions         (${CGAL_CXX_FLAGS_INIT})
    include_directories     (${CGAL_INCLUDE_DIRS})

    link_libraries          (${CGAL_LIBRARY} ${CGAL_3RD_PARTY_LIBRARIES})
else                            (CGAL_FOUND)
    message(STATUS "CGAL not found, alphashape bindings will not be built")
    add_definitions         (-DNO_CGAL)
endif                       (CGAL_FOUND)

add_library                 (_dionysus SHARED   ${sources})
target_link_libraries       (_dionysus          ${libraries})


# Python files and the symlink
add_custom_target           (dionysus ALL
                             ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/dionysus ${CMAKE_CURRENT_BINARY_DIR}/dionysus
                             DEPENDS            dionysus/__init__.py
                                                dionysus/distances.py
                            )

get_target_property         (_dionysus_location _dionysus LOCATION)
if                          (WIN32)
	string(REPLACE "/" "\\\\" _dionysus_location ${_dionysus_location})
	string(CONCAT _dionysus_location "\"" ${_dionysus_location} "\"")
	
	add_custom_target           (dionysus-link ALL 
                             copy ${_dionysus_location} \"${CMAKE_CURRENT_BINARY_DIR}/dionysus/_dionysus.pyd\"
                             DEPENDS _dionysus)
else                          (WIN32)
	add_custom_target           (dionysus-link ALL 
                             cp ${_dionysus_location} ${CMAKE_CURRENT_BINARY_DIR}/dionysus/_dionysus.so
                             DEPENDS _dionysus)
endif                          (WIN32)

