#define BOOST_PYTHON_STATIC_LIB
#include <topology/dynamic-persistence.h>

#include <boost/python.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/return_internal_reference.hpp>
namespace bp = boost::python;

#include "filtration.h"
#include "dynamic-persistence.h"
#include "chain.h"
namespace dp = dionysus::python;


dp::DPersistenceChains::iterator        dpc_begin(dp::DPersistenceChains& dpc)          { return dpc.begin(); }
dp::DPersistenceChains::iterator        dpc_end(dp::DPersistenceChains& dpc)            { return dpc.end(); }

void export_dynamic_persistence_chains()
{
    bp::class_<dp::DPersistenceChainsNode>("DPCNode", bp::no_init)
        .def("pair",            &dp::pair<dp::DPersistenceChainsNode>,      bp::return_internal_reference<1>())
        .add_property("cycle",  &dp::DPersistenceChainsNode::cycle)
        .add_property("chain",  &dp::DPersistenceChainsNode::chain)
        .def("sign",            &dp::DPersistenceChainsNode::sign)
        .def("unpaired",        &dp::DPersistenceChainsNode::unpaired)
    ;

    bp::class_<dp::DPersistenceChains>("DynamicPersistenceChains", bp::no_init)
        .def("__init__",        bp::make_constructor(&dp::init_from_filtration<dp::DPersistenceChains>))
        
        .def("pair_simplices",  &dp::DPersistenceChains::pair_simplices)
        .def("__call__",        &dp::distance<dp::DPersistenceChains, dp::DPersistenceChainsIndex>)
        .def("make_simplex_map",&dp::DPersistenceChains::make_simplex_map<dp::PythonFiltration>)

        .def("__iter__",        bp::range<bp::return_internal_reference<1> >(dpc_begin, dpc_end))
        .def("__len__",         &dp::DPersistenceChains::size)
    ;

    bp::class_<dp::DPersistenceChainsSimplexMap>("DPersistenceChainsSimplexMap", bp::no_init)
        .def("__getitem__",     &dp::psmap_getitem<dp::DPersistenceChainsSimplexMap, dp::DPersistenceChainsIndex>,  bp::return_internal_reference<1>())
    ;
}
