#include <topology/simplex.h>
#include <topology/filtration.h>
#include <topology/dynamic-persistence.h>
#include <topology/persistence-diagram.h>

#include <utilities/containers.h>           // for BackInsertFunctor
#include <utilities/timer.h>

#include <vector>
#include <fstream>

#include <boost/program_options.hpp>

typedef         unsigned                                                Vertex;
typedef         Simplex<Vertex>                                         Smplx;
typedef         Filtration<Smplx>                                       Fltr;
typedef         DynamicPersistenceChains<>                              Persistence;
typedef         Persistence::Chain                                      Chain;
typedef         PersistenceDiagram<>                                    PDgm;

void            program_options(int argc, char* argv[], std::string& infilename, Dimension& skeleton, std::string& diagram_name, Dimension& cycle_dimension, std::string& cycle_filename);
void            read_filtration(const std::string& filename, Fltr& filtration);
void            output_chain(std::ostream& out, const Chain& chain, const Persistence& persistence);

int main(int argc, char* argv[])
{
    Dimension               skeleton, cycle_dimension;
    std::string             infilename, diagram_name, cycle_filename;

    program_options(argc, argv, infilename, skeleton, diagram_name, cycle_dimension, cycle_filename);
    std::ofstream           diagram_out(diagram_name.c_str());
    std::cout << "Diagram:         " << diagram_name << std::endl;
    std::ofstream           cycle_out(cycle_filename.c_str());
    std::cout << "Cycles:          " << cycle_filename << std::endl;

    Fltr                    f;
    read_filtration(infilename, f);
    std::cout << "# Read filtration of size: " << f.size() << std::endl;

    Timer persistence_timer; persistence_timer.start();
    Persistence p(f);
    p.pair_simplices();
    persistence_timer.stop();

#if 1
    // Output chains and pairs
    Persistence::SimplexMap<Fltr>   m = p.make_simplex_map(f);
    for (Persistence::iterator cur = p.begin(); cur != p.end(); ++cur)
    {
        if (!cur->sign())        // only negative simplices have non-empty cycles
        {
            Persistence::OrderIndex birth = cur->pair;      // the cycle that cur killed was born when we added birth (another simplex)

            const Smplx& b = m[birth];
            const Smplx& d = m[cur];
            
            if (b.dimension() >= skeleton) continue;
            diagram_out << b.dimension() << " " << (p.iterator_to(cur->pair) - p.begin()) << " " << (cur - p.begin()) << std::endl;
        } else if (cur->unpaired())    // positive could be unpaired
        {
            const Smplx& b = m[cur];
            
            if (b.dimension() >= skeleton) continue;
            diagram_out << b.dimension() << " " << (cur - p.begin()) << " inf" << std::endl;

            if (b.dimension() != cycle_dimension) continue;
            output_chain(cycle_out, cur->chain, p);
        }
    }
#endif
    
    persistence_timer.check("# Persistence timer");
}

void        program_options(int argc, char* argv[], std::string& infilename, Dimension& skeleton, std::string& diagram_name, Dimension& cycle_dimension, std::string& cycle_filename)
{
    namespace po = boost::program_options;

    po::options_description     hidden("Hidden options");
    hidden.add_options()
        ("input-file",          po::value<std::string>(&infilename),        "Point set whose Rips zigzag we want to compute");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("help,h",                                                                                  "produce help message")
        ("skeleton-dimsnion,s", po::value<Dimension>(&skeleton)->default_value(2),                  "Dimension of the Rips complex we want to compute")
        ("diagram,d",           po::value<std::string>(&diagram_name),                              "Filename where to output the persistence diagram")
        ("cycle-dimension,c",   po::value<Dimension>(&cycle_dimension)->default_value(1),           "Dimension of the essential cycles to output")
        ("cycle-filename,o",    po::value<std::string>(&cycle_filename),                            "Filename where to output the essential cycles");
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

void            read_filtration(const std::string& filename, Fltr& filtration)
{
    std::ifstream in(filename.c_str());
    std::string   line;
    while(std::getline(in, line))
    {
        if (line[0] == '#') continue;               // comment line in the file
        std::stringstream linestream(line);
        Vertex v;
        Smplx s;
        while (linestream >> v)
            s.add(v);
        filtration.push_back(s);            
    }
}

void    output_chain(std::ostream& out, const Chain& chain, const Persistence& persistence)
{
    out << "---\n";
    for (Chain::const_iterator iter = chain.begin(); iter != chain.end(); ++iter)
        out << persistence.iterator_to(*iter) - persistence.begin() << '\n';
}
