#define BOOST_PYTHON_STATIC_LIB
#include <topology/chain.h>
#include "static-persistence.h"
#include "dynamic-persistence.h"

namespace dionysus { 
namespace python   {

typedef     SPersistence::Chain                     VSPChain;
typedef     DPersistenceChains::Chain               VDPChain;

} }     // namespace dionysus::python
