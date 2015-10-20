#include <stdio.h>
#include <iostream>

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

	return (float)reversedBinary / (float)(1 << 30);// TO DO- optimize
}

void derCorputTestWith(unsigned n)
{
	printf("%d number in the van der Corput sequence is %f\n", n, derCorput(n));
}

int main()
{
	for (size_t i = 0; i < 20; ++i)
		derCorputTestWith(i);
	return 0;
}