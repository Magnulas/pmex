#include <vector>
#include <list>
#include <algorithm>
#include <iostream>

#include <utilities/indirect.h>

#include <boost/lambda/lambda.hpp>
using    boost::lambda::_1;

int main()
{
    std::vector<int>    v;
    v.push_back(1);
    v.push_back(3);
    v.push_back(5);
    v.push_back(7);
    v.push_back(9);
    std::cout << "v: ";
    std::for_each(v.begin(), v.end(), std::cout << _1 << ' ');
    std::cout << std::endl;

    std::list<int>      l;
    l.push_back(2);
    l.push_back(3);
    l.push_back(4);
    l.push_back(5);
    l.push_back(6);
    l.push_back(8);
    std::cout << "l: ";
    std::for_each(l.begin(), l.end(), std::cout << _1 << ' ');
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "v \\cap l: ";
    std::for_each(make_intersection_iterator(v.begin(), v.end(), l.begin(), l.end(), std::less<int>()),
                  make_intersection_iterator(v.end(),   v.end(), l.end(),   l.end(), std::less<int>()),
                  std::cout << _1 << ' ');
    std::cout << std::endl;
    
    std::cout << "v - l: ";
    std::for_each(make_difference_iterator(v.begin(),   v.end(), l.begin(), l.end(), std::less<int>()),
                  make_difference_iterator(v.end(),     v.end(), l.end(),   l.end(), std::less<int>()),
                  std::cout << _1 << ' ');
    std::cout << std::endl;
    std::cout << std::endl;
    
    std::cout << "l \\cap v: ";
    std::for_each(make_intersection_iterator(l.begin(), l.end(), v.begin(), v.end(), std::less<int>()),
                  make_intersection_iterator(l.end(),   l.end(), v.end(),   v.end(), std::less<int>()),
                  std::cout << _1 << ' ');
    std::cout << std::endl;
    
    std::cout << "l - v: ";
    std::for_each(make_difference_iterator(l.begin(),   l.end(), v.begin(), v.end(), std::less<int>()),
                  make_difference_iterator(l.end(),     l.end(), v.end(),   v.end(), std::less<int>()),
                  std::cout << _1 << ' ');
    std::cout << std::endl;
}
