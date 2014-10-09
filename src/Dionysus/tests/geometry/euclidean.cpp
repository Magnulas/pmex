#include "geometry/euclidean.h"
#include <vector>
#include <iostream>
#include <cmath>

typedef Kernel<double>				K;
typedef K::Point					Point;
typedef K::Sphere					Sphere;
typedef K::PointContainer			PointContainer;
typedef	K::MatrixType				MatrixType;

int main()
{
	K k(3);
	std::vector<Point> points(6, k.origin());
	points[0][0] = 0; points[0][1] = 0; points[0][2] = 0;
	points[1][0] = 0; points[1][1] = 2; points[1][2] = 0;
	points[2][0] = 0; points[2][1] = 0; points[2][2] = 5;
	points[3][0] = 1; points[3][1] = 1; points[3][2] = 1;
	points[4][0] = 0; points[4][1] = 1; points[4][2] = 0;
	points[5][0] = 1; points[5][1] = 0; points[5][2] = 0;


	// Edges
	{
		PointContainer vertices(2);
		vertices[0] = &points[0]; vertices[1] = &points[2];
		std::cout << "{0, 2}:" << std::endl;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		std::cout << std::endl;

		vertices[0] = &points[0]; vertices[1] = &points[3];
		std::cout << "{0, 3}:" << std::endl;
		s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		std::cout << std::endl;
	}

	// Triangles
	{
		PointContainer vertices(3);
		vertices[0] = &points[0]; vertices[1] = &points[3]; vertices[2] = &points[1];
		std::cout << "{0, 3, 1}:" << std::endl;;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		std::cout << std::endl;
		
		vertices[0] = &points[0]; vertices[1] = &points[4]; vertices[2] = &points[5];
		std::cout << "{0, 4, 5}:" << std::endl;
		s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		std::cout << std::endl;
	}

	// Tetrahedron
	{
		PointContainer vertices(4);
		vertices[0] = &points[3]; vertices[1] = &points[1]; vertices[2] = &points[2]; vertices[3] = &points[0];
		std::cout << "{3, 1, 2, 0}:" << std::endl;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		
		std::cout << s.center().squared_distance(points[0]) << std::endl;
		std::cout << std::endl;
	}
	
	{
		PointContainer vertices(3);
		vertices[0] = &points[3]; vertices[1] = &points[1]; vertices[2] = &points[2];
		std::cout << "{3, 1, 2}:" << std::endl;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, points[0]) << std::endl;

		std::cout << "Distance: " << points[0].squared_distance(s.center()) << std::endl;
		
		std::cout << s.center().squared_distance(points[0]) << std::endl;
		std::cout << std::endl;
	}
}
