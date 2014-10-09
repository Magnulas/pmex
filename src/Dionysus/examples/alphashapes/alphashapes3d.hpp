#include <utilities/log.h>
#include <boost/foreach.hpp>

AlphaSimplex3D::
AlphaSimplex3D(const Delaunay3D::Vertex& v): alpha_(0), attached_(false)
{
    for (int i = 0; i < 4; ++i)
        if (v.cell()->vertex(i)->point() == v.point())
            Parent::add(v.cell()->vertex(i));
}

AlphaSimplex3D::
AlphaSimplex3D(const Delaunay3D::Edge& e)
{
    Cell_handle c = e.first;
    Parent::add(c->vertex(e.second));
    Parent::add(c->vertex(e.third));
}

AlphaSimplex3D::
AlphaSimplex3D(const Delaunay3D::Edge& e, const SimplexSet& simplices, const Delaunay3D& Dt, Facet_circulator facet_bg)
{
    Cell_handle c = e.first;
    Parent::add(c->vertex(e.second));
    Parent::add(c->vertex(e.third));

    Facet_circulator cur = facet_bg;
    while (Dt.is_infinite(*cur))    ++cur;
    SimplexSet::const_iterator cur_iter = simplices.find(AlphaSimplex3D(*cur));
    RealValue min = cur_iter->alpha();

    const VertexSet& vertices = static_cast<const Parent*>(this)->vertices();
    VertexSet::const_iterator v = vertices.begin();
    const Point& p1 = (*v++)->point();
    const Point& p2 = (*v)->point();
    attached_ = false;

    if (facet_bg != 0) do
    {
        VertexSet::const_iterator v = vertices.begin();
        int i0 = (*cur).first->index(*v++);
        int i1 = (*cur).first->index(*v);
        int i = 6 - i0 - i1 - (*cur).second;
        if (Dt.is_infinite(cur->first->vertex(i))) { ++cur; continue; }
        Point p3 = (*cur).first->vertex(i)->point();

        cur_iter = simplices.find(AlphaSimplex3D(*cur));
        if (CGAL::side_of_bounded_sphere(p1, p2, p3) == CGAL::ON_BOUNDED_SIDE)
            attached_ = true;
        RealValue val = cur_iter->alpha();
        if (val < min)
            min = val;
        ++cur;
    } while (cur != facet_bg);

    if (attached_)
        alpha_ = min;
    else
        alpha_ = CGAL::squared_radius(p1, p2);
}

AlphaSimplex3D::
AlphaSimplex3D(const Delaunay3D::Facet& f)
{
    Cell_handle c = f.first;
    for (int i = 0; i < 4; ++i)
        if (i != f.second)
            Parent::add(c->vertex(i));
}

AlphaSimplex3D::
AlphaSimplex3D(const Delaunay3D::Facet& f, const SimplexSet& simplices, const Delaunay3D& Dt)
{
    Cell_handle c = f.first;
    for (int i = 0; i < 4; ++i)
        if (i != f.second)
            Parent::add(c->vertex(i));

    Cell_handle o = c->neighbor(f.second);
    int oi = o->index(c);

    VertexSet::const_iterator v = static_cast<const Parent*>(this)->vertices().begin();
    const Point& p1 = (*v++)->point();
    const Point& p2 = (*v++)->point();
    const Point& p3 = (*v)->point();

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
        alpha_ = CGAL::squared_radius(p1, p2, p3);

    if (attached_)
    {
        if (Dt.is_infinite(c))
            alpha_ = simplices.find(AlphaSimplex3D(*o))->alpha();
        else if (Dt.is_infinite(o))
            alpha_ = simplices.find(AlphaSimplex3D(*c))->alpha();
        else
            alpha_ = std::min(simplices.find(AlphaSimplex3D(*c))->alpha(),
                              simplices.find(AlphaSimplex3D(*o))->alpha());
    }
}

AlphaSimplex3D::
AlphaSimplex3D(const Delaunay3D::Cell& c): attached_(false)
{
    for (int i = 0; i < 4; ++i)
        Parent::add(c.vertex(i));
    VertexSet::const_iterator v = static_cast<const Parent*>(this)->vertices().begin();
    Point p1 = (*v++)->point();
    Point p2 = (*v++)->point();
    Point p3 = (*v++)->point();
    Point p4 = (*v)->point();
    alpha_ = CGAL::squared_radius(p1, p2, p3, p4);
}


bool
AlphaSimplex3D::AlphaOrder::
operator()(const AlphaSimplex3D& first, const AlphaSimplex3D& second) const
{
    if (first.alpha() == second.alpha())
        return (first.dimension() < second.dimension());
    else
        return (first.alpha() < second.alpha());
}

std::ostream&
AlphaSimplex3D::
operator<<(std::ostream& out) const
{
    for (VertexSet::const_iterator cur = Parent::vertices().begin(); cur != Parent::vertices().end(); ++cur)
        out << **cur << ", ";
    out << "value = " << value();

    return out;
}

void fill_simplex_set(const Delaunay3D& Dt, AlphaSimplex3D::SimplexSet& simplices)
{
    // Compute all simplices with their alpha values and attachment information
    for(Cell_iterator cur = Dt.finite_cells_begin(); cur != Dt.finite_cells_end(); ++cur)
        simplices.insert(AlphaSimplex3D(*cur));
    rInfo("Cells inserted");
    for(Facet_iterator cur = Dt.finite_facets_begin(); cur != Dt.finite_facets_end(); ++cur)
        simplices.insert(AlphaSimplex3D(*cur, simplices, Dt));
    rInfo("Facets inserted");
    for(Edge_iterator cur = Dt.finite_edges_begin(); cur != Dt.finite_edges_end(); ++cur)
        simplices.insert(AlphaSimplex3D(*cur, simplices, Dt, Dt.incident_facets(*cur)));
    rInfo("Edges inserted");
    for(Vertex_iterator cur = Dt.finite_vertices_begin(); cur != Dt.finite_vertices_end(); ++cur)
        simplices.insert(AlphaSimplex3D(*cur));
    rInfo("Vertices inserted");
}

template<class Filtration>
void fill_complex(const Delaunay3D& Dt, Filtration& filtration)
{
    AlphaSimplex3D::SimplexSet simplices;
    fill_simplex_set(Dt, simplices);
    BOOST_FOREACH(const AlphaSimplex3D& s, simplices)
        filtration.push_back(s);
}

