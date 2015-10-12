/*
*	Anton Vasilev Dudov
*	#71488
*	antonvdudov@gmail.com
*
*
*	Better safe, than sorry :)
*/

#include <iostream>
#include <stdio.h>

class Sequence
{
	int * arr;
	size_t startSize;
	size_t currentSize;
public:
	Sequence(int n)
	{
		if (n < 0)
			throw "Can`t make Sequence, starting from negative number!";
		arr = new int[n];
		startSize = currentSize = n;

		for (int i = 0; i < startSize; ++i)
			arr[i] = i;
	}

	void erase(int i)
	{
		if (i < 0 || i >= currentSize)
			throw "Invalid index to erase!";
		
		shiftLeft(i);
	}

	int get(int i) const
	{
		if (i < 0 || i >= currentSize)
			throw "Invalid index to get!";

		return arr[i];
	}

	~Sequence()
	{
		delete[] arr;
	}
private:
	// NOTE: Does not checks for the valid value of the @pos !
	void shiftLeft(int pos)
	{
		int size = currentSize - 1;
		for (int i = pos; i < size; ++i)
			arr[i] = arr[i + 1];
	}
};

void testOne()
{
	int size = 10;
	Sequence s(size);
	for (int i = 0; i < 10; ++i) {
		printf("%i, ", s.get(i)); // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	}
	printf("\n");
}

void testTwo()
{
	int size = 10;
	Sequence s(size);
	s.erase(0);
	s.erase(0);
	s.erase(0);
	for (int i = 0; i < 7; ++i) {
		printf("%i, ", s.get(i)); // 3, 4, 5, 6, 7, 8, 9,
	}
	printf("\n");
}

void testThree()
{
	int size = 10;
	Sequence s(size);
	s.erase(9);
	s.erase(0);
	s.erase(7);
	s.erase(0);
	for (int i = 0; i < 6; ++i) {
		printf("%i, ", s.get(i)); // 2, 3, 4, 5, 6, 7,
	}
	printf("\n");
}

void testFour()
{
	int size = 10;
	Sequence s(size);
	s.erase(9);
	s.erase(5);
	s.erase(7);
	s.erase(2);
	s.erase(0);
	s.erase(0);
	s.erase(0);
	s.erase(0);
	s.erase(1);
	for (int i = 0; i < 1; ++i) {
		printf("%i, ", s.get(i)); // 6,
	}
	printf("\n");
}

int main()
{
	// Memory leaks cheching... May not work in something different than Visual studio...
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);
	{
		try
		{
			testOne();
			testTwo();
			testThree();
			testFour();
		}
		catch (const char* msg)
		{
			std::cerr << "What? " << msg << std::endl;
		}
	}

	_CrtMemCheckpoint(&s2);

	if (_CrtMemDifference(&s3, &s1, &s2))
	{
		std::cout << "Memory leak detected!" << std::endl;
		_CrtMemDumpStatistics(&s3);
	}
	else
	{
		std::cout << "Memory is OK!" << std::endl;
	}

	return 0;
}