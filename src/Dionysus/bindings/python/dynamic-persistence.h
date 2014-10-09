#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_DYNAMIC_PERSISTENCE_CHAINS_H__
#define __PYTHON_DYNAMIC_PERSISTENCE_CHAINS_H__

#include <topology/dynamic-persistence.h>
#include "static-persistence.h"

namespace dionysus {
namespace python   {

typedef         DynamicPersistenceChains<>          DPersistenceChains;
typedef         DPersistenceChains::OrderElement    DPersistenceChainsNode;
typedef         DPersistenceChains::OrderIndex      DPersistenceChainsIndex;
typedef         DPersistenceChains::SimplexMap<PythonFiltration>        
                                                    DPersistenceChainsSimplexMap;
} } // namespace dionysus::python

#endif
