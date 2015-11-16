#include <stdio.h> // printf
#include <cstdlib> // rand

inline void reverseBasic(char* bytes, int numChunks);
void test(int size, char flagForPrintValues);
void printArr(char * arr, int size);
bool compareArrayWithReversedOneByChunks(char * arr, char * arrReversed, int chunks, char flag);



inline void reverseBasic(char* bytes, int numChunks)
{
	int i = 0;
	int	leftIndex, rightIndex;
	char temp;

	for (; i < numChunks; ++i)
	{
		leftIndex = i << 6;
		rightIndex = leftIndex + 63; // + 64 - 1

		while (leftIndex < rightIndex)
		{
			temp = bytes[leftIndex];
			bytes[leftIndex] = bytes[rightIndex];
			bytes[rightIndex] = temp;
			++leftIndex;
			--rightIndex;
		}
	}
}

// Basic test. Makes @size chunks of 64 elements and writes in them random values, reverse them and checks if they are ok. 
// IF @flagForPrintValues is set, prints the valus of the arrays(and the result for every chunk).
void test(int size, char flagForPrintValues)
{
	// Allocate memory for the control values(@arr) and the same one in @arrReversed which will be reversed
	int sizeMul64 = size << 6;
	char * arr = new char[sizeMul64];
	char * arrReversed = new char[sizeMul64];

	printf("Starting tests with %d chunks (%d elements)!", size, sizeMul64);

	// Fill with some random values.
	srand(0);
	for (size_t i = 0; i < sizeMul64; ++i)
		arr[i] = arrReversed[i] = rand() % 256 - 128; // -128 to 127

	// Prints the origin values.
	if (flagForPrintValues)
		printArr(arr, sizeMul64);

	// Reverse it in every chunk
	reverseBasic(arrReversed, size);

	if (flagForPrintValues)
	{
		printf("\n[REVERSED]\n\n");

		// Prints the reversed values values.
		printArr(arrReversed, sizeMul64);
		printf("\nLet us check the values:\n");
	}

	// Check if it`s reversed correctly.
	if (compareArrayWithReversedOneByChunks(arr, arrReversed, size, flagForPrintValues))
		printf(" --- [Passed!]\n");
	else
		printf(" --- [Failed!]\n");
	// Delete memory
	delete[] arr;
	delete[] arrReversed;
}

// Compares to see if the @arr values are reversed in @arrReversed (by chunks). If no- prints error and returns false.
bool compareArrayWithReversedOneByChunks(char * arr, char * arrReversed, int chunks, char flag)
{
	int j = 0;
	for (int i = 0; i < chunks; ++i)
	{
		int endIndexOfChunk = j + 63; // + 64 - 1
		for (int k = 0; k < 64; ++j, ++k)
		{
			if (arr[j] != arrReversed[endIndexOfChunk - k])
			{
				printf("Error!!! The value in arr[%d] = %d and the value in arrReverse[%d] = %d\n", j, arr[j], endIndexOfChunk - j, arrReversed[endIndexOfChunk - j]);
				return false;
			}
		}
		if (flag)
			printf("---> Chunk %d is OK\n", i + 1);
	}

	return true;
}

// Prints the values of array of 64 chunks
void printArr(char * arr, int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (i && i % 64 == 0)
			printf("\n\n");
		printf("%d ", arr[i]);
	}
	printf("\n");
}

int main()
{
	//test(3, true);
	
	test(13, false);
	test(26, false);
	test(2 << 20, false); // 2 ^ 21 
	return 0;
}