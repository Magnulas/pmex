#include <qapplication.h>
#include <QtGui>

#include "diagram.h"

#include <fstream>
#include <map>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


int main (int argc, char *argv[])
{
    std::string     diagrams_filename;
    int dimension;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("diagrams-file",  po::value<std::string>(&diagrams_filename),  "The collection of persistence diagrams")
        ("dimension",      po::value<int>(&dimension),                  "Dimension of the diagram to show");

    po::positional_options_description p;
    p.add("diagrams-file", 1);
    p.add("dimension", 2);
    
    po::options_description all; all.add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(p).run(), vm);
    po::notify(vm);

    if (!vm.count("diagrams-file") || !vm.count("dimension"))
    { 
        std::cout << "Usage: " << argv[0] << " diagrams-file dimension" << std::endl;
        std::cout << hidden << std::endl; 
        return 1; 
    }

    std::map<Dimension, PDiagram>       dgms;
    std::ifstream ifs(diagrams_filename.c_str());
    boost::archive::binary_iarchive ia(ifs);
    ia >> dgms;
    
    
    QApplication application(argc, argv);

    std::cout << dimension << std::endl;
    std::cout << dgms[dimension] << std::endl;

    DgmViewer pd(dgms[dimension]);
    pd.show();

    // Run main loop.
    return application.exec();
}
