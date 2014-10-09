#define BOOST_PYTHON_STATIC_LIB
#include <boost/iterator/indirect_iterator.hpp>

#include <boost/python.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/return_internal_reference.hpp>
namespace bp = boost::python;

#include "chain.h"
namespace dp = dionysus::python;


template<class Chain>
boost::indirect_iterator<typename Chain::const_iterator>    chain_begin(Chain& c)                  { return boost::make_indirect_iterator(c.begin()); }

template<class Chain>
boost::indirect_iterator<typename Chain::const_iterator>    chain_end(Chain& c)                    { return boost::make_indirect_iterator(c.end()); }

void export_chain()
{
    bp::class_<dp::VSPChain>("SPChain")
        .def("__iter__",    bp::range<bp::return_internal_reference<1> >(&chain_begin<dp::VSPChain>, &chain_end<dp::VSPChain>))
        .def("__len__",     &dp::VSPChain::size)
    ;
    
    bp::class_<dp::VDPChain>("DPChain")
        .def("__iter__",    bp::range<bp::return_internal_reference<1> >(&chain_begin<dp::VDPChain>, &chain_end<dp::VDPChain>))
        .def("__len__",     &dp::VDPChain::size)
    ;
}
