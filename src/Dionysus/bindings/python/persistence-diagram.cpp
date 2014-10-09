#define BOOST_PYTHON_STATIC_LIB
#include<topology/persistence-diagram.h>
#include<utilities/types.h>

#include "filtration.h"
#include "simplex.h"
#include "static-persistence.h"
#include "dynamic-persistence.h"

#include <boost/foreach.hpp>

#include<boost/python.hpp>
#include<boost/python/init.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/python/stl_iterator.hpp>
#include<boost/python/def.hpp>
namespace bp = boost::python;


namespace dionysus{
namespace python{

typedef     bp::object                  Data;
typedef     PersistenceDiagram<Data>    PersistenceDiagramD;
typedef     PersistenceDiagramD::Point  PointD;
typedef     boost::shared_ptr<PersistenceDiagramD>  PDgmPtr;

} } //namespace dionysus::python

namespace dp = dionysus::python;

struct PointFromTupleConverter
{
    PointFromTupleConverter()
    {
        boost::python::converter::registry::push_back(&convertible,
                                                      &construct,
                                                      boost::python::type_id<dp::PointD>());
    }

    static void* convertible(PyObject* obj_ptr)
    {
        if (!PyTuple_Check(obj_ptr)) return 0;
        if (PyTuple_Size(obj_ptr) < 2) return 0;
        return obj_ptr;
    }

