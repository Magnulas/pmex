#include <utilities/types.h>
#include <topology/persistence-diagram.h>

#include <string>
#include <iostream>
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>

#include <boost/program_options.hpp>


typedef PersistenceDiagram<>                    PDgm;

namespace po = boost::program_options;


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
        std::cout << hidden << std::endl; 
        return 1; 
    }

    std::ifstream ifs1(filename1.c_str()), ifs2(filename2.c_str());
    boost::archive::binary_iarchive ia1(ifs1), ia2(ifs2);

    std::map<Dimension, PDgm>       dgms1, dgms2;

    ia1 >> dgms1;
    ia2 >> dgms2;

    std::cout << "Distance between dimension 0: " << bottleneck_distance(dgms1[0], dgms2[0]) << std::endl;
    std::cout << "Distance between dimension 1: " << bottleneck_distance(dgms1[1], dgms2[1]) << std::endl;
    std::cout << "Distance between dimension 2: " << bottleneck_distance(dgms1[2], dgms2[2]) << std::endl;
}
