set                         (diagram-viewerSources
                             diagram.cpp 
                             diagram-viewer-main.cpp)

set                         (diagram-viewerHeaders
                             diagram.h)

qt4_wrap_cpp                (diagram-viewerMocSources       ${diagram-viewerHeaders})

set                         (libraries                      ${libraries} 
                                                            ${Boost_SERIALIZATION_LIBRARY}
                                                            ${Boost_PROGRAM_OPTIONS_LIBRARY}
                                                            ${QT_LIBRARIES})

add_executable              (diagram-viewer                 ${diagram-viewerSources} 
                                                            ${diagram-viewerMocSources})

target_link_libraries       (diagram-viewer                 ${libraries})
