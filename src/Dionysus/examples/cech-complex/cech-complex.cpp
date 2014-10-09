#include <utilities/log.h>

#include <topology/simplex.h>
#include <topology/filtration.h>
#include <topology/static-persistence.h>
//#include <topology/dynamic-persistence.h>
#include <topology/persistence-diagram.h>
#include "Miniball_dynamic_d.h"
#include <algorithm>

#include <iostream>
#include <fstream>


typedef         std::vector<Point>                                      PointContainer;
typedef         unsigned int                                            PointIndex;
typedef         Simplex<PointIndex, double>                             Smplx;
typedef         Filtration<Smplx>                                       CechFiltration;
typedef         StaticPersistence<>                                     Persistence;
//typedef         DynamicPersistenceTrails<>                              Persistence;
typedef         PersistenceDiagram<>            PDgm;

int choose(int n, int k)
{
    if (k > n/2) k = n-k;
    int numerator = 1, denominator = 1;
    for (int i = 0; i < k; ++i)
    { numerator *= (n-i); denominator *= (i+1); }
    return numerator/denominator;
}

void add_simplices(CechFiltration& sv, int d, const PointContainer& points)
{
    PointIndex indices[d+1];
    for (int i = 0; i < d+1; ++i) 
        indices[i] = d - i;

    while(indices[d] < points.size() - d)
    {
        // Add simplex
        Miniball mb(points[indices[0]].dim());
        Smplx s;
        for (int i = 0; i < d+1; ++i)
        {
            s.add(indices[i]);
            mb.check_in(points[indices[i]]);
        }
        mb.build();
        s.data() = mb.squared_radius();
        sv.push_back(s);

        
        // Advance indices
        for (int i = 0; i < d+1; ++i)
        {
            ++indices[i];
            if (indices[i] < points.size() - i)
            {
                for (int j = i-1; j >= 0; --j)
                    indices[j] = indices[j+1] + 1;
                break;
            }
        }
    }
}

int main(int argc, char** argv) 
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stdoutLog.subscribeTo( RLOG_CHANNEL("info") );
    stdoutLog.subscribeTo( RLOG_CHANNEL("error") );
#endif

    SetFrequency(GetCounter("persistence/pair"), 10000);
    SetTrigger(GetCounter("persistence/pair"), GetCounter(""));

    // Read in the point set and compute its Delaunay triangulation
    std::istream& in = std::cin;
    int ambient_d, homology_d;
    in >> ambient_d >> homology_d;
    
    rInfo("Ambient dimension: %d", ambient_d);
    rInfo("Will compute PD up to dimension: %d", homology_d);
    
    // Read points
    PointContainer points;
    while(in)
    {
        Point p(ambient_d);
        for (int i = 0; i < ambient_d; ++i)
            in >> p[i];
        points.push_back(p);
    }
    rInfo("Points read: %d", points.size());
   
    // Compute simplices with their Cech values
    int num_simplices = 0;
    for (int i = 0; i <= homology_d + 1; ++i)
        num_simplices += choose(points.size(), i+1);
    rInfo("Reserved SimplexVector of size: %d", num_simplices);

    CechFiltration cf;
    for (int i = 0; i <= homology_d + 1; ++i)
        add_simplices(cf, i, points);
    rInfo("Size of SimplexVector: %d", cf.size());

    // Sort the filtration
    cf.sort(DataDimensionComparison<Smplx>());
    rInfo("Filtration initialized");

    // Compute persistence
    Persistence p(cf);
    rInfo("Persistence initialized");
    p.pair_simplices();
    rInfo("Simplices paired");

    Persistence::SimplexMap<CechFiltration>     m = p.make_simplex_map(cf);
    std::map<Dimension, PDgm> dgms;
    init_diagrams(dgms, p.begin(), p.end(), 
                  evaluate_through_map(m, Smplx::DataEvaluator()), 
                  evaluate_through_map(m,  Smplx::DimensionExtractor()));

    for (int i = 0; i <= homology_d; ++i)
    {
        std::cout << i << std::endl << dgms[i] << std::endl;
    }
}

