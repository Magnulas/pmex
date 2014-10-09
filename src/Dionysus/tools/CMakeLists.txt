#find_package                (Qt4)

#if                          (QT4_FOUND)
#    set                     (QT_USE_QTOPENGL TRUE)
#    set                     (QT_USE_QTXML TRUE)
#    include                 (${QT_USE_FILE})

#    add_subdirectory        (diagram-viewer)
##find_library                (gle_LIBRARY                NAMES gle)
##find_library                (QGLViewer_LIBRARY          NAMES QGLViewer)
##find_path                   (QGLViewer_INCLUDE_DIR      QGLViewer/qglviewer.h)
##include_directories         (${QGLViewer_INCLUDE_DIR})
#endif                       (QT4_FOUND)


add_executable              (extract-diagram                extract-diagram.cpp)
target_link_libraries       (extract-diagram                ${libraries} ${Boost_SERIALIZATION_LIBRARY})

add_subdirectory            (matching)
