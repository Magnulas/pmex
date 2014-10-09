#include "alphashapes3d.h"
#include "../cohomology/wrappers.h"

#include <topology/cohomology-persistence.h>

#include <utilities/log.h>
#include <utilities/timer.h>

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/foreach.hpp>


typedef     boost::tuple<Dimension, RealType>                       BirthInfo;
typedef     CohomologyPersistence<BirthInfo>                        Persistence;

typedef     Persistence::SimplexIndex                               Index;
typedef     Persistence::Death                                      Death;
typedef     Persistence::CocyclePtr                                 CocyclePtr;

namespace po = boost::program_options;

int main(int argc, char** argv) 
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stdoutLog.subscribeTo( RLOG_CHANNEL("info") );
    stdoutLog.subscribeTo( RLOG_CHANNEL("error") );
    //stdoutLog.subscribeTo( RLOG_CHANNEL("topology/persistence") );
    //stdoutLog.subscribeTo( RLOG_CHANNEL("topology/chain") );
#endif

    std::string     infilename, outfilename;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file",   po::value<std::string>(&infilename),     "Point set whose alpha shape filtration and persistence we want to compute")
        ("output-file",  po::value<std::string>(&outfilename),    "Where to write the collection of persistence diagrams");

    po::positional_options_description pos;
    pos.add("input-file", 1);
    pos.add("output-file", 2);
    
    po::options_description all; all.add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(pos).run(), vm);
    po::notify(vm);

    if (!vm.count("input-file") || !vm.count("output-file"))
    { 
        std::cout << "Usage: " << argv[0] << " input-file output-file" << std::endl;
        // std::cout << hidden << std::endl; 
        return 1; 
    }

    std::ofstream   diagram_out(outfilename.c_str());

    Timer total_timer; total_timer.start();

    // Read in the point set and compute its Delaunay triangulation
    std::ifstream in(infilename.c_str());
    double x,y,z;
    Delaunay3D Dt;
    while(in)
    {
        in >> x >> y >> z;
        Point p(x,y,z);
        Dt.insert(p);
    }
    rInfo("Delaunay triangulation computed");
 
    // Set up the alpha shape filtration
    typedef     std::vector<AlphaSimplex3D>     AlphaSimplex3DVector;
    AlphaSimplex3DVector complex;
    fill_complex(Dt, complex);
    rInfo("Simplices: %d", complex.size());
    std::sort(complex.begin(), complex.end(), AlphaSimplex3D::AlphaOrder());
 
    Timer persistence_timer; persistence_timer.start();
    std::map<AlphaSimplex3D, Index, AlphaSimplex3D::VertexComparison>       complex_map;
    Persistence             p;
    boost::progress_display show_progress(complex.size());

    #ifdef COUNTERS
    Counter::CounterType    max_element_count = 0;
    #endif
    
    for(AlphaSimplex3DVector::const_iterator cur = complex.begin(); cur != complex.end(); ++cur)
    {
        const AlphaSimplex3D& s = *cur;
        std::vector<Index>      boundary;
        for (AlphaSimplex3D::BoundaryIterator bcur  = s.boundary_begin(); bcur != s.boundary_end(); ++bcur)
            boundary.push_back(complex_map[*bcur]);
        
        Index idx; Death d; CocyclePtr ccl;
        bool store = s.dimension() < 3;
        boost::tie(idx, d, ccl)     = p.add(boundary.begin(), boundary.end(), boost::make_tuple(s.dimension(), s.value()), store);
        
        // c[*cur] = idx;
        if (store)
            complex_map[s] = idx;

        if (d && (s.value() - d->get<1>()) > 0)
        {
            AssertMsg(d->get<0>() == s.dimension() - 1, "Dimensions must match");
            diagram_out << (s.dimension() - 1) << " " << d->get<1>() << " " << s.value() << std::endl;
        }
        ++show_progress;
        
        #ifdef COUNTERS
        max_element_count = std::max(max_element_count, cCohomologyElementCount->count);
        #endif
    }
    // output infinte persistence pairs 
    for (Persistence::CocycleIndex cur = p.begin(); cur != p.end(); ++cur)
        diagram_out << cur->birth.get<0>() << " " << cur->birth.get<1>() << " inf" << std::endl;
    persistence_timer.stop();
    
    total_timer.stop();
    persistence_timer.check("Persistence timer");
    total_timer.check("Total timer");

    #ifdef COUNTERS
    std::cout << "Max element count: " << max_element_count << std::endl;
    #endif
}
