set							(targets						
							 ar-vineyard)
	
add_definitions				(${cgal_cxxflags})
foreach 					(t ${targets})
	add_executable			(${t} ${t}.cpp ${external_sources})
	target_link_libraries	(${t}   ${libraries} ${cgal_library} ${CGAL_3RD_PARTY_LIBRARIES} 
                                    ${Boost_SIGNALS_LIBRARY} 
                                    ${Boost_PROGRAM_OPTIONS_LIBRARY})
endforeach 					(t)
