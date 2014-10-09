set                         (targets                        
                             rips
                             rips-pairwise
                             rips-weighted
                             rips-image-zigzag
                             rips-zigzag)
                             
foreach                     (t ${targets})
    add_executable          (${t} ${t}.cpp)
    target_link_libraries   (${t} ${libraries} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})
endforeach                  (t ${targets})

add_custom_target           (rips.py ALL 
                             ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/rips.py ${CMAKE_CURRENT_BINARY_DIR}/rips.py)
add_custom_target           (rips-pairwise.py ALL 
                             ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/rips-pairwise.py ${CMAKE_CURRENT_BINARY_DIR}/rips-pairwise.py)
