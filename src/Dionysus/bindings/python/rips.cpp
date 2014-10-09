#define BOOST_PYTHON_STATIC_LIB
#include <topology/rips.h>
#include <boost/python.hpp>
namespace bp = boost::python;

#include "rips.h"                   // defines RipsWithDistances
namespace dp = dionysus::python;

#include <iostream>


/* Various wrappers for exposing Rips to Python */
// Constructor from distances
boost::shared_ptr<dp::RipsWithDistances>        init_from_distances(bp::object distances)
{ 
    boost::shared_ptr<dp::RipsWithDistances>    p(new dp::RipsWithDistances(distances));
    return p;
}

void export_rips()
{
    bp::class_<dp::RipsWithDistances>("Rips", bp::no_init)
        .def("__init__",            bp::make_constructor(&init_from_distances))
        .def("generate",            &dp::RipsWithDistances::generate)
        .def("generate",            &dp::RipsWithDistances::generate_candidates)
        .def("vertex_cofaces",      &dp::RipsWithDistances::vertex_cofaces)
        .def("vertex_cofaces",      &dp::RipsWithDistances::vertex_cofaces_candidate)
        .def("edge_cofaces",        &dp::RipsWithDistances::edge_cofaces)
        .def("edge_cofaces",        &dp::RipsWithDistances::edge_cofaces_candidates)

        .def("cmp",                 &dp::RipsWithDistances::cmp)
        .def("cmp",                 &dp::RipsWithDistances::cmp_native)
        .def("eval",                &dp::RipsWithDistances::eval)
        .def("eval",                &dp::RipsWithDistances::eval_native)
    ;
}
