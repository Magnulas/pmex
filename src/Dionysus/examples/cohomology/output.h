#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <cassert>

bool neq(const Smplx& s1, const Smplx& s2)               
{ 
    Smplx::VertexComparison cmp;
    return cmp(s1, s2) || cmp(s2, s1);
}

template<class Comparison>
unsigned index(const SimplexVector& v, const Smplx& s, const Comparison& cmp)
{
	//iter to firt s lower than other simplice with respect to cmp
    SimplexVector::const_iterator it = std::lower_bound(v.begin(), v.end(), s, cmp);
	//Loop until s and *it is the same
    while (neq(*it, s)) ++it;
	//return the index/distance for s
    return it - v.begin();
}

template<class Comparison>
void output_boundary_matrix(std::ostream& out, const SimplexVector& v, const Comparison& cmp)
{
    unsigned i = 0;
    for (SimplexVector::const_iterator cur = v.begin(); cur != v.end(); ++cur)
    {
        // std::cout << "Simplex: " << *cur << std::endl;
        bool                sign = true;
        for (Smplx::BoundaryIterator bcur  = cur->boundary_begin(); bcur != cur->boundary_end(); ++bcur)
        {
            // std::cout << "  " << *bcur << std::endl;
	    //sign which alternates each time, "index" for boundary (byte offset?), simplex index
            out << (sign ? 1 : -1) << " ";
            out << index(v, *bcur, cmp) << " " << i << "\n";
            sign = !sign;
        }
        ++i;
    }
}

void output_vertex_indices(std::ostream& out, const SimplexVector& v)
{
    unsigned i = 0;
    for (SimplexVector::const_iterator cur = v.begin(); cur != v.end(); ++cur)
    {
	//index for simplex, vertex index?
        if (cur->dimension() == 0)
            out << i << " " << cur->vertices()[0] << std::endl;
        ++i;
    }
}

void output_cocycle(std::string cocycle_prefix, unsigned i, const SimplexVector& v, const BirthInfo& birth, const Persistence::ZColumn& zcol, ZpField::Element prime)
{
    std::ostringstream istr; istr << '-' << i;
    std::string filename = cocycle_prefix + istr.str() + ".ccl";
    std::ofstream out(filename.c_str());
    out << "# Cocycle born at " << birth.get<1>() << std::endl;
    for (Persistence::ZColumn::const_iterator zcur = zcol.begin(); zcur != zcol.end(); ++zcur)
    {
        //const Smplx& s = **(zcur->si);
        out << (zcur->coefficient <= prime/2 ? zcur->coefficient : zcur->coefficient - prime) << " ";
        out << zcur->si->getValue() << "\n";
    }
}
