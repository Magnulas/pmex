#define BOOST_PYTHON_STATIC_LIB
// Wrap includes into namespaces to avoid nameclashes
#include "../../examples/alphashapes/alphashapes2d.h" 

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
namespace bp = boost::python;

#include "simplex.h"                // defines SimplexVD, Vertex, and Data
namespace dp = dionysus::python;


void fill_alpha2D_complex(bp::object points, bp::object complex)
{
    typedef     std::map<AlphaSimplex2D::Vertex, unsigned>      ASPointMap;

    Delaunay2D  Dt;
    ASPointMap  point_map;
    unsigned i = 0;
    for (bp::stl_input_iterator<bp::list> pt = points; pt != bp::stl_input_iterator<bp::list>(); ++pt)
    {
        double x = bp::extract<double>((*pt)[0]);
        double y = bp::extract<double>((*pt)[1]);
        point_map[Dt.insert(Point(x,y))] = i++;
    }

    AlphaSimplex2D::SimplexSet simplices;
    fill_simplex_set(Dt, simplices);

    for (AlphaSimplex2D::SimplexSet::const_iterator cur = simplices.begin(); cur != simplices.end(); ++cur)
    {
        dp::SimplexVD s;
        for (AlphaSimplex2D::VertexContainer::const_iterator vcur  = cur->vertices().begin(); 
                                                             vcur != cur->vertices().end(); ++vcur)
            s.add(point_map[*vcur]);
        
        s.data() = bp::object(std::make_pair(cur->value(), !cur->attached()));      // regular/critical rather than attached
        complex.attr("append")(s);
    }
}

void export_alphashapes2d()
{
    bp::def("fill_alpha2D_complex",       &fill_alpha2D_complex);
}
