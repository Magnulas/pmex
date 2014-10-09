set							(targets						
							 test-grid2D 
							 test-grid2D-vineyard
							 combustion-vineyard
                             pl-vineyard)

if                          (use_dsrpdb)
    set                     (t                          pdbdistance-vineyard)
    add_executable			(${t} ${t}.cpp)
    target_link_libraries	(${t} ${libraries} ${dsrpdb_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
endif                       (use_dsrpdb)

foreach 					(t ${targets})
    add_executable			(${t} ${t}.cpp)
    target_link_libraries	(${t} ${libraries} ${Boost_PROGRAM_OPTIONS_LIBRARY})
endforeach 					(t ${targets})
