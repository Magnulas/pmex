/***************************************************************************

    oR-ZZ: oscillating Rips zigzag implementation
    Copyright (C) 2012   Steve Oudot

    Adapted from the Morozov zigzag implementation provided in the
    Rips package of the Dionysus library (see the file "M-ZZ.cpp").
    The Dionysus library is (C) 2006-2012 Dmitriy Morozov.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

***************************************************************************/

#include <topology/rips.h>
#include <topology/zigzag-persistence.h>
#include <utilities/types.h>
#include <utilities/containers.h>

#include <geometry/l2distance.h>    // Point, PointContainer, L2DistanceType, read_points
#include <geometry/distances.h>

#include <utilities/log.h>
#include <utilities/memory.h>       // for report_memory()
#include <utilities/timer.h>

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
typedef     ZigzagPersistence<BirthInfo>                            Zigzag;
typedef     Zigzag::SimplexIndex                                    Index;
typedef     Zigzag::Death                                           Death;
typedef     std::map<Smplx, Index, 
                            Smplx::VertexDimensionComparison>       Complex;
typedef     Zigzag::ZColumn                                         Boundary;

typedef     std::vector<std::list<std::pair<double, double> > >     IntervalsVector;

// Information we need to know when a class dies
struct      BirthInfo
{
                    BirthInfo(DistanceType dist = DistanceType(), Dimension dim = Dimension()):
                        distance(dist), dimension(dim)              {}
    DistanceType    distance;
    Dimension       dimension;
};

