set							(targets
							 euclidean
                             test-ksort-linear
							 test-eventqueue)

if                          (use_synaps)
    set                     (targets                    ${targets}
                                                        polynomial
                                                        test-kinetic-sort
							                            test-linalg)
endif                       (use_synaps)

foreach 					(t ${targets})
	add_executable			(${t} ${t}.cpp)
	target_link_libraries	(${t} ${synaps_libraries} ${libraries})
endforeach 					(t ${targets})

