#include <utilities/log.h>

#if LOGGING
static rlog::RLogChannel* rlARSimplex3D =                       DEF_CHANNEL("ar/simplex3d", rlog::Log_Debug);
#endif

ARSimplex3D::	    
ARSimplex3D(const ARSimplex3D& s): Parent(s)
{ 
	attached_ = s.attached_; 
	alpha_ = s.alpha_; 
	rho_ = s.rho_;
	s_ = s.s_;
	v_ = s.v_;
	phi_const_ = s.phi_const_;
}

ARSimplex3D::
ARSimplex3D(const Parent::Vertex& v)
{
	Parent::add(v);
}

ARSimplex3D::	    
ARSimplex3D(const ::Vertex& v, const Point& z): alpha_(0), rho_(0), v_(0), attached_(false)
{
	for (int i = 0; i < 4; ++i)
		if (v.cell()->vertex(i)->point() == v.point())
			Parent::add(v.cell()->vertex(i));
	s_ = CGAL::squared_distance((*Parent::vertices().begin())->point(), z);

	// phi_const_ will be set by an edge
}

ARSimplex3D::	    
ARSimplex3D(const Edge& e)
{
    Cell_handle c = e.first;
	Parent::add(c->vertex(e.second));
	Parent::add(c->vertex(e.third));
}

ARSimplex3D::	    
ARSimplex3D(const Edge& e, const Point& z, SimplexPhiMap& simplices, const Delaunay& Dt, Facet_circulator facet_bg)
{
    Cell_handle c = e.first;
	Parent::add(c->vertex(e.second));
	Parent::add(c->vertex(e.third));

	Facet_circulator cur = facet_bg;
	while (Dt.is_infinite(*cur))    ++cur; 
	SimplexPhiMap::const_iterator cur_iter = simplices.find(ARSimplex3D(*cur));
	RealValue min = cur_iter->first.alpha();
	RealValue phi_const_min = cur_iter->first.phi_const();
	
	VertexSet::const_iterator v2 = Parent::vertices().begin();
	VertexSet::const_iterator v1 = v2++;
	const Point& p1 = (*v1)->point();
	const Point& p2 = (*v2)->point();
	attached_ = false;
	rho_ = CGAL::squared_radius(p1, p2);

	if (facet_bg != 0) do
	{
		int i0 = (*cur).first->index(*v1);
		int i1 = (*cur).first->index(*v2);
		int i = 6 - i0 - i1 - (*cur).second;

		if (Dt.is_infinite(cur->first->vertex(i)))
		{
			++cur; continue;
			// FIXME: what do we do with infinite cofaces (i.e., what
			// phi_const does a simplex get if its dual Voronoi cell is
			// infinite?
		}
		
		Point p3 = (*cur).first->vertex(i)->point();
		if (CGAL::side_of_bounded_sphere(p1, p2, p3) == CGAL::ON_BOUNDED_SIDE)
			attached_ = true;
		
	    SimplexPhiMap::const_iterator cur_iter = simplices.find(ARSimplex3D(*cur));
		RealValue 								val 				= cur_iter->first.alpha();
		if (val < min) 							min 				= val;
		RealValue 								phi_const_val 		= cur_iter->first.phi_const();
		if (phi_const_val < phi_const_min) 		phi_const_min 		= phi_const_val;
				
		++cur;
	} while (cur != facet_bg);

	if (attached_)
		alpha_ = min;
	else
		alpha_ = rho_;
	phi_const_ = phi_const_min;
	
	// update phi_const_ for v1 and v2 if necessary
	SimplexPhiMap::iterator v1_iter = simplices.find(ARSimplex3D(*v1));
	SimplexPhiMap::iterator v2_iter = simplices.find(ARSimplex3D(*v2));
	if (phi_const_ < v1_iter->second) v1_iter->second = phi_const_;
	if (phi_const_ < v2_iter->second) v2_iter->second = phi_const_;

	
	s_ = CGAL::squared_distance(z, K::Segment_3(p1,p2).supporting_line());
	Point origin(0,0,0);
	Point cc = origin + ((p1 - origin) + (p2 - origin))/2;		// CGAL is funny
	v_ = CGAL::squared_distance(z, cc) - s_;
}

ARSimplex3D::	    
ARSimplex3D(const Facet& f)
{
    Cell_handle c = f.first;
	for (int i = 0; i < 4; ++i)
		if (i != f.second)
			Parent::add(c->vertex(i));
}

