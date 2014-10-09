#include <utilities/log.h>

#include <iostream>
#include <fstream>
#include <algorithm>

#include "grid2D.h"
#include <topology/lsvineyard.h>

int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);
    // stdoutLog.subscribeTo(RLOG_CHANNEL("topology"));
    // stdoutLog.subscribeTo(RLOG_CHANNEL("topology/persistence/transpositions"));
    // stdoutLog.subscribeTo(RLOG_CHANNEL("topology/vineyard"));
    stdoutLog.subscribeTo(RLOG_CHANNEL("lsvineyard"));
#endif     

    Grid2D g0(2, 2), g1(2, 2);
    g0(0,0) = 1; g0(0,1) = 2; g0(1,0) = 3; g0(1,1) = 0;
    g1(0,0) = 4; g1(0,1) = 2; g1(1,0) = 3; g1(1,1) = 5;
    
    // Generate the complex, initialize the vineyard (which also computes the pairing)
    typedef                     LSVineyard<Grid2D::CoordinateIndex, Grid2D>             Grid2DVineyard;
    
    Grid2DVineyard::LSFiltration        simplices;    
    g0.complex_generator(make_push_back_functor(simplices));
    Grid2DVineyard::VertexComparison    vcmp(g0);
    Grid2DVineyard::SimplexComparison   scmp(vcmp);
    simplices.sort(scmp);
    std::cout << "Complex generated, size: " << simplices.size() << std::endl;
    std::copy(simplices.begin(), simplices.end(), std::ostream_iterator<Grid2D::Smplx>(std::cout, "\n"));

    Grid2DVineyard              v(g0.begin(), g0.end(), simplices, g0);
    std::cout << "Filtration generated, size: " << v.filtration().size() << std::endl;
    std::cout << "Pairing computed" << std::endl;
 
    // Simplex order before
    std::cout << "Simplex order:" << std::endl;
    for (Grid2DVineyard::LSFiltration::Index cur = v.filtration().begin(); cur != v.filtration().end(); ++cur)
        std::cout << "  " << v.filtration().simplex(cur) << std::endl;

    // Compute vineyard
    v.compute_vineyard(g1);
    std::cout << "Vineyard computed" << std::endl;
    
    // Simplex order after
    std::cout << "Simplex order:" << std::endl;
    for (Grid2DVineyard::LSFiltration::Index cur = v.filtration().begin(); cur != v.filtration().end(); ++cur)
        std::cout << "  " << v.filtration().simplex(cur) << std::endl;

    v.vineyard().save_edges("test-vineyard");
}
