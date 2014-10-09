#include <utilities/log.h>
#include <boost/foreach.hpp>

AlphaSimplex2D::
AlphaSimplex2D(const Delaunay2D::Vertex& v): alpha_(0), attached_(false)
{
    for (int i = 0; i < 3; ++i)
        if (v.face()->vertex(i) != Vertex_handle() && v.face()->vertex(i)->point() == v.point())
            Parent::add(v.face()->vertex(i));
}

AlphaSimplex2D::
AlphaSimplex2D(const Delaunay2D::Edge& e): attached_(false)
{
    Face_handle f = e.first;
    for (int i = 0; i < 3; ++i)
        if (i != e.second)
            Parent::add(f->vertex(i));
}

AlphaSimplex2D::
AlphaSimplex2D(const Delaunay2D::Edge& e, const SimplexSet& simplices, const Delaunay2D& Dt): attached_(false)
{
    Face_handle f = e.first;
    for (int i = 0; i < 3; ++i)
        if (i != e.second)
            Parent::add(f->vertex(i));

    VertexSet::const_iterator v = static_cast<const Parent*>(this)->vertices().begin();
    const Point& p1 = (*v++)->point();
    const Point& p2 = (*v)->point();

    Face_handle o = f->neighbor(e.second);
    if (o == Face_handle())
    {
        alpha_ = CGAL::squared_radius(p1, p2);
        return;
    }
    int oi = o->index(f);

    attached_ = false;
    if (!Dt.is_infinite(f->vertex(e.second)) &&
        CGAL::side_of_bounded_circle(p1, p2,
                                     f->vertex(e.second)->point()) == CGAL::ON_BOUNDED_SIDE)
        attached_ = true;
    else if (!Dt.is_infinite(o->vertex(oi)) &&
             CGAL::side_of_bounded_circle(p1, p2,
                                          o->vertex(oi)->point()) == CGAL::ON_BOUNDED_SIDE)
        attached_ = true;
    else
        alpha_ = CGAL::squared_radius(p1, p2);

    if (attached_)
    {
        if (Dt.is_infinite(f))
            alpha_ = simplices.find(AlphaSimplex2D(*o))->alpha();
        else if (Dt.is_infinite(o))
            alpha_ = simplices.find(AlphaSimplex2D(*f))->alpha();
        else
            alpha_ = std::min(simplices.find(AlphaSimplex2D(*f))->alpha(),
                              simplices.find(AlphaSimplex2D(*o))->alpha());
    }
}

AlphaSimplex2D::
AlphaSimplex2D(const Delaunay2D::Face& f): attached_(false)
{
    for (int i = 0; i < 3; ++i)
        Parent::add(f.vertex(i));
    VertexSet::const_iterator v = static_cast<const Parent*>(this)->vertices().begin();
    Point p1 = (*v++)->point();
    Point p2 = (*v++)->point();
    Point p3 = (*v)->point();
    alpha_ = CGAL::squared_radius(p1, p2, p3);
}


bool
AlphaSimplex2D::AlphaOrder::
operator()(const AlphaSimplex2D& first, const AlphaSimplex2D& second) const
{
    if (first.alpha() == second.alpha())
        return (first.dimension() < second.dimension());
    else
        return (first.alpha() < second.alpha());
}

std::ostream&
AlphaSimplex2D::
operator<<(std::ostream& out) const
{
    for (VertexSet::const_iterator cur = Parent::vertices().begin();
                                   cur != Parent::vertices().end(); ++cur)
        out << **cur << ", ";
    out << "value = " << value();

    return out;
}

void fill_simplex_set(const Delaunay2D& Dt, AlphaSimplex2D::SimplexSet& simplices)
{
    for(Face_iterator cur = Dt.finite_faces_begin(); cur != Dt.finite_faces_end(); ++cur)
        simplices.insert(AlphaSimplex2D(*cur));
    rInfo("Faces inserted");
    for(Edge_iterator cur = Dt.finite_edges_begin(); cur != Dt.finite_edges_end(); ++cur)
        simplices.insert(AlphaSimplex2D(*cur, simplices, Dt));
    rInfo("Edges inserted");
    for(Vertex_iterator cur = Dt.finite_vertices_begin(); cur != Dt.finite_vertices_end(); ++cur)
        simplices.insert(AlphaSimplex2D(*cur));
    rInfo("Vertices inserted");
}

template<class Filtration>
void fill_complex(const Delaunay2D& Dt, Filtration& filtration)
{
    // Compute all simplices with their alpha values and attachment information
    // TODO: this can be optimized; the new Filtration can act as a SimplexSet
    AlphaSimplex2D::SimplexSet simplices;
    fill_simplex_set(Dt, simplices);
    BOOST_FOREACH(const AlphaSimplex2D& s, simplices)
        filtration.push_back(s);
}

