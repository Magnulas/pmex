#include <topology/rips.h>
#include <topology/image-zigzag-persistence.h>
#include <utilities/types.h>
#include <utilities/containers.h>

#include <utilities/log.h>
#include <utilities/memory.h>       // for report_memory()
#include <utilities/timer.h>

#include <geometry/l2distance.h>    // for L2Distance and read_points()
#include <geometry/distances.h>

#include <map>
#include <cmath>
#include <fstream>
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
typedef     Simplex<Vertex>                                         Smplx;
typedef     std::vector<Smplx>                                      SimplexVector;
typedef     std::list<Smplx>                                        SimplexList;
typedef     std::set<Smplx, Smplx::VertexDimensionComparison>       SimplexSet;

typedef     std::vector<Vertex>                                     VertexVector;
typedef     std::vector<DistanceType>                               EpsilonVector;
typedef     std::vector<std::pair<Vertex, Vertex> >                 EdgeVector;

typedef     Rips<PairDistances, Smplx>                              RipsGenerator;
typedef     RipsGenerator::Evaluator                                SimplexEvaluator;

struct      BirthInfo;
typedef     ImageZigzagPersistence<BirthInfo>                       Zigzag;
typedef     Zigzag::SimplexIndex                                    Index;
typedef     Zigzag::Death                                           Death;
typedef     std::map<Smplx, Index, 
                            Smplx::VertexDimensionComparison>       Complex;
typedef     Zigzag::ZColumn                                         Boundary;

// Information we need to know when a class dies
struct      BirthInfo
{
                    BirthInfo(DistanceType dist = DistanceType(), Dimension dim = Dimension()):
                        distance(dist), dimension(dim)              {}
    DistanceType    distance;
    Dimension       dimension;
};

// Forward declarations of auxilliary functions
void        report_death(std::ofstream& out, Death d, DistanceType epsilon, Dimension skeleton_dimension);
void        make_boundary(const Smplx& s, Complex& c, const Zigzag& zz, Boundary& b);
void        show_image_betti(Zigzag& zz, Dimension skeleton);
std::ostream&   operator<<(std::ostream& out, const BirthInfo& bi);
void        process_command_line_options(int           argc,
                                         char*         argv[],
                                         unsigned&     skeleton_dimension,
                                         float&        from_multiplier, 
                                         float&        to_multiplier,
                                         std::string&  infilename,
                                         std::string&  outfilename);

int main(int argc, char* argv[])
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stderrLog.subscribeTo( RLOG_CHANNEL("error") );
#endif

    Timer total, remove, add, ec, vc;
    total.start();
 
#if 0
    SetFrequency(cOperations, 25000);
    SetTrigger(cOperations, cComplexSize);
