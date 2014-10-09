#include <topology/simplex.h>
#include <topology/cohomology-persistence.h>

#include <utilities/log.h>
#include <utilities/indirect.h>

#include <vector>
#include <map>
#include <iostream>

#include <boost/tuple/tuple.hpp>

#if 1
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#endif

typedef         CohomologyPersistence<unsigned>     Persistence;
typedef         Persistence::SimplexIndex           Index;
typedef         Persistence::Death                  Death;
typedef         Persistence::CocyclePtr             CocyclePtr;

typedef         unsigned                            Vertex;
typedef         Simplex<Vertex, double>             Smplx;

typedef         std::map<Smplx, Index, 
                         Smplx::VertexComparison>   Complex;
typedef         std::vector<Smplx>                  SimplexVector;


void fillTriangleSimplices(SimplexVector& c)
{
    typedef std::vector<Vertex> VertexVector;
    VertexVector vertices(4);
    vertices[0] = 0; vertices[1] = 1; vertices[2] = 2; 
    vertices[3] = 0;
        
    VertexVector::const_iterator bg = vertices.begin();
    VertexVector::const_iterator end = vertices.end();
    c.push_back(Smplx(bg,     bg + 1, 0));                 // 0 = A
    c.push_back(Smplx(bg + 1, bg + 2, 1));                 // 1 = B
    c.push_back(Smplx(bg + 2, bg + 3, 2));                 // 2 = C
    c.push_back(Smplx(bg,     bg + 2, 2.5));               // AB
    c.push_back(Smplx(bg + 1, bg + 3, 2.9));               // BC
    c.push_back(Smplx(bg + 2, end,    3.5));               // CA
    c.push_back(Smplx(bg,     bg + 3, 5));                 // ABC
}

int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stderrLog.subscribeTo(RLOG_CHANNEL("info"));
    stderrLog.subscribeTo(RLOG_CHANNEL("error"));
    stderrLog.subscribeTo(RLOG_CHANNEL("topology"));
#endif

    SimplexVector v;
    fillTriangleSimplices(v);
    std::sort(v.begin(), v.end(), Smplx::DataComparison());
    std::cout << "Simplices filled" << std::endl;
    for (SimplexVector::const_iterator cur = v.begin(); cur != v.end(); ++cur)
        std::cout << "  " << *cur << std::endl;

    // Compute persistence
    Complex         c;
    ZpField         zp(11);
    Persistence     p(zp);
    unsigned i = 0;
    for (SimplexVector::const_iterator cur = v.begin(); cur != v.end(); ++cur)
    {
        std::cout << "-------" << std::endl;

        std::vector<Index>      boundary;
        for (Smplx::BoundaryIterator bcur  = cur->boundary_begin(); 
                                     bcur != cur->boundary_end();       ++bcur)
            boundary.push_back(c[*bcur]);
        
        Index idx; Death d; CocyclePtr ccl;
        boost::tie(idx, d, ccl)     = p.add(boundary.begin(), boundary.end(), i++);
        c[*cur] = idx;
        if (d)
            std::cout << (cur->dimension() - 1) << " " << *d << " " << (i-1) << std::endl;
            // the dimension above is adjusted for what it would look like in homology
            // (i.e. when a 1 class kills 0, it's really that in cohomology forward 0 kills 1,
            //  in cohomology backward 1 kills 0, and in homology 1 kills 0)

        p.show_cocycles();
    }
}

