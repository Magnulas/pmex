#include <geometry/simulator.h>
#include <geometry/kinetic-sort.h>
#include <geometry/linear-kernel.h>
#include <iostream>

#include <boost/utility.hpp>
#include <boost/bind.hpp>

#include <utilities/log.h>

typedef     double                          FieldType;
//typedef       ZZ                              FieldType;
//typedef       QQ                              FieldType;
typedef     LinearKernel<FieldType>         LKernel;
typedef     LKernel::Function               Function;
typedef     std::list<Function>             SortDS;
typedef     SortDS::iterator                SortDSIterator;
typedef     Simulator<LKernel>              SimulatorFT;

class TrajectoryExtractor
{
    public:
        Function                operator()(SortDSIterator i) const          { return *i; }
};

typedef     KineticSort<SortDSIterator, TrajectoryExtractor, SimulatorFT>       KineticSortDS;

struct EvaluatedComparison: public std::binary_function<const Function&, const Function&, bool>
{
                                EvaluatedComparison(FieldType v): vv(v) {}
    bool                        operator()(const Function& p1, const Function& p2)              
                                { return p1(vv) < p2(vv); }
    FieldType                   vv;
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

    SimulatorFT     simulator;
    SortDS          list;

    // Insert polynomials and sort the list for current time
    list.push_back(Function(2, -2));
    list.push_back(Function(1, 3));
    list.push_back(Function(-1, 6));
    list.push_back(Function(2));
    list.push_back(Function(2, 2));
    //list.sort(EvaluatedComparison(simulator.current_time()));
    list.sort(EvaluatedComparison(0));

    // Print out the list
    for (SortDS::const_iterator cur = list.begin(); cur != list.end(); ++cur)
        std::cout << *cur << std::endl;

    // Setup kinetic sort
    KineticSortDS   ks(list.begin(), list.end(), boost::bind(swap, &list, _1, _2), &simulator);

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
