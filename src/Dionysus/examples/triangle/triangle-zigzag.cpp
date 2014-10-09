#include <vector>
#include <cassert>
#include <iostream>

#include <topology/simplex.h>
#include <topology/zigzag-persistence.h>
#include <boost/tuple/tuple.hpp>

typedef     ZigzagPersistence<unsigned>     Zigzag;
typedef     Zigzag::SimplexIndex            Index;
typedef     Zigzag::Death                   Death;
typedef     Zigzag::ZColumn                 Boundary;
typedef     std::vector<Index>              Complex;

int main(int argc, char** argv)
{
#ifdef LOGGING
    rlog::RLogInit(argc, argv);

    //stdoutLog.subscribeTo(RLOG_CHANNEL("topology/persistence"));
#endif


    Zigzag zz;
    Complex c;
    Index i; Death d;
    unsigned birth = 0;

    // Adding the triangle
    std::cout << birth << ": adding 0" << std::endl;
    boost::tie(i, d)        = zz.add(Boundary(), birth++);  // A
    c.push_back(i);
    assert(!d);                                             // birth

    std::cout << birth << ": adding 1" << std::endl;
    boost::tie(i, d)        = zz.add(Boundary(), birth++);  // B
    c.push_back(i);
    assert(!d);                                             // birth

    std::cout << birth << ": adding 2" << std::endl;
    boost::tie(i, d)        = zz.add(Boundary(), birth++); // C
    c.push_back(i);
    assert(!d);                                             // birth
    
    std::cout << birth << ": adding 3" << std::endl;
    boost::tie(i, d)        = zz.add(Boundary(c.begin(), 
                                              boost::next(c.begin(),2)), 
                                     birth++);              // AB
    c.push_back(i);
    assert(d);                                              // death
    if (d)                  std::cout << "Death of: " << *d << std::endl;

    std::cout << birth << ": adding 4" << std::endl;
    boost::tie(i, d)        = zz.add(Boundary(boost::next(c.begin()), 
                                              boost::next(c.begin(),3)), 
                                     birth++);              // BC
    c.push_back(i);
    assert(d);                                              // death
    if (d)                  std::cout << "Death of: " << *d << std::endl;
    
    std::cout << birth << ": adding 5" << std::endl;
    {
        Boundary bdry; bdry.append(*c.begin(), zz.cmp); bdry.append(*boost::next(c.begin(), 2), zz.cmp);
        boost::tie(i, d)    = zz.add(bdry, birth++);        // AC
    }
    c.push_back(i);
    assert(!d);                                             // birth
    
    std::cout << birth << ": adding 6" << std::endl;
    boost::tie(i, d)        = zz.add(Boundary(boost::next(c.begin(), 3), 
                                              boost::next(c.begin(), 6)), 
                                     birth++);              // ABC
    c.push_back(i);
    assert(d);                                              // death
    if (d)                  std::cout << "Death of: " << *d << std::endl;

    //zz.show_all();

    // Removing the triangle in reverse order
    for (Complex::reverse_iterator cur = c.rbegin(); cur != c.rend(); ++cur)
    {
        std::cout << birth << ": removing " << (*cur)->order << std::endl;
        d = zz.remove(*cur, birth++);
        if (d)              std::cout << "Death of: " << *d << std::endl;
        //zz.show_all();
    }
}