#endif

    unsigned        skeleton_dimension;
    float           from_multiplier, to_multiplier;
    std::string     infilename, outfilename;
    process_command_line_options(argc, argv, skeleton_dimension, from_multiplier, to_multiplier, infilename, outfilename);

    // Read in points
    PointContainer      points;
    read_points(infilename, points);
    
    // Create output file
    std::ofstream out(outfilename.c_str());

    // Create pairwise distances
    PairDistances distances(points);
    
    // Order vertices and epsilons (in maxmin fashion)
    VertexVector        vertices;
    EpsilonVector       epsilons;
    EdgeVector          edges;

    {
        EpsilonVector   dist(distances.size(), Infinity);
    
        vertices.push_back(distances.begin());
        //epsilons.push_back(Infinity);
        while (vertices.size() < distances.size())
        {
            for (Vertex v = distances.begin(); v != distances.end(); ++v)
                dist[v] = std::min(dist[v], distances(v, vertices.back()));
            EpsilonVector::const_iterator max = std::max_element(dist.begin(), dist.end());
            vertices.push_back(max - dist.begin());
            epsilons.push_back(*max);
        }
        epsilons.push_back(0);
    }

    rInfo("Point and epsilon ordering:");
    for (unsigned i = 0; i < vertices.size(); ++i)
        rInfo("  %4d: %4d - %f", i, vertices[i], epsilons[i]);

    // Generate and sort all the edges
    for (unsigned i = 0; i != vertices.size(); ++i)
        for (unsigned j = i+1; j != vertices.size(); ++j)
        {
            Vertex u = vertices[i];
            Vertex v = vertices[j];
            if (distances(u,v) <= to_multiplier*epsilons[j-1])
                edges.push_back(std::make_pair(u,v));
        }
    std::sort(edges.begin(), edges.end(), RipsGenerator::ComparePair(distances));
    rInfo("Total participating edges: %d", edges.size());
    for (EdgeVector::const_iterator cur = edges.begin(); cur != edges.end(); ++cur)
        rDebug("  (%d, %d) %f", cur->first, cur->second, distances(cur->first, cur->second));

    // Construct zigzag
    Complex             complex;
    Zigzag              zz;
    RipsGenerator       rips(distances);
    SimplexEvaluator    size(distances);
    
    // Insert vertices
    for (unsigned i = 0; i != vertices.size(); ++i)
    {
        // Add a vertex
        Smplx sv; sv.add(vertices[i]);
        rDebug("Adding %s", tostring(sv).c_str());
        add.start();
        complex.insert(std::make_pair(sv, 
                                      zz.add(Boundary(), 
                                             true,         // vertex is always in the subcomplex
                                             BirthInfo(0, 0)).first));
        add.stop();
        //rDebug("Newly born cycle order: %d", complex[sv]->low->order);
        CountNum(cComplexSize, 0);
        Count(cComplexSize);
        Count(cOperations);
    }

    rInfo("Commencing computation");
    boost::progress_display show_progress(vertices.size());
    unsigned sce    = 0,        // index of the current one past last edge in the subcomplex
             ce     = 0;        // index of the current one past last edge in the complex
    for (unsigned stage = 0; stage != vertices.size() - 1; ++stage)
    {
        unsigned i = vertices.size() - 1 - stage;
        rInfo("Current stage %d: %d %f: %f -> %f", stage, 
                                                   vertices[i], epsilons[i-1],
                                                   from_multiplier*epsilons[i-1],
                                                   to_multiplier*epsilons[i-1]);

        /* Increase epsilon */
        // Record the cofaces of all the simplices that need to be removed and reinserted
        SimplexSet cofaces;
        rDebug("  Cofaces size: %d", cofaces.size());
        while(sce < ce)
        {
            Vertex u,v;
            boost::tie(u,v)     = edges[sce];
            if (distances(u,v) <= from_multiplier*epsilons[i-1])
                ++sce;
            else
                break;

            // Skip an edge if any one of its vertices has been removed from the complex
            bool skip_edge = false;
            for (unsigned j = i+1; j != vertices.size(); ++j)
                if (u == vertices[j] || v == vertices[j])
                {
                    // Debug only: eventually remove
                    rDebug("  Skipping edge (%d, %d)", u, v);
                    Smplx s; s.add(u); s.add(v);
                    AssertMsg(complex.find(s) == complex.end(), "Simplex should not be in the complex.");
                    skip_edge = true;
                    break;
                }
            if (skip_edge) continue;
            rDebug("  Generating cofaces for (%d, %d)", u, v);
        
            ec.start();
            rips.edge_cofaces(u, v, 
                              skeleton_dimension, 
                              to_multiplier*epsilons[i], 
                              make_insert_functor(cofaces),
                              vertices.begin(),
                              vertices.begin() + i + 1);
            ec.stop();
        }
        rDebug("  Recorded cofaces to remove");
        rDebug("  Cofaces size: %d", cofaces.size());
        // Remove all the cofaces
        for (SimplexSet::const_reverse_iterator cur = cofaces.rbegin(); cur != (SimplexSet::const_reverse_iterator)cofaces.rend(); ++cur)
        {
            rDebug("    Removing %s", tostring(*cur).c_str());
            Complex::iterator si = complex.find(*cur);
            remove.start();
            AssertMsg(!si->second->subcomplex, "We should not remove simplices already in the subcomplex when we increase epsilon");
            Death d = zz.remove(si->second,
                                BirthInfo(epsilons[i-1], cur->dimension() - 1));
            remove.stop();
            complex.erase(si);
            CountNumBy(cComplexSize, cur->dimension(), -1);
            CountBy(cComplexSize, -1);
            Count(cOperations);
            AssertMsg(zz.check_consistency(), "Zigzag representation must be consistent after removing a simplex");
            report_death(out, d, epsilons[i-1], skeleton_dimension);
        }
        rDebug("  Removed cofaces");

        // Add anything else that needs to be inserted into the complex
        while (ce < edges.size())
        {
            Vertex u,v;
            boost::tie(u,v)     = edges[ce];
            if (distances(u,v) <= to_multiplier*epsilons[i-1])
                ++ce;
            else
                break;
            rDebug("  Recording cofaces of edges[%d]=(%d, %d) with size=%f", (ce-1), u, v, distances(u,v));
            ec.start();
            rips.edge_cofaces(u, v, 
                              skeleton_dimension, 
                              to_multiplier*epsilons[i-1], 
                              make_insert_functor(cofaces),
                              vertices.begin(),
                              vertices.begin() + i + 1);
            ec.stop();
        }
        rDebug("  Recorded new cofaces to add");

        // Progress sce
        while (sce < ce)
        {
            Vertex u,v;
            boost::tie(u,v)     = edges[sce];
            rDebug("    Progressing sce=%d over (%d, %d) %f", sce, u, v, distances(u,v));
            if (distances(u,v) <= from_multiplier*epsilons[i-1])   
                ++sce;
            else
                break;
        }
        rDebug("  Moved subcomplex index forward");

        // Insert all the cofaces
        rDebug("  Cofaces size: %d", cofaces.size());
        for (SimplexSet::const_iterator cur = cofaces.begin(); cur != cofaces.end(); ++cur)
        {
            Index idx; Death d; Boundary b;
            rDebug("  Adding %s, its size %f", tostring(*cur).c_str(), size(*cur));
            make_boundary(*cur, complex, zz, b);
            add.start();
            boost::tie(idx, d)  = zz.add(b,
                                         size(*cur) <= from_multiplier*epsilons[i-1],
                                         BirthInfo(epsilons[i-1], cur->dimension()));
            add.stop();
            //if (!d) rDebug("Newly born cycle order: %d", complex[*cur]->low->order);
            CountNum(cComplexSize, cur->dimension());
            Count(cComplexSize);
            Count(cOperations);
            AssertMsg(zz.check_consistency(), "Zigzag representation must be consistent after removing a simplex");
            complex.insert(std::make_pair(*cur, idx));
            report_death(out, d, epsilons[i-1], skeleton_dimension);
        }
        rInfo("Increased epsilon; complex size: %d", complex.size());
        show_image_betti(zz, skeleton_dimension);
        report_memory();
        
        /* Remove the vertex */
        cofaces.clear();
        rDebug("  Cofaces size: %d", cofaces.size());
        vc.start();
        rips.vertex_cofaces(vertices[i], 
                            skeleton_dimension, 
                            to_multiplier*epsilons[i-1], 
                            make_insert_functor(cofaces),
                            vertices.begin(),
                            vertices.begin() + i + 1);
        vc.stop();
        rDebug("  Computed cofaces of the vertex, their number: %d", cofaces.size());
        for (SimplexSet::const_reverse_iterator cur = cofaces.rbegin(); cur != (SimplexSet::const_reverse_iterator)cofaces.rend(); ++cur)
        {
            rDebug("    Removing: %s", tostring(*cur).c_str());
            Complex::iterator si = complex.find(*cur);
            remove.start();
            Death d = zz.remove(si->second,
                                BirthInfo(epsilons[i-1], cur->dimension() - 1));
            remove.stop();
            complex.erase(si);
            CountNumBy(cComplexSize, cur->dimension(), -1);
            CountBy(cComplexSize, -1);
            Count(cOperations);
            AssertMsg(zz.check_consistency(), "Zigzag representation must be consistent after removing a simplex");
            report_death(out, d, epsilons[i-1], skeleton_dimension);
        }
        rInfo("Removed vertex; complex size: %d", complex.size());
        for (Complex::const_iterator cur = complex.begin(); cur != complex.end(); ++cur)
            rDebug("  %s", tostring(cur->first).c_str());
        show_image_betti(zz, skeleton_dimension);
        report_memory();
        
        ++show_progress;
    }
    
    // Remove the last vertex
    AssertMsg(complex.size() == 1, "Only one vertex must remain");
    remove.start();
    Death d = zz.remove(complex.begin()->second, BirthInfo(epsilons[0], -1));
    remove.stop();
    complex.erase(complex.begin());
    if (!d)  AssertMsg(false,  "The vertex must have died");
    report_death(out, d, epsilons[0], skeleton_dimension);
    CountNumBy(cComplexSize, 0, -1);
    CountBy(cComplexSize, -1);
    Count(cOperations);
    rInfo("Removed vertex; complex size: %d", complex.size());
    ++show_progress;

    total.stop();

    remove.check("Remove timer          ");
    add.check   ("Add timer             ");
    ec.check    ("Edge coface timer     ");
    vc.check    ("Vertex coface timer   ");
    total.check ("Total timer           ");
}


            
void        report_death(std::ofstream& out, Death d, DistanceType epsilon, Dimension skeleton_dimension)
{
    if (d && ((d->distance - epsilon) != 0) && (d->dimension < skeleton_dimension))
        out << d->dimension << " " << d->distance << " " << epsilon << std::endl;
}

