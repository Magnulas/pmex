#define BOOST_PYTHON_STATIC_LIB
#include <topology/filtration.h>

#include <boost/python.hpp>
#include <boost/iterator.hpp>
#include <boost/python/return_internal_reference.hpp>
namespace bp = boost::python;


#include "simplex.h"
#include "filtration.h"      // defines PythonFiltration
#include "utils.h"           // defines PythonCmp
namespace dp = dionysus::python;

boost::shared_ptr<dp::PythonFiltration>     init_from_iterator(bp::object iter)
{
    typedef     dp::PythonFiltration::Simplex   Smplx;
    boost::shared_ptr<dp::PythonFiltration>     p(new dp::PythonFiltration(bp::stl_input_iterator<Smplx>(iter), 
                                                                           bp::stl_input_iterator<Smplx>()));
    return p;
}

boost::shared_ptr<dp::PythonFiltration>     init_from_iterator_cmp(bp::object iter, bp::object cmp)
{
    typedef     dp::PythonFiltration::Simplex   Smplx;
    boost::shared_ptr<dp::PythonFiltration>     p(new dp::PythonFiltration(bp::stl_input_iterator<Smplx>(iter), 
                                                                           bp::stl_input_iterator<Smplx>(),
                                                                           dp::PythonCmp(cmp)));
    return p;
}

void                                        filtration_sort(dp::PythonFiltration& f, bp::object cmp)
{ f.sort(dp::PythonCmp(cmp)); }

const dp::PythonFiltration::Simplex&        f_getitem(const dp::PythonFiltration& f, int i)
{ 
    if (i >= 0)
        return f.simplex(f.begin() + i); 
    else
        return f.simplex(f.end() + i);
}

unsigned                                    f_call(const dp::PythonFiltration& f, const dp::PythonFiltration::Simplex& s)
{ return f.find(s) - f.begin(); }


void export_filtration()
{
    bp::class_<dp::PythonFiltration>("Filtration")
        .def("__init__",        bp::make_constructor(&init_from_iterator))
        .def("__init__",        bp::make_constructor(&init_from_iterator_cmp))

        .def("append",          &dp::PythonFiltration::push_back)
        .def("sort",            &filtration_sort)

        .def("__getitem__",     &f_getitem,      bp::return_internal_reference<1>())
        .def("__call__",        &f_call)
        .def("__iter__",        bp::range<bp::return_internal_reference<1> >(&dp::PythonFiltration::begin, &dp::PythonFiltration::end))
        .def("__len__",         &dp::PythonFiltration::size)
    ;
}
