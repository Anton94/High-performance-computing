#include <stdio.h> // printf
#include <cstdlib> // rand


#include <chrono>			
using namespace std::chrono;

inline void reverseBasic(char* bytes, int numChunks);
void testValueValidation(int size, char flagForPrintValues);
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


inline void reverseBasicWithoutSecondLoop(char* bytes, int numChunks)
{
	static char temp[32];

	for (int i = 0; i < numChunks; ++i)
	{
		temp[0] = bytes[0];
		bytes[0] = bytes[63];
		bytes[63] = temp[0];

		temp[1] = bytes[1];
		bytes[1] = bytes[62];
		bytes[62] = temp[1];

		temp[2] = bytes[2];
		bytes[2] = bytes[61];
		bytes[61] = temp[2];

		temp[3] = bytes[3];
		bytes[3] = bytes[60];
		bytes[60] = temp[3];

		temp[4] = bytes[4];
		bytes[4] = bytes[59];
		bytes[59] = temp[4];

		temp[5] = bytes[5];
		bytes[5] = bytes[58];
		bytes[58] = temp[5];

		temp[6] = bytes[6];
		bytes[6] = bytes[57];
		bytes[57] = temp[6];

		temp[7] = bytes[7];
		bytes[7] = bytes[56];
		bytes[56] = temp[7];

		temp[8] = bytes[8];
		bytes[8] = bytes[55];
		bytes[55] = temp[8];

		temp[9] = bytes[9];
		bytes[9] = bytes[54];
		bytes[54] = temp[9];

		temp[10] = bytes[10];
		bytes[10] = bytes[53];
		bytes[53] = temp[10];

		temp[11] = bytes[11];
		bytes[11] = bytes[52];
		bytes[52] = temp[11];


		temp[12] = bytes[12];
		bytes[12] = bytes[51];
		bytes[51] = temp[12];


		temp[13] = bytes[13];
		bytes[13] = bytes[50];
		bytes[50] = temp[13];

		temp[14] = bytes[14];
		bytes[14] = bytes[49];
		bytes[49] = temp[14];

		temp[15] = bytes[15];
		bytes[15] = bytes[48];
		bytes[48] = temp[15];

		temp[16] = bytes[16];
		bytes[16] = bytes[47];
		bytes[47] = temp[16];

		temp[17] = bytes[17];
		bytes[17] = bytes[46];
		bytes[46] = temp[17];

		temp[18] = bytes[18];
		bytes[18] = bytes[45];
		bytes[45] = temp[18];

		temp[19] = bytes[19];
		bytes[19] = bytes[44];
		bytes[44] = temp[19];

		temp[20] = bytes[20];
		bytes[20] = bytes[43];
		bytes[43] = temp[20];

		temp[21] = bytes[21];
		bytes[21] = bytes[42];
		bytes[42] = temp[21];

		temp[22] = bytes[22];
		bytes[22] = bytes[41];
		bytes[41] = temp[22];

		temp[23] = bytes[23];
		bytes[23] = bytes[40];
		bytes[40] = temp[23];

		temp[24] = bytes[24];
		bytes[24] = bytes[39];
		bytes[39] = temp[24];

		temp[25] = bytes[25];
		bytes[25] = bytes[38];
		bytes[38] = temp[25];

		temp[26] = bytes[26];
		bytes[26] = bytes[37];
		bytes[37] = temp[26];

		temp[27] = bytes[27];
		bytes[27] = bytes[36];
		bytes[36] = temp[27];

		temp[28] = bytes[28];
		bytes[28] = bytes[35];
		bytes[35] = temp[28];

		temp[29] = bytes[29];
		bytes[29] = bytes[34];
		bytes[34] = temp[29];

		temp[30] = bytes[30];
		bytes[30] = bytes[33];
		bytes[33] = temp[30];

		temp[31] = bytes[31];
		bytes[31] = bytes[32];
		bytes[32] = temp[31];

		bytes += 64;
	}
}

// Runs the given function and prints the taken time.
void testTime(void (*pFunc)(char* , int), int numChunks)
{	
	int sizeMul64 = numChunks << 6;
	char * arr = new char[sizeMul64];

	printf("Starting time test with %d chunks (%d elements)!\n", numChunks, sizeMul64);

	// Fill with some random values.
	srand(0);
	for (size_t i = 0; i < sizeMul64; ++i)
		arr[i] = rand() % 256 - 128; // -128 to 127


	auto begin = high_resolution_clock::now();

	pFunc(arr, numChunks);

	auto end = high_resolution_clock::now();
	auto ticks = duration_cast<microseconds>(end - begin);

	printf(" ---> It took me %d microseconds.\n", ticks.count());
}


// Basic test. Makes @size chunks of 64 elements and writes in them random values, reverse them and checks if they are ok. 
// IF @flagForPrintValues is set, prints the valus of the arrays(and the result for every chunk).
void testValueValidation(int size, char flagForPrintValues)
{
	// Allocate memory for the control values(@arr) and the same one in @arrReversed which will be reversed
	int sizeMul64 = size << 6;
	char * arr = new char[sizeMul64];
	char * arrReversed = new char[sizeMul64];

	printf("Starting value validation tests with %d chunks (%d elements)!", size, sizeMul64);

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
	//testValueValidation(3, true);

	//testValueValidation(13, false);
	//testValueValidation(26, false);
	//testValueValidation(2 << 20, false); // 2 ^ 21 

	testTime(reverseBasicWithoutSecondLoop, 2 << 15);
	testTime(reverseBasic, 2 << 15);

	printf("\n");

	testTime(reverseBasicWithoutSecondLoop, 2 << 20);
	testTime(reverseBasic, 2 << 20);
	
	printf("\n");
	
	testTime(reverseBasicWithoutSecondLoop, 2 << 22);
	testTime(reverseBasic, 2 << 22);

	printf("\n");

	return 0;
}