ARSimplex3D::	    
ARSimplex3D(const Facet& f, const Point& z, const SimplexPhiMap& simplices, const Delaunay& Dt)
{
    Cell_handle c = f.first;
	for (int i = 0; i < 4; ++i)
		if (i != f.second)
			Parent::add(c->vertex(i));

	Cell_handle o = c->neighbor(f.second);
	int oi = o->index(c);

	VertexSet::const_iterator v3 = Parent::vertices().begin();
	VertexSet::const_iterator v1 = v3++;
	VertexSet::const_iterator v2 = v3++;
	const Point& p1 = (*v1)->point();
	const Point& p2 = (*v2)->point();
	const Point& p3 = (*v3)->point();
	rho_ = squared_radius(p1, p2, p3);
	
	attached_ = false;
	if (!Dt.is_infinite(c->vertex(f.second)) &&
        CGAL::side_of_bounded_sphere(p1, p2, p3,
									 c->vertex(f.second)->point()) == CGAL::ON_BOUNDED_SIDE)
		attached_ = true;
	else if (!Dt.is_infinite(o->vertex(oi)) &&
             CGAL::side_of_bounded_sphere(p1, p2, p3,
										  o->vertex(oi)->point()) == CGAL::ON_BOUNDED_SIDE)
		attached_ = true;
	else
		alpha_ = rho_;
	
	if (Dt.is_infinite(c))
	{
	    SimplexPhiMap::const_iterator o_iter = simplices.find(ARSimplex3D(*o,z));
		if (attached_) alpha_ = o_iter->first.alpha();
		phi_const_ = o_iter->first.phi_const();					// FIXME: it's probably the other way around
	}
	else if (Dt.is_infinite(o))
	{
	    SimplexPhiMap::const_iterator c_iter = simplices.find(ARSimplex3D(*c,z));
		if (attached_) alpha_ = c_iter->first.alpha();
		phi_const_ = c_iter->first.phi_const();					// FIXME: it's probably the other way around
	}
	else
	{
	    SimplexPhiMap::const_iterator o_iter = simplices.find(ARSimplex3D(*o,z));
	    SimplexPhiMap::const_iterator c_iter = simplices.find(ARSimplex3D(*c,z));
		if (attached_) alpha_ = std::min(c_iter->first.alpha(), o_iter->first.alpha());
		phi_const_ = std::min(c_iter->first.phi_const(), o_iter->first.phi_const());
	}

	Point cc = CGAL::circumcenter(p1, p2, p3);
	v_ = CGAL::squared_distance(z, K::Plane_3(p1,p2,p3));
	s_ = CGAL::squared_distance(z, cc) - v_;
}

ARSimplex3D::	    
ARSimplex3D(const Cell& c, const Point& z): attached_(false)
{
	for (int i = 0; i < 4; ++i)
		Parent::add(c.vertex(i));
	VertexSet::const_iterator v = Parent::vertices().begin();
	Point p1 = (*v++)->point();
	Point p2 = (*v++)->point();
	Point p3 = (*v++)->point();
	Point p4 = (*v)->point();
	rho_ = alpha_ = CGAL::squared_radius(p1, p2, p3, p4);

	s_ = 0;
	v_ = CGAL::squared_distance(z, CGAL::circumcenter(p1, p2, p3, p4));

	phi_const_ = rho_ - v_;
}

ARSimplex3D::Cycle
ARSimplex3D::boundary() const
{
	Cycle bdry;
	Parent::Cycle pbdry = Parent::boundary();
	for (Parent::Cycle::const_iterator cur = pbdry.begin(); cur != pbdry.end(); ++cur)
		bdry.push_back(*cur);
	return bdry;
}


bool 
ARSimplex3D::AlphaOrder::
operator()(const ARSimplex3D& first, const ARSimplex3D& second) const
{
	if (first.alpha() == second.alpha())
		return (first.dimension() < second.dimension());
	else
		return (first.alpha() < second.alpha()); 
}

std::ostream& 
ARSimplex3D::
operator<<(std::ostream& out) const
{
    out << this << ": ";
	for (VertexSet::const_iterator cur = Parent::vertices().begin(); cur != Parent::vertices().end(); ++cur)
		out << &(**cur) << ", ";
	out << "value = " << value();

	return out;
}


void update_simplex_phi_map(const ARSimplex3D& s, ARSimplex3D::SimplexPhiMap& simplices)
{
	simplices[s] = s.phi_const();
}
		
void fill_alpha_order(const Delaunay& Dt, const Point& z, ARSimplex3DVector& alpha_order)
{
	ARSimplex3D::SimplexPhiMap simplices;
	for(Cell_iterator cur = Dt.finite_cells_begin(); cur != Dt.finite_cells_end(); ++cur)
		update_simplex_phi_map(ARSimplex3D(*cur, z), simplices);
	rLog(rlARSimplex3D, "Cells inserted");
	for(Vertex_iterator cur = Dt.finite_vertices_begin(); cur != Dt.finite_vertices_end(); ++cur)
		simplices[ARSimplex3D(*cur, z)] = 0;			// only one tetrahedron can have non-negative phi_const value 
														// (namely the one containing z); all other simplices will have a 
														// negative phi_const value, so 0 is safe
	rLog(rlARSimplex3D, "Vertices inserted");

	for(Facet_iterator cur = Dt.finite_facets_begin(); cur != Dt.finite_facets_end(); ++cur)
		update_simplex_phi_map(ARSimplex3D(*cur, z, simplices, Dt), simplices);
	rLog(rlARSimplex3D, "Facets inserted");
	for(Edge_iterator cur = Dt.finite_edges_begin(); cur != Dt.finite_edges_end(); ++cur)
		update_simplex_phi_map(ARSimplex3D(*cur, z, simplices, Dt, Dt.incident_facets(*cur)), simplices);
	rLog(rlARSimplex3D, "Edges inserted");
    
	// Sort simplices by their alpha values
	alpha_order.resize(simplices.size()); ARSimplex3DVector::iterator out = alpha_order.begin();
	for (ARSimplex3D::SimplexPhiMap::const_iterator in = simplices.begin(); in != simplices.end(); ++in, ++out)
		*out = in->first;
	std::sort(alpha_order.begin(), alpha_order.end(), ARSimplex3D::AlphaOrder());
	
	// Update phi_const for vertices
	for (ARSimplex3DVector::iterator cur = alpha_order.begin(); cur != alpha_order.end(); ++cur)
		if (cur->dimension() == 0) cur->set_phi_const(simplices[*cur]);
}

