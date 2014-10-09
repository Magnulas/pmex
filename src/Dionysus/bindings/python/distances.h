#define BOOST_PYTHON_STATIC_LIB
#include <utilities/log.h>

#include <boost/python.hpp>
namespace bp = boost::python;

namespace dionysus { 
namespace python   {

typedef     bp::list            ListPoint;

struct ListPointL2Distance:
    public std::binary_function<bp::object, bp::object, double>
{
    result_type     operator()(bp::object p1, bp::object p2) const
    {
        ListPoint lp1 = bp::extract<ListPoint>(p1), lp2 = bp::extract<ListPoint>(p2);

        AssertMsg(bp::len(lp1) == bp::len(lp2), "Points must be in the same dimension (in L2Distance): dim1=%d, dim2=%d", bp::len(lp1), bp::len(lp2));
        result_type sum = 0;
        for (size_t i = 0; i < bp::len(lp1); ++i)
        {
            double diff = bp::extract<double>(lp1[i]) - bp::extract<double>(lp2[i]);
            sum += diff*diff;
        }

        return sqrt(sum);
    }
};

class ListPointPairwiseDistances
{
    public:
        typedef             bp::list                                        Container;
        typedef             ListPointL2Distance                             Distance;
        typedef             unsigned                                        IndexType;
        typedef             Distance::result_type                           DistanceType;


                            ListPointPairwiseDistances(Container container): 
                                container_(container)                       {}

        DistanceType        operator()(IndexType a, IndexType b) const      { return distance_(container_[a], container_[b]); }

        size_t              size() const                                    { return bp::len(container_); }
        IndexType           begin() const                                   { return 0; }
        IndexType           end() const                                     { return size(); }

    private:
        Container           container_;
        Distance            distance_;
};

} }     // namespace dionysus::python

