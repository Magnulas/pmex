#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_BIRTHID_H__
#define __PYTHON_BIRTHID_H__

#include <boost/python.hpp>

namespace dionysus {
namespace python   {

//typedef         int                             BirthID;
//typedef         boost::python::long_            BirthID;
typedef         boost::python::object           BirthID;

} } // namespace dionysus::python

#endif // __PYTHON_BIRTHID_H__
