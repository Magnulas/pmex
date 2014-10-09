#include "geometry/linalg.h"
#include <iostream>

#include <synaps/upol.h>
#include <synaps/upol/gcd.h>
#include "geometry/rational-function.h"


typedef			UPolDse<double>					Polynomial;
typedef			RationalFunction<Polynomial>	RationalF;

//typedef 		LinearAlgebra<double>			LinAlg;
//typedef 		LinearAlgebra<Polynomial>		LinAlg;
typedef 		LinearAlgebra<RationalF>		LinAlg;

int main()
{
	LinAlg::MatrixType  a(2,2);
	a(0,0) = Polynomial("3*x"); a(1,0) = Polynomial(4); a(0,1) = Polynomial(0); a(1,1) = Polynomial(7);

	std::cout << a << std::endl;
	std::cout << LinAlg::determinant(a) << std::endl;
	std::cout << Polynomial("3*x^2 + 4*x") / Polynomial("3*x^2") << std::endl;

	LinAlg::VectorType b(2), x;
	b(0) = Polynomial(4); b(1) = Polynomial(3);
	LinAlg::solve(a, b, x);
	std::cout << x << std::endl;

	x(0).normalize(); x(1).normalize();
	std::cout << x << std::endl;
}
