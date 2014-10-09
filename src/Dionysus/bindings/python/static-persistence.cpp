#define BOOST_PYTHON_STATIC_LIB
#include <topology/static-persistence.h>

#include <boost/python.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/return_internal_reference.hpp>
namespace bp = boost::python;

#include "filtration.h"
#include "static-persistence.h"
#include "chain.h"
namespace dp = dionysus::python;


void            pair_simplices(dp::SPersistence& sp, bool store_negative)
{
    dp::SPersistence::PairVisitorNoProgress visitor;
    sp.pair_simplices(sp.begin(), sp.end(), store_negative, visitor);
}


void export_static_persistence()
{
    bp::class_<dp::SPersistenceNode>("SPNode", bp::no_init)
        .def("pair",            &dp::pair<dp::SPersistenceNode>,        bp::return_internal_reference<1>())
        .add_property("cycle",  &dp::SPersistenceNode::cycle)
        .def("sign",            &dp::SPersistenceNode::sign)
        .def("unpaired",        &dp::SPersistenceNode::unpaired)
    ;

    bp::class_<dp::SPersistence>("StaticPersistence", bp::no_init)
        .def("__init__",        bp::make_constructor(&dp::init_from_filtration<dp::SPersistence>))

        .def("pair_simplices",  &pair_simplices, (bp::args("store_negative")=false))
        .def("__call__",        &dp::distance<dp::SPersistence, dp::SPersistenceIndex>)
        .def("make_simplex_map",&dp::SPersistence::make_simplex_map<dp::PythonFiltration>)

        .def("__iter__",        bp::range<bp::return_internal_reference<1> >(&dp::SPersistence::begin, &dp::SPersistence::end))
        .def("__len__",         &dp::SPersistence::size)
    ;

    bp::class_<dp::SPersistenceSimplexMap>("SPersistenceSimplexMap", bp::no_init)
        .def("__getitem__",     &dp::psmap_getitem<dp::SPersistenceSimplexMap, dp::SPersistenceIndex>,  bp::return_internal_reference<1>())
    ;
}
