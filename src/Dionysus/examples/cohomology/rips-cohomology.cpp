#include <topology/cohomology-persistence.h>
#include <topology/rips.h>

#include <utilities/containers.h>           // for BackInsertFunctor

#include <map>
#include <iostream>

#include <boost/tuple/tuple.hpp>
    
// Trivial example of size() points on a line with integer coordinates
struct Distances
{
    typedef         int             IndexType;
    typedef         double          DistanceType;

    DistanceType    operator()(IndexType a, IndexType b) const      { return std::abs(a - b); }

    size_t          size() const                                    { return 2000; }
    IndexType       begin() const                                   { return 0; }
    IndexType       end() const                                     { return size(); }
};

typedef     CohomologyPersistence<Distances::DistanceType>          Persistence;
typedef     Persistence::SimplexIndex                               Index;
typedef     Persistence::Death                                      Death;
typedef     Persistence::CocyclePtr                                 CocyclePtr;

typedef     Rips<Distances>                                         Generator;
typedef     Generator::Simplex                                      Smplx;

typedef     std::map<Smplx, Index, 
                     Smplx::VertexComparison>                       Complex;
typedef     std::vector<Smplx>                                      SimplexVector;

int main()
{
    Distances               distances;
    Generator               rips(distances);
    Generator::Evaluator    size(distances);
    SimplexVector           v;
    Complex                 c;
    
    rips.generate(2, 50, make_push_back_functor(v));
    std::sort(v.begin(), v.end(), Generator::Comparison(distances));
    std::cout << "Simplex vector generated, size: " << v.size() << std::endl;

    Persistence p;
    for (SimplexVector::const_iterator cur = v.begin(); cur != v.end(); ++cur)
    {
        std::vector<Index>      boundary;
        for (Smplx::BoundaryIterator bcur  = cur->boundary_begin(); 
                                     bcur != cur->boundary_end();       ++bcur)
            boundary.push_back(c[*bcur]);
        
        Index idx; Death d; CocyclePtr ccl;
        boost::tie(idx, d, ccl)     = p.add(boundary.begin(), boundary.end(), size(*cur));
        c[*cur] = idx;
        if (d && (size(*cur) - *d) > 0)
            std::cout << (cur->dimension() - 1) << " " << *d << " " << size(*cur) << std::endl;
    }
}
