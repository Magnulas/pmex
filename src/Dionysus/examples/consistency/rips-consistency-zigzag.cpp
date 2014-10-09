#include <topology/rips.h>
#include <topology/zigzag-persistence.h>

#include <geometry/l2distance.h>    // Point, PointContainer, L2DistanceType, read_points
#include <geometry/distances.h>

#include <utilities/types.h>
#include <utilities/containers.h>
#include <utilities/log.h>
#include <utilities/memory.h>       // for report_memory()
#include <utilities/timer.h>

#include <map>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stack>
#include <cstdlib>

#include <boost/tuple/tuple.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>

#ifdef COUNTERS
static Counter*  cComplexSize =                     GetCounter("rips/size");
static Counter*  cOperations =                      GetCounter("rips/operations");
#endif // COUNTERS

typedef     PairwiseDistances<PointContainer, L2Distance>           PairDistances;
typedef     PairDistances::DistanceType                             DistanceType;

typedef     PairDistances::IndexType                                Vertex;
typedef     std::vector<Vertex>                                     SubsampleVector;

typedef     Simplex<Vertex>                                         Smplx;
typedef     std::vector<Smplx>                                      SimplexVector;
typedef     std::set<Smplx, Smplx::VertexDimensionComparison>       SimplexSet;

typedef     std::vector<Vertex>                                     VertexVector;
typedef     std::vector<DistanceType>                               EpsilonVector;
typedef     std::vector<std::pair<Vertex, Vertex> >                 EdgeVector;

typedef     Rips<PairDistances, Smplx>                              RipsGenerator;
typedef     RipsGenerator::Evaluator                                SimplexEvaluator;

struct      BirthInfo;
typedef     ZigzagPersistence<BirthInfo>                            Zigzag;
typedef     Zigzag::SimplexIndex                                    Index;
typedef     Zigzag::Death                                           Death;
typedef     std::map<Smplx, Index, 
                            Smplx::VertexDimensionComparison>       Complex;
typedef     Zigzag::ZColumn                                         Boundary;

// Information we need to know when a class dies
struct      BirthInfo
{
                    BirthInfo(Dimension d = 0, unsigned i = 0, bool u = false):
                        dimension(d), index(i), un(u)               {}
    
    bool            operator<(const BirthInfo& other) const         { if (index == other.index) return (!un && other.un); else return index < other.index; }
    bool            operator>(const BirthInfo& other) const         { return other.operator<(*this); }
    bool            operator>=(const BirthInfo& other) const        { return !operator<(other); }
    bool            operator<=(const BirthInfo& other) const        { return !operator>(other); }

    Dimension       dimension;
    unsigned        index;
    bool            un;
};
std::ostream&       operator<<(std::ostream& out, const BirthInfo& bi)
{ out << "(d=" << bi.dimension << ") " << bi.index; if (bi.un) out << " U " << (bi.index + 1); return out; }

// Forward declarations of auxilliary functions
void        report_death(std::ostream& out, const BirthInfo& birth, const BirthInfo& death, unsigned skeleton_dimension);
void        make_boundary(const Smplx& s, Complex& c, const Zigzag& zz, Boundary& b);
void        process_command_line_options(int           argc,
                                         char*         argv[],
                                         unsigned&     skeleton_dimension,
                                         float&        epsilon,
                                         std::string&  points_filename,
                                         std::string&  subsample_filename,
                                         std::string&  outfilename);
void        add_simplex(const Smplx& s, const BirthInfo& birth, const BirthInfo& death, 
                        Complex& complex, Zigzag& zz, std::ostream& out, Timer& add, unsigned skeleton_dimension);
void        remove_simplex(const Smplx& s, const BirthInfo& birth, const BirthInfo& death, 
                           Complex& complex, Zigzag& zz, std::ostream& out, Timer& remove, unsigned skeleton_dimension);

int main(int argc, char* argv[])
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stderrLog.subscribeTo( RLOG_CHANNEL("error") );
#endif

    Timer total, remove, add, coface, generate;
    total.start();
 
#if 0
    SetFrequency(cOperations, 25000);
    SetTrigger(cOperations, cComplexSize);
