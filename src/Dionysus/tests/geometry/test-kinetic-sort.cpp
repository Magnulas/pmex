#include <geometry/polynomial.h>
#include <geometry/simulator.h>
#include <geometry/kinetic-sort.h>
#include <iostream>

#include <boost/utility.hpp>
#include <boost/bind.hpp>

#include <utilities/log.h>

typedef		double							FieldType;
//typedef		ZZ								FieldType;
//typedef		QQ								FieldType;
typedef 	UPolynomial<FieldType>			PolyKernel;
typedef		PolyKernel::Polynomial			Polynomial;
typedef 	std::list<Polynomial>			SortDS;
typedef 	SortDS::iterator				SortDSIterator;
typedef		Simulator<PolyKernel>			SimulatorFT;

class TrajectoryExtractor
{
	public:
		Polynomial		operator()(SortDSIterator i) const			{ return *i; }
};

typedef		KineticSort<SortDSIterator, TrajectoryExtractor, SimulatorFT>		KineticSortDS;

struct EvaluatedComparison: public std::binary_function<const Polynomial&, const Polynomial&, bool>
{
								EvaluatedComparison(FieldType v): vv(v)	{}
	bool						operator()(const Polynomial& p1, const Polynomial& p2)				
								{ return p1(vv) < p2(vv); }
	FieldType					vv;
};

void swap(SortDS* s, SortDSIterator i, SimulatorFT* simulator)
{
	std::cout << "Swapping " << *i << " " << *boost::next(i) << std::endl;
	s->splice(i, *s, boost::next(i));
}

int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    stderrLog.subscribeTo( RLOG_CHANNEL("error") );
    stdoutLog.subscribeTo( RLOG_CHANNEL("geometry/simulator") );
    stdoutLog.subscribeTo( RLOG_CHANNEL("geometry/kinetic-sort") );
#endif

	SimulatorFT		simulator;
	SortDS 			list;

	// Insert polynomials and sort the list for current time
	list.push_back(Polynomial("x^3 - 3"));
	list.push_back(Polynomial("x^2 - 2*x - 2"));
	list.push_back(Polynomial("x^2 - 2*x - 2"));
	list.push_back(Polynomial("2*x - 4"));
	list.push_back(Polynomial("x"));
	list.push_back(Polynomial("-x + 4"));
	list.push_back(Polynomial("2"));
	//list.sort(EvaluatedComparison(simulator.current_time()));
	list.sort(EvaluatedComparison(0));

	// Print out the list
	for (SortDS::const_iterator cur = list.begin(); cur != list.end(); ++cur)
		std::cout << *cur << std::endl;

	// Setup kinetic sort
	KineticSortDS	ks(list.begin(), list.end(), boost::bind(swap, &list, _1, _2), &simulator);

    std::cout << "Examining " << simulator;

	while(!simulator.reached_infinity())
	{
		std::cout << "Current time before: " << simulator.current_time() << std::endl;
		//if (!ks.audit(&simulator)) return 1;
		ks.audit(&simulator);
        std::cout << "Examining " << simulator;
		simulator.process();
		std::cout << "Current time after: " << simulator.current_time() << std::endl;
	}
	ks.audit(&simulator);
    std::cout << "Examining " << simulator;

    std::cout << "Done at " << simulator.current_time() << std::endl;
    for (SortDS::const_iterator cur = list.begin(); cur != list.end(); ++cur)
        std::cout << "  " << *cur << std::endl;    
}
