#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_ZIGZAG_PERSISTENCE_H__
#define __PYTHON_ZIGZAG_PERSISTENCE_H__

#include <topology/zigzag-persistence.h>
#include <topology/image-zigzag-persistence.h>
#include <boost/python.hpp>

#include "birthid.h"

namespace dionysus {
namespace python   {

typedef         ZigzagPersistence<BirthID>      ZZPersistence;
typedef         ImageZigzagPersistence<BirthID> IZZPersistence;

} } // namespace dionysus::python

#endif
