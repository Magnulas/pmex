#define BOOST_PYTHON_STATIC_LIB
#include <topology/zigzag-persistence.h>
#include <topology/image-zigzag-persistence.h>

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/shared_ptr.hpp>
namespace bp = boost::python;

#include "zigzag-persistence.h"             // defines ZZPersistence, IZZPersistence
#include "optional.h"
namespace dp = dionysus::python;

#include <sstream>
#include <string>


// ZigzagPersistence
bp::tuple                           zzp_add(dp::ZZPersistence& zzp, bp::object bdry, dp::BirthID birth)
{
    // Make ZColumn
    // NB: it's extremely weird that I have to do it this way,
    //     but for some reason I cannot just create boundary on the stack
    boost::shared_ptr<dp::ZZPersistence::ZColumn>
                                            boundary(new dp::ZZPersistence::ZColumn(bp::stl_input_iterator<dp::ZZPersistence::SimplexIndex>(bdry),
                                                                                    bp::stl_input_iterator<dp::ZZPersistence::SimplexIndex>()));
    boundary->sort(zzp.cmp);

    dp::ZZPersistence::SimplexIndex         i;
    dp::ZZPersistence::Death                d;
    boost::tie(i,d)                                 = zzp.add(*boundary, birth);
    return bp::make_tuple(i,d);
}

dp::ZZPersistence::Death            zzp_remove(dp::ZZPersistence& zzp, dp::ZZPersistence::SimplexIndex s, dp::ZZPersistence::BirthID birth)
{
    return zzp.remove(s, birth);
}

bool                                zzp_is_alive(dp::ZZPersistence& zzp, const dp::ZZPersistence::ZNode& zn)
{
    return zzp.is_alive(zn);
}

// ImageZigzagPersistence
bp::tuple                           izzp_add(dp::IZZPersistence& izzp, bp::object bdry, bool subcomplex, dp::BirthID birth)
{
    // Make ZColumn
    // NB: it's extremely weird that I have to do it this way,
    //     but for some reason I cannot just create boundary on the stack
    boost::shared_ptr<dp::IZZPersistence::ZColumn>
                                            boundary(new dp::IZZPersistence::ZColumn(bp::stl_input_iterator<dp::IZZPersistence::SimplexIndex>(bdry),
                                                                                     bp::stl_input_iterator<dp::IZZPersistence::SimplexIndex>()));
    boundary->sort(izzp.cmp);

    dp::IZZPersistence::SimplexIndex            i;
    dp::IZZPersistence::Death                   d;
    boost::tie(i,d)                                 = izzp.add(*boundary, subcomplex, birth);
    return bp::make_tuple(i,d);
}

dp::IZZPersistence::Death           izzp_remove(dp::IZZPersistence& izzp, dp::IZZPersistence::SimplexIndex s, dp::IZZPersistence::BirthID birth)
{
    return izzp.remove(s, birth);
}


// SimplexIndex
template<class T>
unsigned                            si_order(T& si)
{
    return si->order;
}

template<class T>
std::string                         si_repr(T& si)
{
    std::ostringstream out; out << "SimplexIndex <" << si->order << ">";
    return out.str();
}

// ZNode
template<class Persistence>
typename Persistence::ZColumn::const_iterator
znode_zcolumn_begin(typename Persistence::ZNode& zn)
{ return zn.z_column.begin(); }

template<class Persistence>
typename Persistence::ZColumn::const_iterator
znode_zcolumn_end(typename Persistence::ZNode& zn)
{ return zn.z_column.end(); }



void export_zigzag_persistence()
{
    bp::class_<dp::ZZPersistence::SimplexIndex>("ZZSimplexIndex")
        .def("order",           &si_order<dp::ZZPersistence::SimplexIndex>)
        .def("__repr__",        &si_repr<dp::ZZPersistence::SimplexIndex>)
    ;

    bp::class_<dp::IZZPersistence::SimplexIndex>("IZZSimplexIndex")
        .def("order",           &si_order<dp::IZZPersistence::SimplexIndex>)
        .def("__repr__",        &si_repr<dp::IZZPersistence::SimplexIndex>)
    ;

    bp::class_<dp::ZZPersistence>("ZigzagPersistence")
        .def("add",             &zzp_add)
        .def("remove",          &zzp_remove)
        .def("is_alive",        &zzp_is_alive)
        .def("__iter__",        bp::range(&dp::ZZPersistence::begin, &dp::ZZPersistence::end))
    ;

    bp::class_<dp::IZZPersistence>("ImageZigzagPersistence")
        .def("add",             &izzp_add)
        .def("remove",          &izzp_remove)
        .def("__iter__",        bp::range(&dp::IZZPersistence::image_begin, &dp::IZZPersistence::image_end))
    ;

    bp::class_<dp::ZZPersistence::ZNode>("ZNode", bp::no_init)
        .add_property("birth",  &dp::ZZPersistence::ZNode::birth)
        .def("__iter__",        bp::range(&znode_zcolumn_begin<dp::ZZPersistence>, &znode_zcolumn_end<dp::ZZPersistence>))
    ;

    bp::class_<dp::IZZPersistence::ZNode>("IZNode", bp::no_init)
        .add_property("birth",  &dp::IZZPersistence::ZNode::birth)
        .def("__iter__",        bp::range(&znode_zcolumn_begin<dp::IZZPersistence>, &znode_zcolumn_end<dp::IZZPersistence>))
    ;
}
