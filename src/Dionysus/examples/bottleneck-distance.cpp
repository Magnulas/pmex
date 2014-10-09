#include <utilities/types.h>
#include <topology/persistence-diagram.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>

#include <boost/program_options.hpp>


typedef PersistenceDiagram<>                    PDgm;

namespace po = boost::program_options;


void read_diagram(PDgm& dgm, const std::string& filename)
{
    std::ifstream in(filename.c_str());
    std::string line;
    std::getline(in, line);
    while (in)
    {
        std::istringstream sin(line);
        double x,y;
        sin >> x >> y;
        dgm.push_back(PDgm::Point(x,y));
        std::getline(in, line);
    }
}

int main(int argc, char* argv[])
{
    std::string     filename1, filename2;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file1",  po::value<std::string>(&filename1), "The first collection of persistence diagrams")
        ("input-file2",  po::value<std::string>(&filename2), "The second collection of persistence diagrams");

    po::positional_options_description p;
    p.add("input-file1", 1);
    p.add("input-file2", 2);

    po::options_description all; all.add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(p).run(), vm);
    po::notify(vm);

    if (!vm.count("input-file1") || !vm.count("input-file2"))
    {
        std::cout << "Usage: " << argv[0] << " input-file1 input-file2" << std::endl;
        return 1;
    }

    PDgm dgm1, dgm2;
    read_diagram(dgm1, filename1);
    read_diagram(dgm2, filename2);
    std::cout << "Size dgm1: " << dgm1.size() << std::endl;
    std::cout << "Size dgm2: " << dgm2.size() << std::endl;

    std::cout << "Distance: " << bottleneck_distance(dgm1, dgm2) << std::endl;
}