#endif

    unsigned        skeleton_dimension;
    float           epsilon;
    std::string     points_filename, subsample_filename, outfilename;
    process_command_line_options(argc, argv, skeleton_dimension, epsilon, 
                                             points_filename, subsample_filename, outfilename);

    // Read in points
    PointContainer      points;
    read_points(points_filename, points);
    
    // Read in subsamples
    std::ifstream subsample_in(subsample_filename.c_str());
    std::vector<SubsampleVector> subsamples;
    subsamples.push_back(SubsampleVector());    //  Empty subsample to start from
    while(subsample_in)
    {
        std::string line; std::getline(subsample_in, line);
        std::istringstream line_in(line);
        subsamples.push_back(SubsampleVector());
        unsigned sample;
        while (line_in >> sample)
            subsamples.back().push_back(sample);
    }
    AssertMsg(subsamples.front().size() == 0, "The first subsample should be empty");
    AssertMsg(subsamples.back().size() == 0, "The last subsample should be empty");     // it's a convenient artifact of the stream processing above
    
    std::cout << "Subsample size:" << std::endl;
    for (unsigned i = 0; i < subsamples.size(); ++i)
        std::cout << "  " << subsamples[i].size() << std::endl;

    // Create output file
    std::ofstream out(outfilename.c_str());

    // Create pairwise distances
    PairDistances distances(points);
    

    // Construct zigzag
    Complex             complex;
    Zigzag              zz;
    RipsGenerator       rips(distances);
    SimplexEvaluator    size(distances);
    SimplexVector       subcomplex, across;
    
    rInfo("Commencing computation");
    boost::progress_display show_progress(subsamples.size() - 1);
    for (unsigned i = 0; i < subsamples.size() - 1; ++i)
    {
        // Take union of subsamples i and i+1
        SubsampleVector forward_difference, backward_difference;
        std::set_difference(subsamples[i+1].begin(), subsamples[i+1].end(),
                            subsamples[i].begin(),   subsamples[i].end(),
                            std::back_inserter(forward_difference));
        std::set_difference(subsamples[i].begin(),   subsamples[i].end(),
                            subsamples[i+1].begin(), subsamples[i+1].end(),
                            std::back_inserter(backward_difference));
        rInfo("Forward difference size:  %d", forward_difference.size());
        rInfo("Backward difference size: %d", backward_difference.size());

#if LOGGING
        rDebug("Forward difference:");
        for (SubsampleVector::const_iterator cur = forward_difference.begin(); cur != forward_difference.end(); ++cur)
            rDebug("  %d", *cur);
        
        rDebug("Backward difference:");
        for (SubsampleVector::const_iterator cur = backward_difference.begin(); cur != backward_difference.end(); ++cur)
            rDebug("  %d", *cur);
#endif

        // Add simplices:       take star of the vertices in subsamples[i+1] - subsamples[i]
        //                      by first computing the Rips complex on those vertices, and then 
        //                      taking the star of each individual simplex (perhaps not the most 
        //                      efficient procedure, but it will do for the first pass)
        rInfo("Adding");
        subcomplex.clear(); across.clear();
        generate.start();
        rips.generate(skeleton_dimension, epsilon, make_push_back_functor(subcomplex), forward_difference.begin(), forward_difference.end());
        std::sort(subcomplex.begin(), subcomplex.end(), Smplx::VertexDimensionComparison());
        generate.stop();
        rInfo("  Subcomplex size: %d", subcomplex.size());
        for (SimplexVector::const_iterator cur = subcomplex.begin(); cur != subcomplex.end(); ++cur)
        {
            add_simplex(*cur, BirthInfo(cur->dimension(), i, true), BirthInfo(cur->dimension() - 1, i), 
                        complex, zz, out, add, skeleton_dimension);

            // Record cofaces with all other vertices outside of the subcomplex
            coface.start();
            rips.cofaces(*cur, skeleton_dimension, epsilon, make_push_back_functor(across), subsamples[i].begin(), subsamples[i].end());
            coface.stop();
        }
        std::sort(across.begin(), across.end(), Smplx::VertexDimensionComparison());
        rInfo("  Subcomplex simplices added");
        rInfo("  Cross simplices size: %d", across.size());

        // Add simplices that cut across VR(K_i) and VR(K_{i+1})
        for (SimplexVector::const_iterator cur = across.begin(); cur != across.end(); ++cur)
            add_simplex(*cur, BirthInfo(cur->dimension(), i, true), BirthInfo(cur->dimension() - 1, i), 
                        complex, zz, out, add, skeleton_dimension);
        rInfo("  Cross simplices added");


        // Remove simplices:    take star of the vertices in subsamples[i] - subsamples[i+1]
        rInfo("Removing");
        subcomplex.clear(); across.clear();
        generate.start();
        rips.generate(skeleton_dimension, epsilon, make_push_back_functor(subcomplex), backward_difference.begin(), backward_difference.end());
        std::sort(subcomplex.begin(), subcomplex.end(), Smplx::VertexDimensionComparison());
        generate.stop();
        rInfo("  Subcomplex size: %d", subcomplex.size());

        for (SimplexVector::const_iterator cur = subcomplex.begin(); cur != subcomplex.end(); ++cur)
            rips.cofaces(*cur, skeleton_dimension, epsilon, make_push_back_functor(across), subsamples[i+1].begin(), subsamples[i+1].end());
        std::sort(across.begin(), across.end(), Smplx::VertexDimensionComparison());
        rInfo("  Cross simplices size: %d", across.size());

        for (SimplexVector::const_reverse_iterator cur = across.rbegin(); cur != (SimplexVector::const_reverse_iterator)across.rend(); ++cur)
            remove_simplex(*cur, BirthInfo(cur->dimension() - 1, i+1), BirthInfo(cur->dimension(), i, true),
                           complex, zz, out, remove, skeleton_dimension);
        rInfo("  Cross simplices removed");

        for (SimplexVector::const_reverse_iterator cur = subcomplex.rbegin(); cur != (SimplexVector::const_reverse_iterator)subcomplex.rend(); ++cur)
            remove_simplex(*cur, BirthInfo(cur->dimension() - 1, i+1), BirthInfo(cur->dimension(), i, true),
                           complex, zz, out, remove, skeleton_dimension);
        rInfo("  Subcomplex simplices removed");
        
        Dimension betti_1 = 0;
        for (Zigzag::ZIndex cur = zz.begin(); cur != zz.end(); ++cur)
            if (cur->birth.dimension == 1 && zz.is_alive(cur)) ++betti_1;
        
        rInfo("Complex size: %d, Betti_1 = %d", complex.size(), betti_1);
        
        ++show_progress;
    }

    std::cout << std::endl;
    total.stop();
    remove.check("Remove timer          ");
    add.check   ("Add timer             ");
    coface.check("Coface timer          ");
    total.check ("Total timer           ");
}


