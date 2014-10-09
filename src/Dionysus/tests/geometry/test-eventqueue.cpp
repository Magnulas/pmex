#include <utilities/eventqueue.h>
#include <functional>
#include <iostream>

int main()
{
	typedef EventQueue<int, std::less<int> >            EQ;
	typedef EQ::iterator								iterator;
	 
	EQ queue;

    std::cout << "Queue initialized" << std::endl;

	iterator i1 = queue.push(4);
	iterator i2 = queue.push(2);
	iterator i3 = queue.push(9);
	iterator i4 = queue.push(6);
	iterator i5 = queue.push(5);

    std::cout << "Values inserted" << std::endl;
    queue.print(std::cout, "  ");

    queue.replace(i1,1);
    queue.remove(i4);
    queue.replace(i5,10);

    *i3 = 14;
    queue.demoted(i3);
    
    std::cout << "Replaced and removed" << std::endl;

	while (!queue.empty())
	{
		std::cout << *queue.top() << std::endl;
		queue.pop();
	}
}
