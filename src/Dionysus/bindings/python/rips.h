#define BOOST_PYTHON_STATIC_LIB
#ifndef __PYTHON_RIPS_H__
#define __PYTHON_RIPS_H__

#include <topology/rips.h>
#include <utilities/indirect.h>

#include "simplex.h"

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>


namespace bp = boost::python;


namespace dionysus { 
namespace python   {

// This strange wrapper is necessary because Rips<...> stores only a const reference to the distances. 
// Something on the C++ side of things must store the actual DistancesWrapper object, and that's the 
// purpose of this class.
class RipsWithDistances
{
    public:
        class DistancesWrapper
        {
            public:
                typedef             unsigned                                        IndexType;
                typedef             double                                          DistanceType;
        
                                    DistancesWrapper(bp::object distances):
                                        distances_(distances)                       {}
        
                DistanceType        operator()(IndexType a, IndexType b) const      { return bp::extract<DistanceType>(distances_(a, b)); }
        
                IndexType           size() const                                    { return bp::len(distances_); }
                IndexType           begin() const                                   { return 0; }
                IndexType           end() const                                     { return size(); }
        
            private:
                bp::object          distances_;
        };

        typedef             DistancesWrapper::IndexType                             IndexType;
        typedef             DistancesWrapper::DistanceType                          DistanceType;

        typedef             Rips<DistancesWrapper, SimplexVD>                       RipsDS;
        typedef             RipsDS::Comparison                                      Comparison;
        typedef             RipsDS::Evaluator                                       Evaluator;

        class FunctorWrapper
        {
            public:
                                    FunctorWrapper(bp::object functor):
                                        functor_(functor)                           {}
        
                void                operator()(const RipsDS::Simplex& s) const      { functor_(s); }
        
            private:
                bp::object          functor_;
        };


                            RipsWithDistances(bp::object distances):
                                distances_(distances), rips_(distances_),
                                cmp_(Comparison(distances_)), eval_(distances_)     {}

        void                generate(Dimension k, DistanceType max, bp::object functor) const
        { rips_.generate(k, max, FunctorWrapper(functor)); }

        void                vertex_cofaces(IndexType v, Dimension k, DistanceType max, bp::object functor) const
        { rips_.vertex_cofaces(v, k, max, FunctorWrapper(functor)); }
        
        void                edge_cofaces(IndexType u, IndexType v, Dimension k, DistanceType max, bp::object functor) const
        { rips_.edge_cofaces(u, v, k, max, FunctorWrapper(functor)); }

        void                generate_candidates(Dimension k, DistanceType max, bp::object functor, bp::object seq) const
        { 
            rips_.generate(k, max, FunctorWrapper(functor), 
                           bp::stl_input_iterator<IndexType>(seq), bp::stl_input_iterator<IndexType>());
        }
        
        void                vertex_cofaces_candidate(IndexType v, Dimension k, DistanceType max, 
                                                     bp::object functor, bp::object seq) const
        { 
            rips_.vertex_cofaces(v, k, max, FunctorWrapper(functor), 
                                 bp::stl_input_iterator<IndexType>(seq), bp::stl_input_iterator<IndexType>());
        }
        
        void                edge_cofaces_candidates(IndexType u, IndexType v, Dimension k, DistanceType max, 
                                                    bp::object functor, bp::object seq) const
        { 
            rips_.edge_cofaces(u, v, k, max, FunctorWrapper(functor), 
                               bp::stl_input_iterator<IndexType>(seq), bp::stl_input_iterator<IndexType>());
        }

        int                 cmp(const SimplexObject& s1, const SimplexObject& s2) const                     { return cmp_native(s1, s2); }
        int                 cmp_native(const SimplexVD& s1, const SimplexVD& s2) const                      { return cmp_.compare(s1, s2); }
        
        DistanceType        eval(const SimplexObject& s) const                                              { return eval_native(s); }
        DistanceType        eval_native(const SimplexVD& s) const                                           { return eval_(s); }
        
    private:
        DistancesWrapper                            distances_;
        RipsDS                                      rips_;
        ThreeOutcomeCompare<Comparison>             cmp_;           // in Python, cmp is a three outcome comparison
        Evaluator                                   eval_;
};

} } // namespace dionysus::python

#endif
