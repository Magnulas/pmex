#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>
namespace bp = boost::python;

#include "distances.h"
namespace dp = dionysus::python;

boost::shared_ptr<dp::ListPointPairwiseDistances>       init_from_list(bp::list lst)
{
    boost::shared_ptr<dp::ListPointPairwiseDistances>   p(new dp::ListPointPairwiseDistances(lst));
    return p;
}

void export_pairwise_distances()
{
    bp::class_<dp::ListPointPairwiseDistances>("PairwiseDistances", bp::no_init)
        .def("__init__",        bp::make_constructor(&init_from_list))
        .def("__len__",         &dp::ListPointPairwiseDistances::size)
        .def("__call__",        &dp::ListPointPairwiseDistances::operator())
    ;
}

