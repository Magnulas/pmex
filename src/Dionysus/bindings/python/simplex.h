#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_SIMPLEX_H__
#define __PYTHON_SIMPLEX_H__

#include <topology/simplex.h>

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
namespace bp = boost::python;


namespace dionysus {
namespace python   {

/**
 * SimplexVD is a base class for Python simplices (it's exposed to python as Simplex)
 *
 * SimplexObject is the representation of Python simplices in C++; i.e. it wraps bp::object and exposes a simplex-like interface.
 */
typedef                             int                                         Vertex;
typedef                             bp::object                                  Data;
typedef                             Simplex<Vertex, Data>                       SimplexVD;


// Wrapper around bp::object that acts like a simplex
class SimplexObject: public bp::object
{
    public:
        typedef                 SimplexObject                                   Self;
        typedef                 bp::object                                      Parent;
        typedef                 bp::stl_input_iterator<Self>                    BoundaryIterator;


                                SimplexObject(Parent o = Parent()): Parent(o)   {}

        BoundaryIterator        boundary_begin() const                          { return bp::stl_input_iterator<Self>(this->attr("boundary")); }
        BoundaryIterator        boundary_end() const                            { return bp::stl_input_iterator<Self>(); }

                                operator SimplexVD() const                      { return bp::extract<const SimplexVD&>(*this); }
                                operator bp::object() const                     { return *this; }

        bp::object              getattribute(const char* name) const            { return this->attr(name); }

        class                   VertexComparison: public SimplexVD::VertexComparison
        {
            public:
                typedef         Self                                            first_argument_type;
                typedef         Self                                            second_argument_type;
                typedef         bool                                            result_type;

                bool            operator()(const SimplexObject& s1, const SimplexObject& s2) const  
                { return SimplexVD::VertexComparison::operator()(bp::extract<const SimplexVD&>(s1), bp::extract<const SimplexVD&>(s2)); }
        };
};

struct SimplexObjectToSimplexVD
{
    static PyObject* convert (const SimplexObject& so)
    {
        return (PyObject*) &so;
    }
};

} } // namespace dionysus::python

#endif
