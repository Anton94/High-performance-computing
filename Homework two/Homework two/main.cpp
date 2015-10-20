/*
*	HPC Homework two
*	Anton Vasilev Dudov
*	#71488
*	antonvdudov@gmail.com
*
*	Github repository: https://github.com/Anton94/High-performance-computing/tree/master/Homework%20two
*   TO DO : Delete time checker!!!!
*	TO DO : make a test where I don`t reverse all bits, only from the MSB to the end.
*/

#include <stdio.h>
#include <chrono>// TO DO delete it
using namespace std::chrono;// TO DO delete it

inline float derCorput(unsigned n)
{
	unsigned bitCount = sizeof(n) * 8;
	bitCount -= 2; // -1 for the sign bit and -1 for the max number, I will divide by 1000...0, so 1 bit for the divider
	unsigned reversedBinary = 0;
	for (unsigned i = 0; i < bitCount; ++i)
	{
		reversedBinary <<= 1;
		if ((n >> i) & 1)
			++reversedBinary;
	}

	return (float)reversedBinary / (float)(1 << 30); // TO DO- optimize
}

// Test - calculate the numbers from 0 to the given @n with/without printing it. 
// If @flag- than print the values.
void test1(unsigned n, char flag)
{
	printf("Test: get the numbers in sequence from 0 to %d\n", n);
	auto begin = high_resolution_clock::now();

	if (flag)
		for (unsigned i = 0; i < n; ++i)
			printf("%d number in the van der Corput sequence is %f\n", i, derCorput(i));
	else
		for (unsigned i = 0; i < n; ++i)
			derCorput(i);

	auto end = high_resolution_clock::now();
	auto ticks = duration_cast<microseconds>(end - begin);

	printf("It took me %d microseconds.\n", ticks.count());
}

int main()
{
	test1(20, 1);
	test1(20000000, 0);
	return 0;
}