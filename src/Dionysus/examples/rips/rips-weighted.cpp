#include <topology/weighted-rips.h>
#include <topology/filtration.h>
#include <topology/static-persistence.h>
#include <topology/dynamic-persistence.h>
#include <topology/persistence-diagram.h>

//#define RIPS_CLOSURE_CECH_SKELETON
#ifndef RIPS_CLOSURE_CECH_SKELETON
#include <geometry/weighted-l2distance.h>
#else
#include <geometry/weighted-cechdistance.h>
#endif

#include <geometry/distances.h>

#include <utilities/containers.h>           // for BackInsertFunctor
#include <utilities/timer.h>

#include <vector>

#include <boost/program_options.hpp>


typedef         PairwiseDistances<PointContainer, WeightedL2Distance>   PairDistances;
typedef         PairDistances::DistanceType                             DistanceType;
typedef         PairDistances::IndexType                                Vertex;

typedef         WeightedRips<PairDistances>                             Generator;
typedef         Generator::Simplex                                      Smplx;
typedef         Filtration<Smplx>                                       Fltr;
typedef         StaticPersistence<>                                     Persistence;
//typedef         DynamicPersistenceChains<>                              Persistence;
typedef         PersistenceDiagram<>                                    PDgm;

void            program_options(int argc, char* argv[], std::string& infilename, Dimension& skeleton, DistanceType& max_distance);

int main(int argc, char* argv[])
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stderrLog.subscribeTo( RLOG_CHANNEL("error") );
#endif

    Dimension               skeleton;
    DistanceType            max_distance;
    std::string             infilename;

    program_options(argc, argv, infilename, skeleton, max_distance);

    PointContainer          points;
    read_weighted_points(infilename, points);

    PairDistances           distances(points);
    Generator               rips(distances);
    Generator::Evaluator    size(distances);
    Generator::Comparison   cmp (distances);
    Fltr                    complex;
    
    // Generate skeleton of the weighted Rips complex for epsilon = 50
    rips.generate(skeleton, max_distance, make_push_back_functor(complex));
    std::cout << "# Generated complex of size: " << complex.size() << std::endl;

    // Generate filtration with respect to distance and compute its persistence
    complex.sort(cmp);

    Timer persistence_timer; persistence_timer.start();
    Persistence p(complex);
    p.pair_simplices();
    persistence_timer.stop();

    // Output cycles
    Persistence::SimplexMap<Fltr>   m = p.make_simplex_map(complex);
    for (Persistence::iterator cur = p.begin(); cur != p.end(); ++cur)
    {
        const Persistence::Cycle& cycle = cur->cycle;

        if (!cur->sign())        // only negative simplices have non-empty cycles
        {
            Persistence::OrderIndex birth = cur->pair;      // the cycle that cur killed was born when we added birth (another simplex)

            const Smplx& b = m[birth];
            const Smplx& d = m[cur];
            
            if (b.dimension() >= skeleton) continue;
            std::cout << b.dimension() << " " << size(b) << " " << size(d) << std::endl;
        } else if (cur->unpaired())    // positive could be unpaired
        {
            const Smplx& b = m[cur];
            if (b.dimension() >= skeleton) continue;
            
            std::cout << b.dimension() << " " << size(b) << " inf" << std::endl;
            //cycle = cur->chain;
        }
    }
    
    persistence_timer.check("# Persistence timer");
}

void        program_options(int argc, char* argv[], std::string& infilename, Dimension& skeleton, DistanceType& max_distance)
{
    namespace po = boost::program_options;

    po::options_description     hidden("Hidden options");
    hidden.add_options()
        ("input-file",          po::value<std::string>(&infilename),        "Point set whose weighed Rips zigzag we want to compute");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("help,h",                                                                                  "produce help message")
        ("skeleton-dimsnion,s", po::value<Dimension>(&skeleton)->default_value(2),                  "Dimension of the weighted Rips complex we want to compute")
        ("max-distance,m",      po::value<DistanceType>(&max_distance)->default_value(Infinity),    "Maximum value for the weighted Rips complex construction");
#if LOGGING
    std::vector<std::string>    log_channels;
    visible.add_options()
        ("log,l",               po::value< std::vector<std::string> >(&log_channels),           "log channels to turn on (info, debug, etc)");
#endif

    po::positional_options_description pos;
    pos.add("input-file", 1);
    
    po::options_description all; all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(pos).run(), vm);
    po::notify(vm);

#if LOGGING
    for (std::vector<std::string>::const_iterator cur = log_channels.begin(); cur != log_channels.end(); ++cur)
        stderrLog.subscribeTo( RLOG_CHANNEL(cur->c_str()) );
    /**
     * Interesting channels
     * "info", "debug", "topology/persistence"
     */
#endif

    if (vm.count("help") || !vm.count("input-file"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] input-file" << std::endl;
        std::cout << visible << std::endl; 
        std::abort();
    }
}
