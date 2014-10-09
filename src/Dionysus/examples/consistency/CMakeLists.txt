set                         (targets                        
                             rips-consistency-zigzag)
                             
foreach                     (t ${targets})
    add_executable          (${t} ${t}.cpp)
    target_link_libraries   (${t} ${libraries} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})
endforeach                  (t ${targets})
