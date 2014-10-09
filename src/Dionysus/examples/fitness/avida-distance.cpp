#include <iostream>
#include <vector>
#include <algorithm>
#include "avida-population-detail.h"

#include <topology/filtration.h>
#include <topology/simplex.h>
#include <topology/static-persistence.h>


typedef         Simplex<AvidaOrganismDetail::IDType, double>        Smplx;
typedef         std::vector<Smplx>                                  Complex;
typedef         Filtration<Smplx>                                   Fltr;
typedef         StaticPersistence<>                                 Persistence;

int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);
    //stdoutLog.subscribeTo(RLOG_CHANNEL("info"));
#endif

    if (argc < 2)
    {
        std::cout << "USAGE: avida FILENAME" << std::endl;
        return 0;
    }

    AvidaPopulationDetail population(argv[1]);
    const AvidaPopulationDetail::OrganismVector& organisms = population.get_organisms();

    rInfo("Number of organisms: %d", organisms.size());
    for (int i = 0; i < population.get_organisms().size(); ++i)
        rInfo("%d (%s) %f %d %d", organisms[i].id(),
                                  organisms[i].genome().c_str(),
                                  organisms[i].fitness(),
                                  organisms[i].length(),
                                  organisms[i].genome().size());

    // Distance function filtration
    Complex simplices;

    // Insert edges between all the organisms
    AvidaOrganismDetail::DistanceType avg_distance = 0;
    for (AvidaOrganismDetail::CountType i = 0; i < organisms.size(); ++i)
    {
        simplices.push_back(0);                     // all vertices have 0 value
        simplices.back().add(organisms[i].id());

        for (AvidaOrganismDetail::CountType j = i+1; j < organisms.size(); ++j)
        {
            avg_distance += organisms[i].genome_distance(organisms[j]);
            simplices.push_back(Smplx(organisms[i].genome_distance(organisms[j])));
            simplices.back().add(organisms[i].id());
            simplices.back().add(organisms[j].id());
        }
    }
    rInfo("Average distance: %f", float(avg_distance)/
                                  ((organisms.size()*organisms.size() - organisms.size())/2));

    Fltr f(simplices.begin(), simplices.end(), DataDimensionComparison<Smplx>());
    Persistence p(f);
    p.pair_simplices();

    std::cout << "Outputting histogram of death values" << std::endl;
    typedef std::vector<RealType> DeathVector;
    DeathVector deaths;
    Smplx::DataEvaluator    eval;
    Persistence::SimplexMap<Fltr>   m = p.make_simplex_map(f);
    for (Persistence::iterator i = p.begin(); i != p.end(); ++i)
    {
        if (i->unpaired()) continue;
        if (i->sign())
        {
            const Smplx& s = m[i];
            const Smplx& t = m[i->pair];
            AssertMsg(s.dimension() == 0, "Expecting only 0-dimensional diagram");
            AssertMsg(eval(s) == 0,       "Expecting only 0 birth values in 0-D diagram ");
            deaths.push_back(eval(t));
        }
    }

    // Produce histogram
    std::sort(deaths.begin(), deaths.end());
    for (DeathVector::iterator cur = deaths.begin(); cur != deaths.end(); )
    {
        DeathVector::iterator nw = std::find_if(cur, deaths.end(), 
                                                std::bind2nd(std::greater<RealType>(), *cur));
        std::cout << *cur << "\t" << (nw - cur) << std::endl;
        cur = nw;
    }
    std::cout << "Total: " << deaths.size() + 1;        // +1 for the unpaired
}
