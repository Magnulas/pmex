#define BOOST_PYTHON_STATIC_LIB
#include <topology/cohomology-persistence.h>

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/overloads.hpp>
#include <boost/shared_ptr.hpp>
namespace bp = boost::python;

#include "cohomology-persistence.h"             // defines CohomPersistence
#include "optional.h"
namespace dp = dionysus::python;


// CohomPersistence
boost::shared_ptr<dp::CohomPersistence>     init_from_prime(unsigned p)
{
    dp::CohomPersistence::Field field(p);       // Zp

    boost::shared_ptr<dp::CohomPersistence> chp(new dp::CohomPersistence(field));
    return chp;
}

boost::shared_ptr<dp::CohomPersistence>     init()
{
    return init_from_prime(11);
}


bp::tuple                                   chp_add(dp::CohomPersistence& chp,
                                                    bp::object bdry,
                                                    dp::BirthID birth,
                                                    bool store,
                                                    bool image,
                                                    bp::object coefficients)
{
    dp::CohomPersistence::SimplexIndex      i;
    dp::CohomPersistence::Death             d;
    dp::CohomPersistence::CocyclePtr        ccl;

    if (coefficients)
    {
        boost::tie(i,d,ccl)                         = chp.add(bp::stl_input_iterator<int>(coefficients),
                                                              bp::stl_input_iterator<dp::CohomPersistence::SimplexIndex>(bdry),
                                                              bp::stl_input_iterator<dp::CohomPersistence::SimplexIndex>(),
                                                              birth, store, dp::CohomPersistence::SimplexData(), image);
    } else
    {
        boost::tie(i,d,ccl)                         = chp.add(bp::stl_input_iterator<dp::CohomPersistence::SimplexIndex>(bdry),
                                                              bp::stl_input_iterator<dp::CohomPersistence::SimplexIndex>(),
                                                              birth, store, dp::CohomPersistence::SimplexData(), image);
    }

    return bp::make_tuple(i,d, ccl);
}


dp::CohomPersistence::ZColumn::const_iterator
zcolumn_begin(dp::CohomPersistence::ZColumn& zcol)
{ return zcol.begin(); }

dp::CohomPersistence::ZColumn::const_iterator
zcolumn_end(dp::CohomPersistence::ZColumn& zcol)
{ return zcol.end(); }

dp::CohomPersistence::ZColumn::const_iterator
cocycle_zcolumn_begin(dp::CohomPersistence::Cocycle& ccl)
{ return ccl.zcolumn.begin(); }

dp::CohomPersistence::ZColumn::const_iterator
cocycle_zcolumn_end(dp::CohomPersistence::Cocycle& ccl)
{ return ccl.zcolumn.end(); }

// SimplexIndex
template<class T>
unsigned                            si_order(T& si)
{
    return si->order;
}


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(add_overloads, add, 2, 4)

void export_cohomology_persistence()
{
    bp::class_<dp::CohomPersistence::SimplexIndex>("CHSimplexIndex")
        .add_property("order",          &si_order<dp::CohomPersistence::SimplexIndex>)
    ;

    bp::class_<dp::CohomPersistence::SNode>("CHSNode", bp::no_init)
        .add_property("coefficient",    &dp::CohomPersistence::SNode::coefficient)
        .add_property("si",             &dp::CohomPersistence::SNode::si)
    ;

    bp::class_<dp::CohomPersistence>("CohomologyPersistence", bp::no_init)
        .def("__init__",        bp::make_constructor(&init))
        .def("__init__",        bp::make_constructor(&init_from_prime))
        .def("add",             &chp_add, (bp::arg("bdry"), bp::arg("birth"), bp::arg("store")=true, bp::arg("image")=true, bp::arg("coefficients")=false))

        .def("__iter__",        bp::range(&dp::CohomPersistence::begin, &dp::CohomPersistence::end))
        .def("show_cocycles",   &dp::CohomPersistence::show_cocycles)
    ;

    bp::class_<dp::CohomPersistence::Cocycle>("Cocycle", bp::no_init)
        .add_property("birth",  &dp::CohomPersistence::Cocycle::birth)
        .def("__iter__",        bp::range(&cocycle_zcolumn_begin, &cocycle_zcolumn_end))
    ;

    bp::class_<dp::CohomPersistence::ZColumn,
               boost::shared_ptr<dp::CohomPersistence::ZColumn> >("ZColumn", bp::no_init)
        .def("__iter__",        bp::range(&zcolumn_begin, &zcolumn_end))
    ;
}
