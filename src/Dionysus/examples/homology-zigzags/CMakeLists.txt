set                         (targets
                             M-ZZ
                             dM-ZZ
                             iR-ZZ
                             oR-ZZ)

foreach                     (t ${targets})
    add_executable          (${t} ${t}.cpp)
    target_link_libraries   (${t} ${libraries} ${Boost_PROGRAM_OPTIONS_LIBRARY})
endforeach                  (t ${targets})
