#include <iostream>
#include <fstream>

#include <topology/persistence-diagram.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

typedef PersistenceDiagram<>                    PDgm;
typedef PDgm::Point                             Point;


void    read_diagram(const std::string& filename, PDgm& dgm);
void    process_program_options(int argc, char* argv[], std::string& filename1, std::string& filename2);


int main(int argc, char* argv[])
{
    std::string     filename1, filename2;
    process_program_options(argc, argv, filename1, filename2);

    PDgm dgm1, dgm2;
    read_diagram(filename1, dgm1);
    read_diagram(filename2, dgm2);

    std::cout << "Distance: " << bottleneck_distance(dgm1, dgm2) << std::endl;
}


void    read_diagram(const std::string& filename, PDgm& dgm)
{
    std::ifstream in(filename.c_str());
    double birth, death;
    while(in)
    {
        in >> birth >> death;
        if (in)
            dgm.push_back(Point(birth, death));
    }
}

void    process_program_options(int     argc, char* argv[], std::string& filename1, std::string& filename2)
{
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
        std::abort();
    }
}
