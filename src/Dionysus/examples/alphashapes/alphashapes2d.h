/**
 * Author: Dmitriy Morozov
 * Department of Computer Science, Duke University, 2007
 */

#ifndef __ALPHASHAPES2D_H__
#define __ALPHASHAPES2D_H__

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <topology/simplex.h>
#include <utilities/types.h>

#include <vector>
#include <set>
#include <iostream>

struct K: CGAL::Exact_predicates_exact_constructions_kernel {};

typedef CGAL::Delaunay_triangulation_2<K>           Delaunay2D;
typedef Delaunay2D::Point                           Point;
typedef Delaunay2D::Vertex_handle                   Vertex_handle;
typedef Delaunay2D::Face_handle                     Face_handle;
typedef K::FT                                       RealValue;

typedef Delaunay2D::Finite_vertices_iterator        Vertex_iterator;
typedef Delaunay2D::Finite_edges_iterator           Edge_iterator;
typedef Delaunay2D::Finite_faces_iterator           Face_iterator;


class AlphaSimplex2D: public Simplex<Vertex_handle>
{
    public:
        typedef     Simplex<Vertex_handle>                              Parent;
        typedef     std::set<AlphaSimplex2D, Parent::VertexComparison>  SimplexSet;
        typedef     Parent::VertexContainer                             VertexSet;

    public:
                                    AlphaSimplex2D()                    {}
                                    AlphaSimplex2D(const Parent& p): 
                                            Parent(p)                   {}
                                    AlphaSimplex2D(const AlphaSimplex2D& s): 
                                            Parent(s)                   { attached_ = s.attached_; alpha_ = s.alpha_; }
                                    AlphaSimplex2D(const Delaunay2D::Vertex& v);
        
                                    AlphaSimplex2D(const Delaunay2D::Edge& e);
                                    AlphaSimplex2D(const Delaunay2D::Edge& e, const SimplexSet& simplices, const Delaunay2D& Dt);
        
                                    AlphaSimplex2D(const Delaunay2D::Face& c);
        
        RealType                    value() const                       { return CGAL::to_double(alpha_); }
        RealValue                   alpha() const                       { return alpha_; }
        bool                        attached() const                    { return attached_; }

        // Ordering
        struct AlphaOrder
        { bool operator()(const AlphaSimplex2D& first, const AlphaSimplex2D& second) const; };
        
        struct AlphaValueEvaluator
        { 
            typedef                 AlphaSimplex2D                                  first_argument_type;
            typedef                 RealType                                        result_type;

            RealType                operator()(const AlphaSimplex2D& s) const       { return s.value(); }
        };

        std::ostream&               operator<<(std::ostream& out) const;
        
    private:
        RealValue                   alpha_;
        bool                        attached_;
};

typedef             std::vector<AlphaSimplex2D>                             AlphaSimplex2DVector;
void                fill_simplex_set(const Delaunay2D& Dt, AlphaSimplex2D::SimplexSet& simplices);
template<class Filtration>
void                fill_complex(const Delaunay2D& Dt,     Filtration& filtration);

std::ostream&       operator<<(std::ostream& out, const AlphaSimplex2D& s)  { return s.operator<<(out); }

#include "alphashapes2d.hpp"

#endif // __ALPHASHAPES2D_H__
