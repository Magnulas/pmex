set                         (targets                        
                             rips-cohomology
                             rips-pairwise-cohomology
                             rips-explicit-cohomology
                             rips-weighted-cohomology
                             triangle-cohomology
                            )
                             
foreach                     (t ${targets})
    add_executable          (${t} ${t}.cpp)
    target_link_libraries   (${t} ${libraries} ${Boost_PROGRAM_OPTIONS_LIBRARY})
endforeach                  (t ${targets})