void        make_boundary(const Smplx& s, Complex& c, const Zigzag& zz, Boundary& b)
{
    rDebug("  Boundary of <%s>", tostring(s).c_str());
    for (Smplx::BoundaryIterator cur = s.boundary_begin(); cur != s.boundary_end(); ++cur)
    {
        b.append(c[*cur], zz.cmp);
        rDebug("   %d (inL=%d)", c[*cur]->order, b.back()->subcomplex);
    }
}

bool        face_leaving_subcomplex(Complex::reverse_iterator si, const SimplexEvaluator& size, DistanceType after, DistanceType before)
{
    const Smplx& s = si->first;
    for (Smplx::VertexContainer::const_iterator v1 = s.vertices().begin(); v1 != s.vertices().end(); ++v1)
        for (Smplx::VertexContainer::const_iterator v2 = boost::next(v1); v2 != s.vertices().end(); ++v2)
        {
            Smplx e; e.add(*v1); e.add(*v2);
            if (size(e) > after && size(e) <= before)
                return true;
        }

    return false;
}

void        show_image_betti(Zigzag& zz, Dimension skeleton)
{
    for (Zigzag::ZIndex cur = zz.image_begin(); cur != zz.image_end(); ++cur)
        if (cur->low == zz.boundary_end() && cur->birth.dimension < skeleton)
            rInfo("Class in the image of dimension: %d",  cur->birth.dimension);
}