// Forward declarations of auxilliary functions
void write_intervals(std::ostream& out, const IntervalsVector& intervals, int skeleton_dimension, bool logscale, double min_value=0);
// void        report_death(std::ostream& out, Death d, DistanceType epsilon, Dimension skeleton_dimension);
void        report_death(IntervalsVector& intervals, Death d, DistanceType epsilon, DistanceType birthEpsilon, Dimension skeleton_dimension);
void        make_boundary(const Smplx& s, Complex& c, const Zigzag& zz, Boundary& b);
std::ostream&   operator<<(std::ostream& out, const BirthInfo& bi);
void        process_command_line_options(int           argc,
                                         char*         argv[],
                                         unsigned&     skeleton_dimension,
                                         float&        eta,
					 float&        rho,
					 bool&         logscale,
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
    float           eta;
    float           rho;
    bool            logscale = false;
    std::string     infilename, outfilename;
    process_command_line_options(argc, argv, skeleton_dimension, eta, rho, logscale, infilename, outfilename);

    // Read in points
    PointContainer      points;
    read_points(infilename, points);
    
    std::cout << "Number of points: " << points.size() << std::endl;

    // Create output file
    std::ofstream out(outfilename.c_str());

    // Create pairwise distances
    PairDistances distances(points);
    
    // Create intervals DS
    IntervalsVector intervals(skeleton_dimension);

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
            if (distances(u,v) <= rho*epsilons[j-1])
                edges.push_back(std::make_pair(u,v));
        }
    std::sort(edges.begin(), edges.end(), RipsGenerator::ComparePair(distances));
    std::cout << "Total participating edges: " << edges.size() << std::endl;
    for (EdgeVector::const_iterator cur = edges.begin(); cur != edges.end(); ++cur)
        rDebug("  (%d, %d) %f", cur->first, cur->second, distances(cur->first, cur->second));

    // Construct zigzag
    Complex             complex;
    Zigzag              zz;
    RipsGenerator       rips(distances);
    SimplexEvaluator    size(distances);
    
    // Insert vertices (initial complex is just a disjoint union of vertices)
    for (unsigned i = 0; i != vertices.size(); ++i)
    {
        // Add a vertex
        Smplx sv; sv.add(vertices[i]);
        rDebug("Adding %s", tostring(sv).c_str());
        add.start();
        complex.insert(std::make_pair(sv, 
                                      zz.add(Boundary(), 
                                             BirthInfo(0, 0)).first));
        add.stop();
        //rDebug("Newly born cycle order: %d", complex[sv]->low->order);
        CountNum(cComplexSize, 0);
        Count(cComplexSize);
        Count(cOperations);
    }

    rInfo("Commencing computation");
    // std::cerr << std::setprecision(15);
    bool erased[vertices.size()];
    for (unsigned i = 0; i<vertices.size(); i++)
      erased[i] = false;
    boost::progress_display show_progress(vertices.size());
    unsigned ce     = 0;        // index of the current one past last edge in the complex
    for (unsigned stage = 0; stage != vertices.size() - 1; ++stage)
    {
        unsigned i = vertices.size() - 1 - stage;

        /* Increase epsilon */
        // Record the cofaces of all the simplices that need to be removed and reinserted
        SimplexSet cofaces, large_cofaces, tmp_cofaces;
        rDebug("  Cofaces size: %d", cofaces.size());
	// std::cerr << "Cofaces sizes: " << cofaces.size() << " "
	// 	 << large_cofaces.size() << " "
	// 	 << tmp_cofaces.size() << std::endl;

        // Add anything else that needs to be inserted into the complex
	unsigned cee = ce;
	bool ce_set = false;

	// if (stage > 0) {
	//   std::cerr << "Stage " << stage << " :";
	//   Vertex u,v;
	//   boost::tie(u,v)     = edges[cee-1];
	//   std::cerr << distances(u,v) << " <= " << multiplier*epsilons[i];
	//   boost::tie(u,v)     = edges[cee];
	//   std::cerr << " < " << distances(u,v) << " <= "
	// 	    << epsilons[i-1] << std::endl
	// 	    << "  vertices[i] = " << vertices[i] << std::endl
	// 	    << "  vertices[i+1] = " << vertices[i+1] << std::endl;
	// }

	// if (stage > 0)
	//   std::cerr << "Stage " << stage << " :" << std::endl;

        while (cee < edges.size())
        {
            Vertex u,v;
            boost::tie(u,v)     = edges[cee];
	    // if (stage > 0 && (u == vertices[i+1] || v == vertices[i+1]))
	    // 	std::cerr << "ATTENTION: [" << u << "," << v << "]" << std::endl;
	    // skip already erased edges (may appear since set of participating is larger than in the Morozov zigzag)

            if (distances(u,v) > eta*epsilons[i-1] && !ce_set) {
	      ce = cee;
	      ce_set = true;
	      // std::cerr << "ce = " << distances(u,v) << " > " << eta*epsilons[i-1] << std::endl;
	    }
            if (distances(u,v) > rho*epsilons[i-1]) {
	      // std::cerr << "cee = " << distances(u,v) << " > " << rho*epsilons[i-1] << std::endl;
	      break;
	    }
            rDebug("  Recording cofaces of edges[%d]=(%d, %d) with size=%f", (cee-1), u, v, distances(u,v));
            ec.start();
	    tmp_cofaces.clear();

	    // Ignore edges with already removed vertices
	    if (!erased[u] && !erased[v])
	      rips.edge_cofaces(u, v, 
				skeleton_dimension, 
				rho*epsilons[i-1], 
				make_insert_functor(tmp_cofaces),
				vertices.begin(),
				vertices.begin() + i + 1);
	    // insert computed cofaces to cofaces sets
	    cofaces.insert(tmp_cofaces.begin(), tmp_cofaces.end());
            if (distances(u,v) > eta*epsilons[i-1])
	      large_cofaces.insert(tmp_cofaces.begin(), tmp_cofaces.end());
            ec.stop();

	    ++cee;
	    if (cee == edges.size() && !ce_set)
	      ce = cee;
        }
        rDebug("  Recorded new cofaces to add");

        // Insert all the cofaces
        rDebug("  Cofaces size: %d", cofaces.size());
        for (SimplexSet::const_iterator cur = cofaces.begin(); cur != cofaces.end(); ++cur)
        {
            Index idx; Death d; Boundary b;
            rDebug("  Adding %s, its size %f", tostring(*cur).c_str(), size(*cur));
	    // std::cerr << "  Adding " << *cur << ", its size " << size(*cur) << std::endl;
            make_boundary(*cur, complex, zz, b);
            add.start();
            boost::tie(idx, d)  = zz.add(b,
                                         BirthInfo(epsilons[i-1], cur->dimension()));
            add.stop();
            //if (!d) rDebug("Newly born cycle order: %d", complex[*cur]->low->order);
            CountNum(cComplexSize, cur->dimension());
            Count(cComplexSize);
            Count(cOperations);
            AssertMsg(zz.check_consistency(), "Zigzag representation must be consistent after removing a simplex");
            complex.insert(std::make_pair(*cur, idx));
            report_death(intervals, d, epsilons[i-1], epsilons[i], skeleton_dimension);
        }
        rInfo("Increased epsilon to %f; complex size: %d", rho*epsilons[i-1], complex.size());
	// std::cerr << "Increased epsilon to " << rho*epsilons[i-1] << ", complex size " << complex.size() << std::endl;
        report_memory();
        

	/* Remove edges of length between eta*epsilons[i-1] and rho*epsilons[i-1] and their cofaces */
        for (SimplexSet::const_reverse_iterator cur = large_cofaces.rbegin(); cur != (SimplexSet::const_reverse_iterator)large_cofaces.rend(); ++cur)
        {
            rDebug("    Removing: %s", tostring(*cur).c_str());
	    // std::cerr << "  Removing " << *cur << std::endl;
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
            report_death(intervals, d, epsilons[i-1], epsilons[i], skeleton_dimension);
        }
        rInfo("Decreased epsilon to %f; complex size: %d", eta*epsilons[i-1], complex.size());
        for (Complex::const_iterator cur = complex.begin(); cur != complex.end(); ++cur)
            rDebug("  %s", tostring(cur->first).c_str());
        report_memory();
	// std::cerr << "Decreased epsilon to " << eta*epsilons[i-1] << ", complex size " << complex.size() << std::endl;


        /* Remove the vertex */
        cofaces.clear();
        rDebug("  Cofaces size: %d", cofaces.size());
        vc.start();
        rips.vertex_cofaces(vertices[i], 
                            skeleton_dimension, 
                            eta*epsilons[i-1], 
                            make_insert_functor(cofaces),
                            vertices.begin(),
                            vertices.begin() + i + 1);
        vc.stop();
        rDebug("  Computed cofaces of the vertex, their number: %d", cofaces.size());
        for (SimplexSet::const_reverse_iterator cur = cofaces.rbegin(); cur != (SimplexSet::const_reverse_iterator)cofaces.rend(); ++cur)
        {
            rDebug("    Removing: %s", tostring(*cur).c_str());
	    // std::cerr << " Removing " << *cur << std::endl;
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
            report_death(intervals, d, epsilons[i-1], epsilons[i], skeleton_dimension);
        }
        rInfo("Removed vertex; complex size: %d", complex.size());
        for (Complex::const_iterator cur = complex.begin(); cur != complex.end(); ++cur)
            rDebug("  %s", tostring(cur->first).c_str());
	// std::cerr << " Removed vertex " << vertices[i] << ", complex size " << complex.size() << std::endl;
	
	// Book-keep set of erased vertices
	erased[vertices[i]] = true;

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
    report_death(intervals, d, epsilons[0], epsilons[0], skeleton_dimension);
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

    std::cout << "Writing intervals...";
    // Note (hack): use epsilons[vertices.size()-2]/2 as minimal value for the log-scale intervals to avoid intervals starting at -infinity and thus a scaling effect
    write_intervals(out, intervals, skeleton_dimension, logscale, epsilons[vertices.size()-2]/2);
    std::cout << " done." << std::endl;
}


