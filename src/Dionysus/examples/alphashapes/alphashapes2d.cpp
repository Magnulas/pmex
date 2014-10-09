#include <utilities/log.h>

#include "alphashapes2d.h"
#include <topology/filtration.h>
#include <topology/static-persistence.h>
#include <topology/persistence-diagram.h>
#include <iostream>

#include <fstream>


typedef Filtration<AlphaSimplex2D>              AlphaFiltration;
typedef StaticPersistence<>                     Persistence;
typedef PersistenceDiagram<>                    PDgm;


int main(int argc, char** argv) 
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stdoutLog.subscribeTo( RLOG_CHANNEL("error") );
    stdoutLog.subscribeTo( RLOG_CHANNEL("info") );
    //stdoutLog.subscribeTo( RLOG_CHANNEL("topology/filtration") );
    //stdoutLog.subscribeTo( RLOG_CHANNEL("topology/cycle") );
#endif

    SetFrequency(GetCounter("filtration/pair"), 10000);
    SetTrigger(GetCounter("filtration/pair"), GetCounter(""));

    // Read in the point set and compute its Delaunay triangulation
    std::istream& in = std::cin;
    double x,y;
    Delaunay2D Dt;
    while(in)
    {
        in >> x >> y;
        if (!in) break;
        Point p(x,y);
        Dt.insert(p);
    }
    rInfo("Delaunay triangulation computed");
   
    AlphaFiltration af;
    fill_complex(Dt, af);
    rInfo("Simplices: %i", af.size());

    // Create the alpha-shape filtration
    af.sort(AlphaSimplex2D::AlphaOrder());
    rInfo("Filtration initialized");

    Persistence p(af);
    rInfo("Persistence initializaed");

    p.pair_simplices();
    rInfo("Simplices paired");

    Persistence::SimplexMap<AlphaFiltration>    m       = p.make_simplex_map(af);
    std::map<Dimension, PDgm>                   dgms;
    init_diagrams(dgms, p.begin(), p.end(), 
                  evaluate_through_map(m, AlphaSimplex2D::AlphaValueEvaluator()),
                  evaluate_through_map(m, AlphaSimplex2D::DimensionExtractor()));

#if 1
    std::cout << 0 << std::endl << dgms[0] << std::endl;
    std::cout << 1 << std::endl << dgms[1] << std::endl;
#endif
}

