/*
*	HPC Homework two
*	Anton Vasilev Dudov
*	#71488
*	antonvdudov@gmail.com
*
*	Github repository: https://github.com/Anton94/High-performance-computing/tree/master/Homework%20two
*
*
*
*
*	TODO : To make a test where I don`t reverse all bits, only from the MSB to the end - (basic tests shows it`s slower).
*/

#include <stdio.h>
#include <chrono>			
using namespace std::chrono;




inline float derCorput(unsigned n)
{
	unsigned bitCount = (sizeof(unsigned) << 3) - 2;
	// -1 for the sign bit (for the float dividing) and -1 for the max number, I will divide by 1000...0, so 1 bit for the divider (can`t calculate number bigger than 100...0)

	unsigned reversedBinary = 0;
	for (unsigned i = 0; i < bitCount; ++i)
	{
		reversedBinary <<= 1;		
		reversedBinary |= ((n >> i) & 1); // If ((n >> i) & 1 == 1) THEN reversedBinary |= 1  ; (reversedBinary |= 1 === ++reversedBinary)
	}

	return (float)reversedBinary / (float)(1 << bitCount);
}




// Test - calculate the numbers from 0 to the given @n with/without printing it. 
// If @flag- than print the values.
void test1(unsigned n, char flag)
{
	printf("Test: Geting the numbers in the van der Corput sequence [0 to %d]\n", n);
	auto begin = high_resolution_clock::now();

	if (flag)
		for (unsigned i = 0; i < n; ++i)
			printf("Number %3u in the van der Corput sequence is %f\n", i, derCorput(i));
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
	test1(1000000, 0);
	test1(10000000, 0);
	test1(100000000, 0);

	return 0;
}






/*
	For tests...

	inline float derCorputWithFindingTheMSB(unsigned n)
	{
		unsigned bitCount = 1 + log2(n);

		unsigned reversedBinary = 0;
		for (unsigned i = 0; i < bitCount; ++i)
		{
			reversedBinary <<= 1;
			if ((n >> i) & 1)
				++reversedBinary;
		}

		return (float)reversedBinary / (float)(1 << bitCount);
	}
*/