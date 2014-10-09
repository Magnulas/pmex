#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_STATIC_PERSISTENCE_H__
#define __PYTHON_STATIC_PERSISTENCE_H__

#include <topology/static-persistence.h>

#include "filtration.h"

namespace dionysus {
namespace python   {

typedef         StaticPersistence<>             SPersistence;
typedef         SPersistence::OrderElement      SPersistenceNode;
typedef         SPersistence::OrderIndex        SPersistenceIndex;
typedef         SPersistence::SimplexMap<PythonFiltration>        
                                                SPersistenceSimplexMap;


/* Persistence */
template<class Persistence>
boost::shared_ptr<Persistence>          init_from_filtration(bp::object f)
{
    PythonFiltration& sf = bp::extract<PythonFiltration&>(f);
    boost::shared_ptr<Persistence> p(new Persistence(sf));
    return p;
}

template<class Persistence, class PersistenceIndex>
unsigned                                distance(Persistence& p, 
                                                 const PersistenceIndex& i)             { return p.iterator_to(i) - p.begin(); }

/* SPNode */
template<class PNode>
const PNode&                            pair(const PNode& n)                            { return *n.pair; }


/* PersistenceSimplexMap */
template<class PersistenceSimplexMap, class PersistenceIndex>
const SimplexVD&                        psmap_getitem(const PersistenceSimplexMap& psmap, 
                                                      const PersistenceIndex& i)        { return psmap[i]; }


} } // namespace dionysus::python

#endif
