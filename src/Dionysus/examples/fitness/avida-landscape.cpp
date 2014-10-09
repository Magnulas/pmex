#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "avida-population-detail.h"

#include <boost/program_options.hpp>

#include <topology/lowerstarfiltration.h>


// Lower-star filtration
typedef         AvidaPopulationDetail::OrganismIndex                OrganismIndex;
struct          OrganismVertexType;
typedef         std::vector<OrganismVertexType>                     VertexVector;
typedef         VertexVector::iterator                              VertexIndex;
typedef         LowerStarFiltration<VertexIndex>                    LSFiltration;
typedef         LSFiltration::Simplex                               Simplex;

struct          OrganismVertexType: public LSFiltration::VertexType<OrganismIndex> 
{
    typedef     LSFiltration::VertexType<OrganismIndex>             Parent;
                OrganismVertexType(OrganismIndex i): Parent(i)      {}
};

struct          OrganismVertexComparison
{
    public:
        bool    operator()(VertexIndex i, VertexIndex j) const      
        { return i->index()->fitness() > j->index()->fitness(); }       
        // > because of -fitness, so that maxima turn into minima
};

typedef         LSFiltration::Vineyard                              LSVineyard;
class           StaticEvaluator: public LSVineyard::Evaluator
{
    public:
                StaticEvaluator(float max_fitness): 
                    max_fitness_(max_fitness)                       {}

        virtual RealType        
                value(const Simplex& s) const       
        { return s.get_attachment()->index()->fitness()/max_fitness_; }

    private:
        float   max_fitness_;
};

std::ostream& operator<<(std::ostream& out, VertexIndex i)
{ return (out << (i->index())); }


// Distance filtration
typedef         SimplexWithValue<VertexIndex>                       DistanceSimplex;
typedef         std::vector<DistanceSimplex>                        DistanceSimplexVector;
typedef         Filtration<DistanceSimplex>                         DistanceSimplexFiltration;


namespace po = boost::program_options;

int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);
    stderrLog.subscribeTo(RLOG_CHANNEL("error"));
    //stdoutLog.subscribeTo(RLOG_CHANNEL("info"));
#endif

    typedef     AvidaOrganismDetail::DistanceType       DistanceType;

    DistanceType connected_distance;
    bool connect_mst = false;
    std::string population_input_fn;

    // Parse program options
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file",  po::value<std::string>(&population_input_fn),
                        "Avida population file");

    po::options_description visible("Allowed options");
    visible.add_options()
        ("help,h",      "produce help message")
        ("distance,d",  po::value<DistanceType>(&connected_distance)->default_value(0), 
                        "set connected distance")
        ("mst,m",       "connect minimum spanning tree");
    po::positional_options_description p;
    p.add("input-file", 1);
    
    po::options_description all; all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                  options(all).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("mst"))            { connect_mst = true; }
    if (vm.count("help") || !vm.count("input-file"))
    { 
        std::cout << "Usage: " << argv[0] << " [options] POPULATION" << std::endl;
        std::cout << visible << std::endl; 
        return 1; 
    }

    // Read organisms
    AvidaPopulationDetail population(population_input_fn);
    const AvidaPopulationDetail::OrganismVector& organisms = population.get_organisms();

    rInfo("Number of organisms: %d", organisms.size());
    float max_fitness = organisms[0].fitness();
    for (int i = 0; i < population.get_organisms().size(); ++i)
    {
        max_fitness = std::max(max_fitness, organisms[i].fitness());
        rInfo("%d (%s) %f %d %d", organisms[i].id(),
                                  organisms[i].genome().c_str(),
                                  organisms[i].fitness(),
                                  organisms[i].length(),
                                  organisms[i].genome().size());
    }

    // Order vertices 
    StaticEvaluator     evaluator(max_fitness);
    LSVineyard          vineyard(&evaluator);
    VertexVector        vertices;
    for (OrganismIndex cur = organisms.begin(); cur != organisms.end(); ++cur)  vertices.push_back(cur);
    LSFiltration        fitness_filtration(vertices.begin(), vertices.end(), OrganismVertexComparison(), &vineyard);

    // Compute MST and insert its edges if asked
    if (connect_mst)
    {
        DistanceSimplexFiltration filtration;
        {   // Scope so that simplices is deleted once it's not needed
            // Distance function filtration
            DistanceSimplexVector simplices;
        
            // Insert edges
            for (VertexIndex i = vertices.begin(); i != vertices.end(); ++i)
            {
                simplices.push_back(DistanceSimplex());
                simplices.back().add(i);
        
                for (VertexIndex j = boost::next(i); j != vertices.end(); ++j)
                {
                    simplices.push_back(DistanceSimplex(i->index()->genome_distance(*(j->index()))));
                    simplices.back().add(i);
                    simplices.back().add(j);
                }
            }
            std::sort(simplices.begin(), simplices.end(), DimensionValueComparison<DistanceSimplex>());
        
            for (DistanceSimplexVector::const_iterator  cur = simplices.begin(); 
                                                        cur != simplices.end(); ++cur)
                filtration.append(*cur);
        }

        filtration.fill_simplex_index_map();
        filtration.pair_simplices(false);            // pair simplices without storing trails

        for (DistanceSimplexFiltration::Index i = filtration.begin(); i != filtration.end(); ++i)
        {
            if (i->is_paired() && !i->sign())
            {
                Simplex s(*i);
                if (i->get_value() > connected_distance)    // <= will be connected below
                    fitness_filtration.append(s);
            }
        }
    }

    // Add simplices
    for (VertexIndex cur = vertices.begin(); cur != vertices.end(); ++cur)
        for (VertexIndex link = boost::next(cur); link != vertices.end(); ++link)
            if (cur->index()->genome_distance(*(link->index())) <= connected_distance)
            {
                Simplex s(2, cur); s.add(link);
                fitness_filtration.append(s);
            }
    rInfo("Number of simplices: %d", fitness_filtration.size());

    // Pair simplices
    fitness_filtration.fill_simplex_index_map();
    fitness_filtration.pair_simplices(false);            // pair simplices without storing trails

    //std::cout << "Outputting persistence pairs" << std::endl;
    for (LSFiltration::Index i = fitness_filtration.begin(); i != fitness_filtration.end(); ++i)
    {
        if (i->is_paired())
        {
            if (i->sign())
            {
                AssertMsg(i->dimension() == 0, "Expecting only 0-dimensional diagram");
                if (i->pair()->get_attachment() == i->vertices()[0]) continue;     // skip non-critical pairs
                std::cout << i->dimension() << " " 
                          << evaluator.value(*i) << " " 
                          << evaluator.value(*(i->pair())) << std::endl;
            }
        }
        else
        {
            if (i->dimension() != 0) continue;
            std::cout << i->dimension() << " "
                      << evaluator.value(*i) << " "
                      << 0 << std::endl;
            // The infinite pair does not make sense since we are interested in 
            // max of fitness, rather than min of -fitness. However min value for 
            // fitness is 0, so it's a natural choice of the answer.
        }
    }
}
