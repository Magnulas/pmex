#define BOOST_PYTHON_STATIC_LIB
#include <utilities/log.h>
#include <boost/python.hpp>
#include "utils.h"

namespace bp = boost::python;
namespace dp = dionysus::python;

void export_simplex();
void export_filtration();
void export_static_persistence();
void export_dynamic_persistence_chains();
void export_chain();
void export_birthid();
void export_zigzag_persistence();
void export_cohomology_persistence();
void export_point();
void export_persistence_diagram();

void export_rips();
void export_pairwise_distances();

#ifndef NO_CGAL
void export_alphashapes2d();
void export_alphashapes3d();
#endif

#ifdef LOGGING
void            enable_log(std::string s)
{
    stdoutLog.subscribeTo(RLOG_CHANNEL(s.c_str()));
}
#endif

BOOST_PYTHON_MODULE(_dionysus)
{
    bp::to_python_converter<std::pair<double, bool>, dp::PairToTupleConverter<double, bool> >();

    export_simplex();
    export_filtration();
    export_static_persistence();
    export_dynamic_persistence_chains();
    export_chain();
    export_point();
    export_persistence_diagram();

    export_birthid();
    export_zigzag_persistence();
    export_cohomology_persistence();

    export_rips();
    export_pairwise_distances();

#ifndef NO_CGAL
    export_alphashapes2d();
    export_alphashapes3d();
#endif

#ifdef LOGGING
    bp::def("enable_log",           &enable_log);
#endif
};
