#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_UTILS_H__
#define __PYTHON_UTILS_H__

#include <boost/python.hpp>
#include <boost/iterator/counting_iterator.hpp>
namespace bp = boost::python;

namespace dionysus {
namespace python   {

// Random access iterator into python's list (using integer indices)
template<class Value>
class ListRandomAccessIterator:
    public boost::iterator_adaptor<ListRandomAccessIterator<Value>,         // Derived
                                   boost::counting_iterator<unsigned>,      // Base
                                   Value,                                   // Value
                                   boost::use_default,
                                   Value>
{
    public:
        typedef                 ListRandomAccessIterator                                        Self;
        typedef                 boost::iterator_adaptor<ListRandomAccessIterator,           
                                                        boost::counting_iterator<unsigned>,     
                                                        Value,
                                                        boost::use_default,
                                                        Value>                                  Parent;
                    
                                ListRandomAccessIterator()                                      {}

                                ListRandomAccessIterator(bp::list l, unsigned i):
                                    Parent(i), l_(l)                                            {}

    private:
        friend class boost::iterator_core_access;
        friend class FiltrationPythonIterator;

        typename Parent::reference       
                                dereference() const                                             { return bp::object(l_[*(this->base())]); }

        bp::list                l_;
};

// Adaptor of a Pyhon object to act as a C++-style comparison functor
struct PythonCmp
{
    template<class T>
    bool            operator()(T x1, T x2) const            { return cmp_(x1, x2) < 0; }

                    PythonCmp(bp::object cmp): cmp_(cmp)    {}

    bp::object      cmp_;
};

template<class T1, class T2>
struct PairToTupleConverter 
{
    static PyObject* convert(const std::pair<T1, T2>& pair) { return bp::incref(bp::make_tuple(pair.first, pair.second).ptr()); }
};

} } // namespace dionysus::python

#endif
