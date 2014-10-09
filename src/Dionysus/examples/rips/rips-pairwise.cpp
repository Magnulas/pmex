#include <topology/rips.h>
#include <topology/filtration.h>
#include <topology/static-persistence.h>
#include <topology/dynamic-persistence.h>
#include <topology/persistence-diagram.h>

#include <geometry/l2distance.h>
#include <geometry/distances.h>

#include <utilities/containers.h>           // for BackInsertFunctor
#include <utilities/timer.h>

#include <vector>

#include <boost/program_options.hpp>


typedef         PairwiseDistances<PointContainer, L2Distance>           PairDistances;
typedef         PairDistances::DistanceType                             DistanceType;
typedef         PairDistances::IndexType                                Vertex;

typedef         Rips<PairDistances>                                     Generator;
typedef         Generator::Simplex                                      Smplx;
typedef         Filtration<Smplx>                                       Fltr;
// typedef         StaticPersistence<>                                     Persistence;
typedef         DynamicPersistenceChains<>                              Persistence;
typedef         PersistenceDiagram<>                                    PDgm;

void            program_options(int argc, char* argv[], std::string& infilename, Dimension& skeleton, DistanceType& max_distance, std::string& diagram_name);

int main(int argc, char* argv[])
{
    Dimension               skeleton;
    DistanceType            max_distance;
    std::string             infilename, diagram_name;

    program_options(argc, argv, infilename, skeleton, max_distance, diagram_name);
    std::ofstream           diagram_out(diagram_name.c_str());
    std::cout << "Diagram:         " << diagram_name << std::endl;

    PointContainer          points;
    read_points(infilename, points);

    PairDistances           distances(points);
    Generator               rips(distances);
    Generator::Evaluator    size(distances);
    Fltr                    f;
    
    // Generate 2-skeleton of the Rips complex for epsilon = 50
    rips.generate(skeleton, max_distance, make_push_back_functor(f));
    std::cout << "# Generated complex of size: " << f.size() << std::endl;

    // Generate filtration with respect to distance and compute its persistence
    f.sort(Generator::Comparison(distances));

    Timer persistence_timer; persistence_timer.start();
    Persistence p(f);
    p.pair_simplices();
    persistence_timer.stop();

#if 1
    // Output cycles
    Persistence::SimplexMap<Fltr>   m = p.make_simplex_map(f);
    for (Persistence::iterator cur = p.begin(); cur != p.end(); ++cur)
    {
        const Persistence::Cycle& cycle = cur->cycle;

        if (!cur->sign())        // only negative simplices have non-empty cycles
        {
            Persistence::OrderIndex birth = cur->pair;      // the cycle that cur killed was born when we added birth (another simplex)

            const Smplx& b = m[birth];
            const Smplx& d = m[cur];
            
            // if (b.dimension() != 1) continue;
            // std::cout << "Pair: (" << size(b) << ", " << size(d) << ")" << std::endl;
            if (b.dimension() >= skeleton) continue;
            diagram_out << b.dimension() << " " << size(b) << " " << size(d) << std::endl;
        } else if (cur->unpaired())    // positive could be unpaired
        {
            const Smplx& b = m[cur];
            // if (b.dimension() != 1) continue;
            
            // std::cout << "Unpaired birth: " << size(b) << std::endl;
            // cycle = cur->chain;      // TODO
            if (b.dimension() >= skeleton) continue;
            diagram_out << b.dimension() << " " << size(b) << " inf" << std::endl;
        }

        // Iterate over the cycle
        // for (Persistence::Cycle::const_iterator si =  cycle.begin();
        //                                                          si != cycle.end();     ++si)
        // {
        //     const Smplx& s = m[*si];
        //     //std::cout << s.dimension() << std::endl;
        //     const Smplx::VertexContainer& vertices = s.vertices();          // std::vector<Vertex> where Vertex = Distances::IndexType
        //     AssertMsg(vertices.size() == s.dimension() + 1, "dimension of a simplex is one less than the number of its vertices");
        //     std::cout << vertices[0] << " " << vertices[1] << std::endl;
        // }
    }
#endif
    
    persistence_timer.check("# Persistence timer");
}

void        program_options(int argc, char* argv[], std::string& infilename, Dimension& skeleton, DistanceType& max_distance, std::string& diagram_name)
{
    namespace po = boost::program_options;

    po::options_description     hidden("Hidden options");
    hidden.add_options()
        ("input-file",          po::value<std::string>(&infilename),        "Point set whose Rips zigzag we want to compute");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("help,h",                                                                                  "produce help message")
        ("skeleton-dimsnion,s", po::value<Dimension>(&skeleton)->default_value(2),                  "Dimension of the Rips complex we want to compute")
        ("max-distance,m",      po::value<DistanceType>(&max_distance)->default_value(Infinity),    "Maximum value for the Rips complex construction")
        ("diagram,d",           po::value<std::string>(&diagram_name),                              "Filename where to output the persistence diagram");
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
#endif

    if (vm.count("help") || !vm.count("input-file"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] input-file" << std::endl;
        std::cout << visible << std::endl; 
        std::abort();
    }
}
