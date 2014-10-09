#include "topology/simplex.h"
#include "topology/filtration.h"
#include "topology/static-persistence.h"
#include "topology/persistence-diagram.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>

typedef         Simplex<unsigned, unsigned>     Smplx;
typedef         Filtration<Smplx>               Fltr;
typedef         StaticPersistence<>             Persistence;
typedef         PersistenceDiagram<>            PDgm;

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    std::string infilename;

    // Parse program options
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file",  po::value<std::string>(&infilename),
                        "Filtration filename");

    po::options_description visible("Allowed options");
    visible.add_options()
        ("help,h",      "produce help message");
    po::positional_options_description p;
    p.add("input-file", 1);
    
    po::options_description all; all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("input-file"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] FILENAME" << std::endl;
        std::cout << visible << std::endl; 
        return 1; 
    }


    Fltr f;
    std::ifstream in(infilename.c_str());
    unsigned int i = 0;
    std::string s;
    std::getline(in, s);
    while(in)
    {
        std::istringstream linestream(s);
        Smplx simplex(i++);
        unsigned int vertex;
        linestream >> vertex;
        while(linestream)
        {
            simplex.add(vertex);
            linestream >> vertex;
        }
        std::cout << simplex << std::endl;
        f.push_back(simplex);
        std::getline(in, s);
    }
    
    f.sort(Smplx::DataComparison());
    Persistence pers(f);
    pers.pair_simplices();

    Persistence::SimplexMap<Fltr>   m = pers.make_simplex_map(f);
    std::map<Dimension, PDgm> dgms;
    init_diagrams(dgms, pers.begin(), pers.end(), 
                  evaluate_through_map(m, Smplx::DataEvaluator()), 
                  evaluate_through_map(m, Smplx::DimensionExtractor()));

    std::cout << 0 << std::endl << dgms[0] << std::endl;
    std::cout << 1 << std::endl << dgms[1] << std::endl;
    std::cout << 2 << std::endl << dgms[2] << std::endl;
    std::cout << 3 << std::endl << dgms[3] << std::endl;
}

