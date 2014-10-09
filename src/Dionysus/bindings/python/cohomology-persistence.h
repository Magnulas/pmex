#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_ZIGZAG_PERSISTENCE_H__
#define __PYTHON_ZIGZAG_PERSISTENCE_H__

#include <topology/cohomology-persistence.h>
#include <boost/python.hpp>

#include "birthid.h"

namespace dionysus {
namespace python   {

typedef         CohomologyPersistence<BirthID>  CohomPersistence;

} } // namespace dionysus::python

#endif
