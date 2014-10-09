/*
 * Author: Dmitriy Morozov
 * Department of Computer Science, Duke University, 2005 -- 2006
 */

#ifndef __GRID2D_H__
#define __GRID2D_H__

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <iostream>
//#include <cmath>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

#include <boost/iterator/counting_iterator.hpp>

#include "utilities/types.h"
#include "topology/simplex.h"

/** 
 * Grid2D stores a grid
 */
class Grid2D
{
    public:
        typedef                 RealType                                        ValueType;
        typedef                 ValueType                                       result_type;
        
        typedef                 unsigned int                                    CoordinateIndex;
        typedef                 boost::counting_iterator<CoordinateIndex>       iterator;

        typedef                 Simplex<CoordinateIndex>                        Smplx;
        typedef                 std::vector<ValueType>                          ValueVector;

    public:
        Grid2D(CoordinateIndex xx = 1, CoordinateIndex yy = 1);

        /// Sets the grid dimensions to (xx,yy)
        void                    change_dimensions(CoordinateIndex xx, CoordinateIndex yy);

        ValueType&              operator()(CoordinateIndex i, CoordinateIndex j)            { return data[i*x + j]; }
        const ValueType&        operator()(CoordinateIndex i, CoordinateIndex j) const      { return data[i*x + j]; }
        ValueType&              operator()(CoordinateIndex i)                               { return data[i]; }
        const ValueType&        operator()(CoordinateIndex i) const                         { return data[i]; }

        CoordinateIndex         xsize() const                                               { return x; }
        CoordinateIndex         ysize() const                                               { return y; }
        CoordinateIndex         size() const                                                { return x*y; }

        iterator                begin() const                                               { return iterator(0); }
        iterator                end() const                                                 { return iterator(size()); }
        
        /* Given a sequential index of an element return its coordinates */
        CoordinateIndex         xpos(CoordinateIndex i) const                               { return i / x; }
        CoordinateIndex         ypos(CoordinateIndex i) const                               { return i % x; }
        CoordinateIndex         seq(CoordinateIndex i, CoordinateIndex j) const;

        template<class Functor>
        void                    complex_generator(const Functor& f);

        std::ostream&           operator<<(std::ostream& out) const;

        static const CoordinateIndex INVALID_INDEX = -1;

    private:
        CoordinateIndex         x,y;
        ValueVector             data;

#if 0
    private:
        // Serialization
        friend class boost::serialization::access;

        template<class Archive> void save(Archive& ar, version_type ) const;
        template<class Archive> void load(Archive& ar, version_type );

        BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};
//BOOST_CLASS_EXPORT(Grid2D)
    

std::ostream& operator<<(std::ostream& out, const Grid2D& grid)                             { return grid.operator<<(out); }

        
#include "grid2D.hpp"

#endif // __GRID2D_H__
