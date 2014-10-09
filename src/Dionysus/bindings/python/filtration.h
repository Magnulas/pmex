#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_FILTRATION_H__
#define __PYTHON_FILTRATION_H__

#include <topology/filtration.h>
#include <boost/python.hpp>
#include "simplex.h"
#include "utils.h"                      // for ListRandomAccessIterator

namespace bp = boost::python;

namespace dionysus {
namespace python   {

typedef         Filtration<SimplexVD>                       PythonFiltration;

} } // namespace dionysus::python

#endif
