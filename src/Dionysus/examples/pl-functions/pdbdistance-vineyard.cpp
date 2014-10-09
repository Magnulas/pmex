//#include <boost/archive/binary_oarchive.hpp>
#include "utilities/log.h"

#include "pdbdistance.h"
#include <topology/lsvineyard.h>

#include <fstream>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>

void        program_options(int argc, char* argv[], std::string& input_fn_list,
                                                    std::string& output_prefix, 
                                                    bool& all_atoms,
                                                    bool& save_vines);

int main(int argc, char** argv)
{
#ifdef LOGGING
	rlog::RLogInit(argc, argv);
#endif

    std::string                 input_fn_list, output_fn;
    bool                        all_atoms = false, save_vines = false;
    program_options(argc, argv, input_fn_list, output_fn, all_atoms, save_vines);

	// Compute initial filtration
    std::vector<std::string> frame_fns;
    std::ifstream in_fns(input_fn_list.c_str());
    std::string fn;
    while(std::getline(in_fns, fn))
        frame_fns.push_back(fn);
        
	std::ifstream in(frame_fns[0].c_str());
    PDBDistanceGrid ginit(in, !all_atoms);
	in.close();

    typedef                     LSVineyard<Grid2D::CoordinateIndex, Grid2D>             Grid2DVineyard;
    
    Grid2DVineyard::LSFiltration        simplices;    
    ginit.complex_generator(make_push_back_functor(simplices));
    Grid2DVineyard::VertexComparison    vcmp(ginit);
    Grid2DVineyard::SimplexComparison   scmp(vcmp);
    simplices.sort(scmp);
    std::cout << "Complex generated, size: " << simplices.size() << std::endl;

    Grid2DVineyard              v(ginit.begin(), ginit.end(), simplices, ginit);
	std::cout << "Filtration generated, size: " << v.filtration().size() << std::endl;
	std::cout << "Pairing computed" << std::endl;

	// Process frames computing the vineyard
	for (size_t i = 1; i < frame_fns.size(); ++i)
	{
		std::cout << "Processing " << frame_fns[i] << std::endl;
		in.open(frame_fns[i].c_str());
		v.compute_vineyard(PDBDistanceGrid(in, !all_atoms));
		in.close();
	}
	std::cout << "Vineyard computed" << std::endl;

    if (save_vines)
        v.vineyard().save_vines(output_fn);
    else
    	v.vineyard().save_edges(output_fn);

#if 0
	std::ofstream ofs(output_fn.c_str(), std::ios::binary);
	boost::archive::binary_oarchive oa(ofs);
	oa << make_nvp("Filtration", pgf);
	ofs.close();
#endif
}

void        program_options(int argc, char* argv[], std::string& input_fn_list, 
                                                    std::string& output_prefix, 
                                                    bool& all_atoms,
                                                    bool& save_vines)
{
    namespace po = boost::program_options;

    po::options_description     hidden("Hidden options");
    hidden.add_options()
        ("input-fn-list",       po::value<std::string>(&input_fn_list),         "prefix of the input frames")
        ("output-prefix",       po::value<std::string>(&output_prefix),         "output prefix");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("all-atoms,a",         po::bool_switch(&all_atoms),                    "process all atoms (not only alpha carbons)")
        ("save-vines,v",        po::bool_switch(&save_vines),                   "save vines instead of edges")
        ("help,h",                                                              "produce help message");
#if LOGGING
    std::vector<std::string>    log_channels;
    visible.add_options()
        ("log,l",               po::value< std::vector<std::string> >(&log_channels),           "log channels to turn on (info, debug, etc)");
#endif

    po::positional_options_description pos;
    pos.add("input-fn-list", 1);
    pos.add("output-prefix", 1);
    
    po::options_description all; all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(pos).run(), vm);
    po::notify(vm);

#if LOGGING
    for (std::vector<std::string>::const_iterator cur = log_channels.begin(); cur != log_channels.end(); ++cur)
        stderrLog.subscribeTo( RLOG_CHANNEL(cur->c_str()) );
#endif

    if (vm.count("help") || !vm.count("input-fn-list") || !vm.count("output-prefix"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] input-fn-list output-prefix" << std::endl;
        std::cout << visible << std::endl; 
		std::cout << std::endl;
		std::cout << "Computes a vineyard of the pairwise distance function for a sequence of PDB frames." << std::endl;
		std::cout << "Frames are listed in input-fn-list file." << std::endl;
        std::abort();
    }
}
                                                    