std::ostream&   operator<<(std::ostream& out, const BirthInfo& bi)
{ return (out << bi.distance); }

void        process_command_line_options(int           argc, 
                                         char*         argv[],
                                         unsigned&     skeleton_dimension,
                                         float&        from_multiplier, 
                                         float&        to_multiplier,
                                         std::string&  infilename,
                                         std::string&  outfilename)
{
    namespace po = boost::program_options;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file",          po::value<std::string>(&infilename),        "Point set whose Rips zigzag we want to compute")
        ("output-file",         po::value<std::string>(&outfilename),       "Location to save persistence pairs");
    
    po::options_description visible("Allowed options", 100);
    visible.add_options()
        ("help,h",                                                                              "produce help message")
        ("skeleton-dimsnion,s", po::value<unsigned>(&skeleton_dimension)->default_value(2),     "Dimension of the Rips complex we want to compute")
        ("from,f",              po::value<float>(&from_multiplier)->default_value(4),           "From multiplier for the epsilon (distance to next maxmin point) when computing the Rips complex")
        ("to,t",                po::value<float>(&to_multiplier)->default_value(16),            "To multiplier for the epsilon (distance to next maxmin point) when computing the Rips complex");
#if LOGGING
    std::vector<std::string>    log_channels;
    visible.add_options()
        ("log,l",               po::value< std::vector<std::string> >(&log_channels),           "log channels to turn on (info, debug, etc)");
#endif

    po::positional_options_description pos;
    pos.add("input-file", 1);
    pos.add("output-file", 2);
    
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

    if (vm.count("help") || !vm.count("input-file") || !vm.count("output-file"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] input-file output-file" << std::endl;
        std::cout << visible << std::endl; 
        std::abort();
    }
}