void write_intervals(std::ostream& out, const IntervalsVector& intervals, int skeleton_dimension, bool logscale, double min_value) {
  out << "I = { ";
    for (int d = 0; d<skeleton_dimension; d++) {
      out << "[ ";
      for (std::list<std::pair<double,double> >::const_iterator pit = intervals[d].begin(); pit != intervals[d].end(); pit++)
	if (logscale)
	  out << "[" << log2(std::max(pit->first, min_value)) << ";" << log2(std::max(pit->second, min_value)) << "] ";
	else
	  out << "[" << pit->first << ";" << pit->second << "] ";

      // add dummy interval if intervals list is empty
      if (intervals[d].empty())
	out << "[0;0] ";
      out << "] ,";
    }
    out << "} ";
} 
            
void        report_death(IntervalsVector& intervals, Death d, DistanceType epsilon, DistanceType birthEpsilon, Dimension skeleton_dimension)
{
  // std::cerr << "   d = " << d;
  // if (d)
  //   std::cerr << "   d->dim = " << d->dimension 
  // 	      << "   d->dist = " << d->distance;
  // std::cerr << "   epsilon = " << epsilon << std::endl;

  if (d && ((d->distance - epsilon) != 0) && (d->dimension < skeleton_dimension))
    intervals[d->dimension].push_back(std::pair<double,double>(d->distance, birthEpsilon));
}

void        make_boundary(const Smplx& s, Complex& c, const Zigzag& zz, Boundary& b)
{
    rDebug("  Boundary of <%s>", tostring(s).c_str());
    for (Smplx::BoundaryIterator cur = s.boundary_begin(); cur != s.boundary_end(); ++cur)
    {
        b.append(c[*cur], zz.cmp);
        rDebug("   %d", c[*cur]->order);
    }
}

std::ostream&   operator<<(std::ostream& out, const BirthInfo& bi)
{ return (out << bi.distance); }


void        process_command_line_options(int           argc, 
                                         char*         argv[],
                                         unsigned&     skeleton_dimension,
                                         float&        eta,
                                         float&        rho,
					 bool&         logscale,
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
        ("dim,d", po::value<unsigned>(&skeleton_dimension)->default_value(3),     "maximal dimension of the Rips complex")
        ("eta,e",        po::value<float>(&eta)->default_value(3),                "multiplier eta used in the Rips parameter")
        ("rho,r",        po::value<float>(&rho)->default_value(3),                "multiplier rho used in the Rips parameter")
      ( "logscale,l" , po::value<bool>(&logscale)->zero_tokens(), "outputs interval bounds on log_2 scale" ) ;
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


