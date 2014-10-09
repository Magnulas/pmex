set                         (targets
                             triangle
                             triangle-zigzag)

set                         (libraries                          ${libraries} ${Boost_SERIALIZATION_LIBRARY})
                             
foreach                     (t ${targets})
    add_executable          (${t} ${t}.cpp)
    target_link_libraries   (${t} ${libraries})
endforeach                  (t ${targets})
