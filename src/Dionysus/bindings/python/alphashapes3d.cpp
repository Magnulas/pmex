#define BOOST_PYTHON_STATIC_LIB
// Wrap includes into namespaces to avoid nameclashes
#include "../../examples/alphashapes/alphashapes3d.h" 

#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
namespace bp = boost::python;

#include "utils.h"
#include "simplex.h"                // defines SimplexVD, Vertex, and Data
namespace dp = dionysus::python;


void fill_alpha3D_complex(bp::object points, bp::object complex)
{
    typedef     std::map<AlphaSimplex3D::Vertex, unsigned>      ASPointMap;

    Delaunay3D  Dt;
    ASPointMap  point_map;
    unsigned i = 0;
    for (bp::stl_input_iterator<bp::list> pt = points; pt != bp::stl_input_iterator<bp::list>(); ++pt)
    {
        double x = bp::extract<double>((*pt)[0]);
        double y = bp::extract<double>((*pt)[1]);
        double z = bp::extract<double>((*pt)[2]);
        point_map[Dt.insert(Point(x,y,z))] = i++;
    }

    AlphaSimplex3D::SimplexSet simplices;
    fill_simplex_set(Dt, simplices);

    for (AlphaSimplex3D::SimplexSet::const_iterator cur = simplices.begin(); cur != simplices.end(); ++cur)
    {
        
        dp::SimplexVD s;
        for (AlphaSimplex3D::VertexContainer::const_iterator vcur  = cur->vertices().begin(); 
                                                             vcur != cur->vertices().end(); ++vcur)
            s.add(point_map[*vcur]);
        
        s.data() = bp::object(std::make_pair(cur->value(), !cur->attached()));      // regular/critical rather than attached
        complex.attr("append")(s);
    }
}

void export_alphashapes3d()
{
    bp::def("fill_alpha3D_complex",       &fill_alpha3D_complex);
}
