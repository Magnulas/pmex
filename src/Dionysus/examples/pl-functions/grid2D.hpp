#include <iostream>
#include <limits>

/* Implementations */

Grid2D::
Grid2D(CoordinateIndex xx, CoordinateIndex yy):
    x(xx), y(yy), data(x*y)
{}

void                    
Grid2D::
change_dimensions(CoordinateIndex xx, CoordinateIndex yy)
{
    x = xx; y = yy;
    data.resize(x*y);
}

Grid2D::CoordinateIndex
Grid2D::
seq(CoordinateIndex i, CoordinateIndex j) const
{ 
    // Do not forget to check if less than 0, if Index is made signed --- dangerous
    if (i >= x || j >= y)
        return INVALID_INDEX;

    return i*x + j; 
}

std::ostream&           
Grid2D::
operator<<(std::ostream& out) const
{
    for (Grid2D::CoordinateIndex i = 0; i < xsize(); ++i)
    {
        for (Grid2D::CoordinateIndex j = 0; j < ysize(); ++j)
            std::cout << operator()(i, j) << ' ';
        std::cout << std::endl;
    }
    return out; 
}   

#if 0
using boost::serialization::make_nvp;

template<class Archive>
void 
Grid2D::
save(Archive& ar, version_type ) const
{
    ar << BOOST_SERIALIZATION_NVP(x);
    ar << BOOST_SERIALIZATION_NVP(y);
    ar << make_nvp("data", data);
}

template<class Archive> 
void 
Grid2D::
load(Archive& ar, version_type )
{
    ar >> make_nvp("x", x);
    ar >> make_nvp("y", y);
    ar >> make_nvp("data", data);
}
#endif

template<class Functor>
void    
Grid2D::
complex_generator(const Functor& f)
{
    for (CoordinateIndex x = 0; x < xsize() - 1; ++x)
        for (CoordinateIndex y = 0; y < ysize() - 1; ++y)
        {
            CoordinateIndex v(seq(x,y));
            CoordinateIndex vh(seq(x+1,y));
            CoordinateIndex vv(seq(x,y+1));
            CoordinateIndex vd(seq(x+1,y+1));

            Smplx sh; sh.add(v);  
            f(sh);
            sh.add(vh); f(sh);              // Horizontal edge
            sh.add(vd); f(sh);              // "Horizontal" triangle
            
            Smplx sv; sv.add(v);
            sv.add(vv); f(sv);              // Vertical edge
            sv.add(vd); f(sv);              // "Vertical" triangle
            
            Smplx sd; sd.add(v);
            sd.add(vd); f(sd);              // Diagonal edge

            if (y == ysize() - 2)
            {
                Smplx s; s.add(vv);
                s.add(vd); f(s);            // Top edge
            }
            if (x == xsize() - 2)
            {
                Smplx s; s.add(vh);
                s.add(vd); f(s);            // Right edge
            }
        }
    
    // Last row
    for (CoordinateIndex x = 0; x < xsize(); ++x)
    {
        std::cout << x << " " << ysize() - 1 << " " << seq(x, ysize() - 1) << std::endl;
        CoordinateIndex v(seq(x, ysize() - 1));
        Smplx s; s.add(v);
        f(s);
    }

    // Last column
    for (CoordinateIndex y = 0; y < ysize() - 1; ++y)
    {
        std::cout << xsize() - 1 << " " << y << " " << seq(xsize() - 1, y) << std::endl;
        CoordinateIndex v(seq(xsize() - 1, y));
        Smplx s; s.add(v);
        f(s);
    }
}
