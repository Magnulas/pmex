#include "grid2D.h"
#include <iostream>

int main()
{
	Grid2D grid(40,60);
	int i = 0;
	for (int x = 0; x < 40; ++x)
		for (int y = 0; y < 60; ++y)
		{
			grid(x,y) = i++;
		}

	std::cout << grid(20,30) << std::endl;
	std::cout << grid << std::endl;
}
