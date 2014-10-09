/**
 * Author: Dmitriy Morozov
 * Department of Computer Science, Duke University, 2007
 */

#ifndef __ARSIMPLEX3D_H__
#define __ARSIMPLEX3D_H__

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
//#include <CGAL/Kernel/global_functions_3.h>			// FIXME: what do we include for circumcenter?
#include <CGAL/squared_distance_3.h>

#include <topology/simplex.h>
#include <utilities/types.h>

#include <vector>
#include <set>
#include <iostream>

struct K: CGAL::Exact_predicates_exact_constructions_kernel {};

typedef CGAL::Delaunay_triangulation_3<K>    		Delaunay;
typedef Delaunay::Point                				Point;
typedef Delaunay::Vertex            				Vertex;
typedef Delaunay::Vertex_handle            			Vertex_handle;
typedef Delaunay::Edge								Edge;
typedef Delaunay::Facet								Facet;
typedef Delaunay::Cell								Cell;
typedef Delaunay::Cell_handle						Cell_handle;

typedef Delaunay::Finite_vertices_iterator    		Vertex_iterator;
typedef Delaunay::Finite_edges_iterator        		Edge_iterator;
typedef Delaunay::Finite_facets_iterator        	Facet_iterator;
typedef Delaunay::Finite_cells_iterator        		Cell_iterator;
typedef Delaunay::Facet_circulator					Facet_circulator;


class ARSimplex3D: public SimplexWithVertices<Vertex_handle>
{
	public:
        typedef     K::FT										        RealValue;
		typedef 	std::map<ARSimplex3D, RealValue>					SimplexPhiMap;
		typedef		SimplexWithVertices<Vertex_handle>					Parent;
		typedef		Parent::VertexContainer								VertexSet;
		typedef		std::list<ARSimplex3D>								Cycle;

    public:
									ARSimplex3D()						{}
									ARSimplex3D(const Parent& p): 
											Parent(p) 					{}
									ARSimplex3D(const ARSimplex3D& s);
									
									ARSimplex3D(const Parent::Vertex& v);
									ARSimplex3D(const ::Vertex& v, const Point& z);
		
								    ARSimplex3D(const Edge& e);
								    ARSimplex3D(const Edge& e, const Point& z, SimplexPhiMap& simplices, 
                                                const Delaunay& Dt, Facet_circulator facet_bg);
		
								    ARSimplex3D(const Facet& f);
								    ARSimplex3D(const Facet& f, const Point& z, const SimplexPhiMap& simplices,
                                                const Delaunay& Dt);
	    
									ARSimplex3D(const Cell& c, const Point& z);
	    
		RealType					value() const						{ return CGAL::to_double(alpha_); }
		RealValue					alpha() const						{ return alpha_; }
		RealValue					phi_const() const					{ return phi_const_; }
		RealValue					rho() const							{ return rho_; }
		RealValue					s() const							{ return s_; }
		RealValue					v() const							{ return v_; }
		
		bool						attached() const					{ return attached_; }
		Cycle						boundary() const;

		void						set_phi_const(RealValue phi_const)	{ phi_const_ = phi_const; }

		// Ordering
		struct AlphaOrder
		{ bool operator()(const ARSimplex3D& first, const ARSimplex3D& second) const; };
		
		std::ostream& 				operator<<(std::ostream& out) const;
		
	private:
		RealValue 					alpha_;		// alpha_ 	is the squared radius of the smallest _empty_ circumsphere
		RealValue					rho_;		// rho_ 	is the squared radius of the smallest circumsphere
		RealValue					s_;			// s_ 		is the squared distance from z to the affine hull of the simplex
		RealValue					v_;			// v_ 		is the squared distance from z to the affine hull of the dual Voronoi cell
		RealValue					phi_const_;	// see LHI paper, Appendices A and B
		bool 						attached_;

        // in paper's notation: s_ = v^2; v_ = d^2

};

typedef 			std::vector<ARSimplex3D>								ARSimplex3DVector;

void				update_simplex_phi_map(const ARSimplex3D& s, ARSimplex3D::SimplexPhiMap& simplices);
void 				fill_alpha_order(const Delaunay& Dt, const Point& z, ARSimplex3DVector& alpha_order);

std::ostream& 		operator<<(std::ostream& out, const ARSimplex3D& s)	{ return s.operator<<(out); }

#include "ar-simplex3d.hpp"

#endif // __ARSIMPLEX3D_H__
