/*
 * Author: Dmitriy Morozov
 * Department of Computer Science, Duke University, 2005 -- 2006
 *
 * Depends on Daniel Russel's "simple C++ PDB" (aka DSR-PDB).
 */

#ifndef __PDBDISTANCE_H__
#define __PDBDISTANCE_H__

#include <fstream>
#include <string>
#include <dsrpdb/Protein.h>
#include <dsrpdb/iterator.h>
#include <cmath>

#include <boost/serialization/access.hpp>

#include "utilities/types.h"
#include "grid2D.h"

#include <boost/serialization/export.hpp>


class PDBDistanceGrid: public Grid2D
{
	public:
		PDBDistanceGrid()
		{}
	
		PDBDistanceGrid(std::istream& in, bool ca_only = true)
		{
			load_stream(in, ca_only);
		}

		void	load_stream(std::istream& in, bool ca_only = true)
		{
			dsrpdb::Protein p(in);
			typedef std::vector<dsrpdb::Point> PointVector;
			PointVector coordinates;
			if (ca_only)
			{
				PointVector v(ca_coordinates_begin(p), ca_coordinates_end(p));
				coordinates.swap(v);
			}
			else
			{
				PointVector v(backbone_coordinates_begin(p), backbone_coordinates_end(p));
				coordinates.swap(v);
			}

			std::cout << "Coordinatess created, size: " << coordinates.size() << std::endl;

			Grid2D::change_dimensions(coordinates.size(), coordinates.size());
			for (Grid2D::CoordinateIndex i = 0; i < coordinates.size(); ++i)
				for (Grid2D::CoordinateIndex j = 0; j < coordinates.size(); ++j)
				{
					if (i < j)
						Grid2D::operator()(i,j) = distance(coordinates[i], coordinates[j]);
					else
						Grid2D::operator()(i,j) = 0;
				}
		}

	private:
		Grid2D::ValueType distance(dsrpdb::Point p1, dsrpdb::Point p2) const
		{
			dsrpdb::Vector v = p1 - p2;
			return std::sqrt(v*v);
		}

#if 0
	private:
		// Serialization
		friend class boost::serialization::access;
		
		template<class Archive>
		void serialize(Archive& ar, version_type version)
		{
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Grid2D);
		}
#endif
};

//BOOST_CLASS_EXPORT(PDBDistanceGrid)

#endif // __PDBDISTANCE_H__
