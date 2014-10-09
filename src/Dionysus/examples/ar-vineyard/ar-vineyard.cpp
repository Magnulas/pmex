#include <utilities/log.h>

#include "ar-vineyard.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


int main(int argc, char** argv) 
{
#ifdef LOGGING
	rlog::RLogInit(argc, argv);
	stderrLog.subscribeTo( RLOG_CHANNEL("error") );
#endif

	std::string infilename;
	double zx,zy,zz,r;
	std::string outfilename;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file",  po::value<std::string>(&infilename), "Points file")
        ("x",  po::value<double>(&zx), "x")
        ("y",  po::value<double>(&zy), "y")
        ("z",  po::value<double>(&zz), "z")
        ("r",  po::value<double>(&r), "r")
        ("output-file",  po::value<std::string>(&outfilename), "Vineyard edges output");

    std::vector<std::string> log_channels;
    po::options_description visible("Allowed options");
    visible.add_options()
        ("help,h",      "produce help message");
#if LOGGING
    visible.add_options()
        ("log,l",       po::value< std::vector<std::string> >(&log_channels),
                        "log channels to turn on");
#endif
    
    po::positional_options_description p;
    p.add("input-file", 1).add("x", 1).add("y", 1).add("z", 1).add("r", 1).add("output-file", 1);
    
    po::options_description all; all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(p).run(), vm);
    po::notify(vm);

#if LOGGING
    for (std::vector<std::string>::const_iterator cur = log_channels.begin(); cur != log_channels.end(); ++cur)
        stdoutLog.subscribeTo( RLOG_CHANNEL(cur->c_str()) );
    /* Interesting channels
     * "ar/vineyard", "ar/function-kernel/value", "geometry/simulator",
     * "topology/filtration", "topology/cycle", "topology/vineyard",
     * "topology/filtration/transpositions", "topology/lowerstar"
     */
#endif

	// Read command-line arguments
	if (vm.count("help") || !vm.count("input-file") || !vm.count("output-file") 
                         || !vm.count("x") || !vm.count("y") || !vm.count("z") || !vm.count("r"))
	{
		std::cout << "Usage: ar-vineyard [OPTIONS] POINTS X Y Z R OUTFILENAME" << std::endl;
		std::cout << "  POINTS       - filename containing points" << std::endl;
		std::cout << "  X,Y,Z        - center-point z at which to compute the vineyard" << std::endl;
		std::cout << "  R            - maximum radius" << std::endl;
		std::cout << "  OUTFILENAME  - filename for the resulting vineyard" << std::endl;
        std::cout << visible << std::endl;
		std::cout << std::endl;
		std::cout << "Computes an (alpha,r)-vineyard of the given pointset around the given point." << std::endl;
		return 1;
	}
	
	// Read in the point set and compute its Delaunay triangulation
	std::ifstream in(infilename.c_str());
	double x,y,z;
	ARVineyard::PointList points;
	while(in)
	{
		in >> x >> y >> z;
		points.push_back(Point(x,y,z));
	}
   

	// Setup vineyard and compute initial pairing
	ARVineyard arv(points, Point(zx,zy,zz));
	arv.compute_pairing();

	// Compute vineyard
	arv.compute_vineyard(r);
	std::cout << "Vineyard computed" << std::endl;
	arv.vineyard()->save_edges(outfilename);
}

