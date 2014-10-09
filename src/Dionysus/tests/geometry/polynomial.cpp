#include <geometry/euclidean.h>
#include <geometry/polynomial.h>

#include <vector>
#include <iostream>
#include <cmath>

typedef UPolynomial<ZZ>						PolynomialKernel;
typedef PolynomialKernel::Polynomial		Polynomial;
typedef PolynomialKernel::Function	        RationalF;

typedef Kernel<RationalF>					K;
typedef K::Point							Point;
typedef K::Sphere							Sphere;
typedef K::PointContainer					PointContainer;
typedef	K::MatrixType						MatrixType;

int main()
{
	K k(3);
	std::vector<Point> points(7, k.origin());
	points[0][0] = Polynomial(0); 				points[0][1] = Polynomial(0); 				points[0][2] = Polynomial(0);
	points[1][0] = Polynomial(0); 				points[1][1] = Polynomial("x+2"); 			points[1][2] = Polynomial(0);
	points[2][0] = Polynomial(0); 				points[2][1] = Polynomial(0); 				points[2][2] = Polynomial("x^2 + 5");
	points[3][0] = Polynomial("x^3"); 			points[3][1] = Polynomial(1); 				points[3][2] = Polynomial("x");
	points[4][0] = Polynomial(0); 				points[4][1] = Polynomial("x^2 + 2*x + 5");	points[4][2] = Polynomial(0);
	points[5][0] = Polynomial("x^3 + 3*x + 7");	points[5][1] = Polynomial(0); 				points[5][2] = Polynomial(0);
	points[6][0] = Polynomial(0); 				points[6][1] = Polynomial("x + 6"); 		points[6][2] = Polynomial("x");

	// Solving polynomials
	{
		PolynomialKernel::RootStack roots;
		std::cout << "Solving " << points[5][0] << ": " << std::endl;
		PolynomialKernel::solve(points[5][0], roots);
		while (!roots.empty()) { std::cout << roots.top() << std::endl; roots.pop(); }
	}
	
	{
		Polynomial p("x^3 - 2*x + 1");
		PolynomialKernel::RootStack roots;
		std::cout << "Solving " << p << ": " << std::endl;
		PolynomialKernel::solve(p, roots);
		while (!roots.empty()) { std::cout << roots.top() << std::endl; roots.pop(); }
	}

#if 0
	// FIXME: explore
	{
		UPolynomial<QQ>::Polynomial p("1.2*x + 3.67");
		UPolynomial<QQ>::RootStack roots;
		UPolynomial<QQ>::solve(p, roots);
		while (!roots.empty()) { std::cout << roots.top() << std::endl; roots.pop(); }
	}
#endif

	{
		RationalF r1 = Polynomial("2*x - 4");
		RationalF r2 = Polynomial("x^3 - 3");
		RationalF r3 = Polynomial("x^2 - 3*x^3");
		std::cout << r2 - r1 << std::endl;
		std::cout << RationalF(Polynomial("2*x"), Polynomial(1)*Polynomial(1)) << std::endl;
		
		PolynomialKernel::RootStack roots;
		std::cout << "Solving " << (r2 - r1) << ": " << std::endl;
		PolynomialKernel::solve(r2 - r1, roots);
		while (!roots.empty()) { std::cout << roots.top() << std::endl; roots.pop(); }
		
		std::cout << "Solving " << (r3 - r1) << ": " << std::endl;
		PolynomialKernel::solve(r3 - r1, roots);
		while (!roots.empty()) { std::cout << roots.top() << std::endl; roots.pop(); }
		
		std::cout << "Solving " << (r3 - r2) << ": " << std::endl;
		PolynomialKernel::solve(r3 - r2, roots);
		//std::cout << "Sign of r3 at " << roots.top() << " is " << PolynomialKernel::sign_at(r3, roots.top()) << std::endl;
		while (!roots.empty()) { std::cout << roots.top() << std::endl; roots.pop(); }
	}

	// Edges
	{
		PointContainer vertices(2);
		vertices[0] = &points[0]; vertices[1] = &points[2];
		std::cout << "{0, 2}:" << std::endl;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;

		vertices[0] = &points[0]; vertices[1] = &points[3];
		std::cout << "{0, 3}:" << std::endl;
		s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
	}

#if 1
	// Triangles
	{
		PointContainer vertices(3);
		vertices[0] = &points[0]; vertices[1] = &points[3]; vertices[2] = &points[1];
		std::cout << "{0, 3, 1}:" << std::endl;;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		
		vertices[0] = &points[0]; vertices[1] = &points[4]; vertices[2] = &points[5];
		std::cout << "{0, 4, 5}:" << std::endl;
		s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
		
		// Degenerate
		vertices[0] = &points[0]; vertices[1] = &points[1]; vertices[2] = &points[6];
		std::cout << "{0, 1, 6}:" << std::endl;
		s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
	}

	// Tetrahedron
	{
		PointContainer vertices(4);
		vertices[0] = &points[3]; vertices[1] = &points[1]; vertices[2] = &points[2]; vertices[3] = &points[0];
		std::cout << "{3, 1, 2, 0}:" << std::endl;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << " " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, *vertices[1]) << std::endl;
	}
	
	// Tetrahedron
	{
		PointContainer vertices(3);
		vertices[0] = &points[3]; vertices[1] = &points[1]; vertices[2] = &points[2];
		std::cout << "{3, 1, 2}:" << std::endl;
		Sphere s = k.circumsphere(vertices);
		std::cout << "Circumsphere: " << s.center() << ", radius: " << s.squared_radius() << std::endl;
		std::cout << "Side of: " << k.side_of_circumsphere(vertices, points[0]) << std::endl;
	}
#endif
}