    static void construct(PyObject* obj_ptr,
                          boost::python::converter::rvalue_from_python_stage1_data* data)
    {
        //const char* value = PyString_AsString(obj_ptr);
        //if (value == 0) boost::python::throw_error_already_set();

        // Grab pointer to memory into which to construct the new T
        void* storage = ( (boost::python::converter::rvalue_from_python_storage<dp::PointD>*) data)->storage.bytes;

        RealType x = bp::extract<RealType>(PyTuple_GetItem(obj_ptr, 0));
        RealType y = bp::extract<RealType>(PyTuple_GetItem(obj_ptr, 1));

        // in-place construct the new T using the character data extraced from the python object
        dp::PointD* p = new (storage) dp::PointD(x,y);

        if (PyTuple_Size(obj_ptr) > 2)
            p->data() = bp::extract<bp::object>(PyTuple_GetItem(obj_ptr, 2));

        // Stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

struct PointToTupleConverter
{
    static PyObject* convert(const dp::PointD& p)
    {
        if (p.data().ptr() == bp::object().ptr())
            return bp::incref(bp::make_tuple(p.x(), p.y()).ptr());
        else
            return bp::incref(bp::make_tuple(p.x(), p.y(), p.data()).ptr());
    }
};


void export_point( )
{
    PointFromTupleConverter();
    bp::to_python_converter<dp::PointD, PointToTupleConverter>();
}


boost::shared_ptr<dp::PersistenceDiagramD>      init_from_points_sequence(Dimension dimension, bp::object point_sequence)
{
    typedef     bp::stl_input_iterator<dp::PointD>  PointIterator;

    PointIterator beg = PointIterator(point_sequence), end = PointIterator();
    boost::shared_ptr<dp::PersistenceDiagramD> p(new dp::PersistenceDiagramD(dimension));

    for(PointIterator cur = beg; cur != end; cur++)
        (*p).push_back(*cur);
    return p;

}

RealType    bottleneck_distance_adapter(const dp::PersistenceDiagramD& dgm1, const dp::PersistenceDiagramD& dgm2)
{
    return bottleneck_distance(dgm1, dgm2);
}


template<class Persistence>
struct InitDiagrams
{
    typedef             std::map<int, dp::PDgmPtr>                                          DiagramMap;
    typedef             typename Persistence::template SimplexMap<dp::PythonFiltration>     SMap;

    struct DataEvaluator
    {
                            DataEvaluator(const SMap& smap_):
                                smap(smap_)                         {}

        template<class Key>
        RealType            operator()(Key k) const                 { return bp::extract<RealType>(smap[k].data()); }

        const SMap& smap;
    };

    struct PythonEvaluator
    {
                            PythonEvaluator(const SMap& smap_, bp::object eval_):
                                smap(smap_), eval(eval_)            {}

        template<class Key>
        RealType            operator()(Key k) const                 { return bp::extract<RealType>(eval(smap[k])); }

        const SMap& smap;
        bp::object  eval;
    };

    // A hack
    struct DiagramMapOwner: public DiagramMap
    {
        typedef             dp::PersistenceDiagramD                 mapped_type;

        mapped_type&        operator[](Dimension d)
        {
            if (this->find(d) == this->end())
                this->insert(std::make_pair(d, dp::PDgmPtr(new dp::PersistenceDiagramD(d))));
            return *DiagramMap::operator[](d);
        }
    };

    static
    bp::list    extract_list(const DiagramMapOwner& dgms)
    {
        bp::list result;
        size_t dim = 0;
        typedef         typename DiagramMapOwner::value_type         ValType;
        BOOST_FOREACH(const ValType& dim_dgm, dgms)
        {
            while (dim_dgm.first > dim)
            {
                result.append(dp::PDgmPtr(new dp::PersistenceDiagramD));
                ++dim;
            }

            // dim_dgm.first == dim
            result.append(dim_dgm.second);
            dim++;
        }

        return result;
    }

    struct PointDataVisitor
    {
                PointDataVisitor(bp::object data_): data(data_)                                 {}
        void    point(const typename Persistence::iterator& i, dp::PointD& p) const             { p.data() = data(*i); }
        bp::object data;
    };

    static
    bp::list    init(const Persistence& p, const dp::PythonFiltration& f, bp::object eval, bp::object data)
    {

        DiagramMapOwner dgms;
        SMap            smap = p.make_simplex_map(f);
        if (eval == bp::object())
            init_diagrams(dgms, p.begin(), p.end(),
                          DataEvaluator(smap),
                          evaluate_through_map(smap, dp::SimplexVD::DimensionExtractor()));
        else if (data == bp::object())
            init_diagrams(dgms, p.begin(), p.end(),
                          PythonEvaluator(smap, eval),
                          evaluate_through_map(smap, dp::SimplexVD::DimensionExtractor()));
        else
            init_diagrams(dgms, p.begin(), p.end(),
                          PythonEvaluator(smap, eval),
                          evaluate_through_map(smap, dp::SimplexVD::DimensionExtractor()),
                          PointDataVisitor(data));
        return extract_list(dgms);
    }
};

void export_persistence_diagram()
{
    bp::class_<dp::PersistenceDiagramD, dp::PDgmPtr>("PersistenceDiagram")
        .def("__init__",            bp::make_constructor(&init_from_points_sequence))
        .def(                       bp::init<Dimension>())
        .def("append",              &dp::PersistenceDiagramD::push_back)
        .add_property("dimension",  &dp::PersistenceDiagramD::dimension)
        .def(                       repr(bp::self))
        .def("__iter__",            bp::range(&dp::PersistenceDiagramD::begin, &dp::PersistenceDiagramD::end))
        .def("__len__",             &dp::PersistenceDiagramD::size)
    ;

    bp::def("init_diagrams",        &InitDiagrams<dp::SPersistence>::init,
                                     (bp::arg("persistence"),
                                      bp::arg("filtration"),
                                      bp::arg("eval")=bp::object(),
                                      bp::arg("data")=bp::object()));
    bp::def("init_diagrams",        &InitDiagrams<dp::DPersistenceChains>::init,
                                     (bp::arg("persistence"),
                                      bp::arg("filtration"),
                                      bp::arg("eval")=bp::object(),
                                      bp::arg("data")=bp::object()));

    bp::def("bottleneck_distance",  &bottleneck_distance_adapter);
    bp::def("wasserstein_distance", &wasserstein_distance<dp::PersistenceDiagramD>);
}
