#include <utilities/log.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <topology/lsvineyard.h>

#include <boost/program_options.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
namespace bl = boost::lambda;


typedef     double                                      VertexValue;
typedef     unsigned                                    Vertex;
typedef     std::vector<VertexValue>                    VertexVector;
struct SubscriptFunctor: public std::unary_function<Vertex, VertexValue>
{
                                SubscriptFunctor(const VertexVector& v): vec(&v)    {}
        float                   operator()(Vertex i) const                          { return (*vec)[i]; }
        SubscriptFunctor&       operator=(const SubscriptFunctor& other)            { vec = other.vec; return *this; }
        const VertexVector*     vec;
};
typedef     SubscriptFunctor                            VertexEvaluator;
typedef     std::vector<VertexVector>                   VertexVectorVector;
typedef     LSVineyard<Vertex, VertexEvaluator>         PLVineyard;
typedef     PLVineyard::Simplex                         Smplx;              // gotta start using namespaces

void        program_options(int argc, char* argv[], std::string& complex_fn, 
                                                    std::string& values_fn, 
                                                    std::string& output_prefix, 
                                                    bool& skip_infinite_vines, 
                                                    bool& save_vines,
                                                    bool& explicit_events);
void        read_simplices(const std::string& complex_fn, PLVineyard::LSFiltration& simplices);
void        read_vertices(const std::string& vertex_fn, VertexVectorVector& vertices); 


int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);
#endif     
    
    std::string                 complex_fn, values_fn, output_prefix;
    bool                        skip_infinite_vines = false, explicit_events = false, save_vines = false;
    program_options(argc, argv, complex_fn, values_fn, output_prefix, skip_infinite_vines, save_vines, explicit_events);


    // Read in the complex
    PLVineyard::LSFiltration            simplices;
    read_simplices(complex_fn, simplices);
    std::cout << "Complex read, size: " << simplices.size() << std::endl;

    // Read in vertex values
    VertexVectorVector                  vertices;
    read_vertices(values_fn, vertices);

    // Setup the vineyard
    VertexEvaluator                     veval(vertices[0]);
    PLVineyard::VertexComparison        vcmp(veval);
    PLVineyard::SimplexComparison       scmp(vcmp);
    simplices.sort(scmp);
    PLVineyard                      v(boost::counting_iterator<Vertex>(0),
                                      boost::counting_iterator<Vertex>(vertices[0].size()), 
                                      simplices, veval);
    std::cout << "Pairing computed" << std::endl;

    // Compute vineyard
    for (size_t i = 1; i < vertices.size(); ++i)
    {
        veval = VertexEvaluator(vertices[i]);
        v.compute_vineyard(veval);
        std::cout << "Processed frame: " << i << std::endl;
    }
    std::cout << "Vineyard computed" << std::endl;
    
    if (save_vines)
        v.vineyard().save_vines(output_prefix, skip_infinite_vines);
    else
        v.vineyard().save_edges(output_prefix, skip_infinite_vines);
}


void        read_simplices(const std::string& complex_fn, PLVineyard::LSFiltration& simplices)
{
    std::ifstream   in(complex_fn.c_str());
    std::string     line;
    while (std::getline(in, line))
    {
        std::istringstream  strin(line);
        simplices.push_back(Smplx(std::istream_iterator<Vertex>(strin), std::istream_iterator<Vertex>()));
    }
    std::cout << "Simplices read:" << std::endl;
    std::copy(simplices.begin(), simplices.end(), std::ostream_iterator<Smplx>(std::cout, "\n"));
}

void        read_vertices(const std::string& vertex_fn, VertexVectorVector& vertices)
{
    std::ifstream   in(vertex_fn.c_str());
    std::string     line;
    while (std::getline(in, line))
    {
        std::istringstream  strin(line);
        vertices.push_back(VertexVector(std::istream_iterator<VertexValue>(strin), std::istream_iterator<VertexValue>()));
    }
    std::cout << "Vertex values read:" << std::endl;
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        std::copy(vertices[i].begin(), vertices[i].end(), std::ostream_iterator<VertexValue>(std::cout, " "));
        std::cout << std::endl;
    }
}

void        program_options(int argc, char* argv[], std::string& complex_fn, 
                                                    std::string& values_fn, 
                                                    std::string& output_prefix, 
                                                    bool& skip_infinite_vines,
                                                    bool& save_vines,
                                                    bool& explicit_events)
{
    namespace po = boost::program_options;

    po::options_description     hidden("Hidden options");
    hidden.add_options()
        ("complex-file",        po::value<std::string>(&complex_fn),            "file listing the simplices of the complex")
        ("values-file",         po::value<std::string>(&values_fn),             "file listing the values at the vertices")
        ("output-prefix",       po::value<std::string>(&output_prefix),         "output prefix");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("skip-infinite,s",     po::bool_switch(&skip_infinite_vines),                          "skip infinite vines in the output")
        ("explicit-events,e",   po::bool_switch(&explicit_events),                              "process kinetic sort events one by one")
        ("save-vines,v",        po::bool_switch(&save_vines),                                   "save vines instead of edges")
        ("help,h",                                                                              "produce help message");
#if LOGGING
    std::vector<std::string>    log_channels;
    visible.add_options()
        ("log,l",               po::value< std::vector<std::string> >(&log_channels),           "log channels to turn on (info, debug, etc)");
#endif

    po::positional_options_description pos;
    pos.add("complex-file", 1);
    pos.add("values-file", 1);
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

    if (vm.count("help") || !vm.count("complex-file") || !vm.count("values-file") || !vm.count("output-prefix"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] complex-file values-file output-prefix" << std::endl;
        std::cout << visible << std::endl; 
        std::abort();
    }
}
