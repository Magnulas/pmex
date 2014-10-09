#define BOOST_PYTHON_STATIC_LIB
#include "birthid.h"
#include "optional.h"

namespace dp = dionysus::python;

void export_birthid()
{
    python_optional<dp::BirthID>();   
}

