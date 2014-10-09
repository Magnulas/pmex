set							(targets						
							 test-set-iterators
							 test-consistencylist
							 test-orderlist)

if                          (counters)
    set                     (targets    ${targets} test-counters)
endif                       (counters)

foreach 					(t ${targets})
	add_executable			(${t} ${t}.cpp)
	target_link_libraries	(${t} ${libraries})
endforeach 					(t ${targets})
