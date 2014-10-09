#include "utilities/consistencylist.h"
#include <iostream>

typedef ConsistencyList<int> 		OList;
typedef OList::OrderComparison		Comparison;
typedef OList::iterator 			iterator;

int main()
{
#ifdef CWDEBUG
	dionysus::debug::init();

	//Debug(dc::orderlist.on());
#endif
		
	OList list; Comparison cmp;
	iterator i20 = list.push_back(20);
	iterator i50 = list.push_back(50);
	list.show_elements();
	
	iterator i30 = list.insert(i20, 30);
	list.show_elements();
	
	iterator i40 = list.insert(i30, 40);
	iterator i60 = list.insert(i50, 60);
	list.show_elements();
	
	iterator i70 = list.push_back(70);
	iterator i45 = list.insert(i40,45);
	iterator i25 = list.insert(i20,25);
	iterator i35 = list.insert(i30,35);
	iterator i55 = list.insert(i50,55);
	iterator i65 = list.insert(i60,65);
	iterator i57 = list.insert(i55,57);
	std::cout << "Before swaps" << std::endl;
	list.show_elements();

	list.swap(i45, i50);
	list.swap(i45, i65);
	std::cout << "After swaps" << std::endl;
	list.show_elements();
	
	std::cout << *(++list.end()) << std::endl;
	std::cout << *(--list.end()) << std::endl;
	std::cout << std::endl;

	for (int i = 0; i < 100000; ++i)
	{
		list.insert(i20,i);
	}
	list.show_elements();
	
	std::cout << "20 < 30: " << cmp.compare(i20,i30) << std::endl;
	std::cout << "60 < 40: " << cmp.compare(i60,i40) << std::endl;
	std::cout << "50 < 70: " << cmp.compare(i50,i70) << std::endl;
	std::cout << "60 < 70: " << cmp.compare(i60,i70) << std::endl;
}
