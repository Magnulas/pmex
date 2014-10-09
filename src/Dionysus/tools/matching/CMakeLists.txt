set                             (libraries                      ${libraries} 
                                                                ${Boost_PROGRAM_OPTIONS_LIBRARY})

# Build compare-diagrams
add_executable                  (bottleneck                     bottleneck.cpp)
target_link_libraries           (bottleneck                     ${libraries})