void        make_boundary(const Smplx& s, Complex& c, const Zigzag& zz, Boundary& b)
{
    // rDebug("  Boundary of <%s>", tostring(s).c_str());
    for (Smplx::BoundaryIterator cur = s.boundary_begin(); cur != s.boundary_end(); ++cur)
    {
        b.append(c[*cur], zz.cmp);
        // rDebug("   %d", c[*cur]->order);
    }
}

void        report_death(std::ostream& out, const BirthInfo& birth, const BirthInfo& death, unsigned skeleton_dimension)
{
    if (birth.dimension < skeleton_dimension && death >= birth)
        out << birth << " --- " << death << std::endl;
}

void        add_simplex(const Smplx& s, const BirthInfo& birth, const BirthInfo& death, 
                        Complex& complex, Zigzag& zz, std::ostream& out, Timer& add, unsigned skeleton_dimension)
{
    rDebug("Adding simplex: %s", tostring(s).c_str());

    Index idx; Death d; Boundary b;
    make_boundary(s, complex, zz, b);
    add.start();
    boost::tie(idx, d)  = zz.add(b, birth);
    if (d) report_death(out, *d, death, skeleton_dimension);
    add.stop();
    complex.insert(std::make_pair(s, idx));
}

void        remove_simplex(const Smplx& s, const BirthInfo& birth, const BirthInfo& death, 
                           Complex& complex, Zigzag& zz, std::ostream& out, Timer& remove, unsigned skeleton_dimension)
{
    rDebug("Removing simplex: %s", tostring(s).c_str());

    Complex::iterator si = complex.find(s);
    remove.start();
    Death d = zz.remove(si->second, birth);
    remove.stop();
    if (d) report_death(out, *d, death, skeleton_dimension);
    complex.erase(si);
}

void        process_command_line_options(int           argc,
                                         char*         argv[],
                                         unsigned&     skeleton_dimension,
                                         float&        epsilon,
                                         std::string&  points_filename,
                                         std::string&  subsample_filename,
                                         std::string&  outfilename)
{
    namespace po = boost::program_options;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("points-file",         po::value<std::string>(&points_filename),   "Point set whose Rips consistency zigzag we want to compute")
        ("subsample-file",      po::value<std::string>(&subsample_filename),"Subsample list")
        ("output-file",         po::value<std::string>(&outfilename),       "Location to save persistence pairs");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("help,h",                                                                              "produce help message")
        ("skeleton-dimsnion,s", po::value<unsigned>(&skeleton_dimension)->default_value(2),     "Dimension of the Rips complex we want to compute")
        ("epsilon,e",           po::value<float>(&epsilon)->default_value(0),                   "epsilon to use when computing the Rips complex");
#if LOGGING
    std::vector<std::string>    log_channels;
    visible.add_options()
        ("log,l",               po::value< std::vector<std::string> >(&log_channels),           "log channels to turn on (info, debug, etc)");
#endif

    po::positional_options_description pos;
    pos.add("points-file", 1);
    pos.add("subsample-file", 1);
    pos.add("output-file", 1);
    
    po::options_description all; all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(pos).run(), vm);
    po::notify(vm);

#if LOGGING
    for (std::vector<std::string>::const_iterator cur = log_channels.begin(); cur != log_channels.end(); ++cur)
        stderrLog.subscribeTo( RLOG_CHANNEL(cur->c_str()) );
    /**
     * Interesting channels
     * "info", "debug", "topology/persistence"
     */
#endif

    if (vm.count("help") || !vm.count("points-file") || !vm.count("subsample-file") || !vm.count("output-file"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] points-file subsample-file output-file" << std::endl;
        std::cout << visible << std::endl; 
        std::abort();
    }
}
