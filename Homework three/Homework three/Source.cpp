/*
Name: Anton Vasilev Dudov
#71488

Homework 3
HPC 2015
*/

#include <stdio.h> // printf
#include <cstdlib> // rand
#include <immintrin.h> // _mm256_set_epi8 & _mm256_storeu_si256 and the intrinsics for the bonus part (SSE)
//#include <chrono> // For time checking
//using namespace std::chrono; // For time checking

inline void reverseBasic(char* bytes, int numChunks);
inline void reverseBasicWithoutSecondLoop(char* bytes, int numChunks);
inline void reverse(char * bytes, int numChunks);
void testValueValidation(void(*pFunc)(char*, int), int size, char flagForPrintValues);
void testTime(void(*pFunc)(char*, int), int numChunks);
void printArr(char * arr, int size);
bool compareArrayWithReversedOneByChunks(char * arr, char * arrReversed, int chunks, char flag);
// BONUS PART
inline static void foo(float(&inout)[8]);
inline static void bar(float(&inout)[8]);
static void insertion_sort(float(&inout)[8]);
void test();


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

// Reverse with intrinsics
// First - reverse the values in the first half and second half
// Second - copy the second half of temp array to the beginning of original one, and first half (from temp array) next to the first in original one
// (swap the halfs)
inline void reverse(char * bytes, int numChunks)
{
	static char temp[64];
	__m256i firstHalf, secondHalf;

	for (int i = 0; i < numChunks; ++i)
	{
		// Reverse first half.
		firstHalf = _mm256_set_epi8(bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7], bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15],
			bytes[16], bytes[17], bytes[18], bytes[19], bytes[20], bytes[21], bytes[22], bytes[23], bytes[24], bytes[25], bytes[26], bytes[27], bytes[28], bytes[29], bytes[30], bytes[31]);

		// Reverse second half.
		secondHalf = _mm256_set_epi8(bytes[32], bytes[33], bytes[34], bytes[35], bytes[36], bytes[37], bytes[38], bytes[39], bytes[40], bytes[41], bytes[42], bytes[43], bytes[44], bytes[45], bytes[46], bytes[47], bytes[48],
			bytes[49], bytes[50], bytes[51], bytes[52], bytes[53], bytes[54], bytes[55], bytes[56], bytes[57], bytes[58], bytes[59], bytes[60], bytes[61], bytes[62], bytes[63]);

		// write the second half at the begining, and after it- first one.
		_mm256_storeu_si256((__m256i*)bytes, secondHalf);
		_mm256_storeu_si256((__m256i*)(bytes + 32), firstHalf);

		bytes += 64;
	}
}

// Runs the given function and prints the taken time.
void testTime(void(*pFunc)(char*, int), int numChunks)
{
	//int sizeMul64 = numChunks << 6;
	//char * arr = new char[sizeMul64];

	//printf("Starting time test with %d chunks (%d elements)!\n", numChunks, sizeMul64);

	//// Fill with some random values.
	//srand(0);
	//for (size_t i = 0; i < sizeMul64; ++i)
	//	arr[i] = rand() % 256 - 128; // -128 to 127

	//auto begin = high_resolution_clock::now();

	//pFunc(arr, numChunks);

	//auto end = high_resolution_clock::now();
	//auto ticks = duration_cast<microseconds>(end - begin);

	//printf(" ---> It took me %d microseconds.\n", ticks.count());

	//delete[] arr;
}


// Basic test. Makes @size chunks of 64 elements and writes in them random values, reverse them and checks if they are ok. 
// IF @flagForPrintValues is set, prints the valus of the arrays(and the result for every chunk).
void testValueValidation(void(*pFunc)(char*, int), int size, char flagForPrintValues)
{
	// Allocate memory for the control values(@arr) and the same one in @arrReversed which will be reversed
	int sizeMul64 = size << 6;
	char * arr = new char[sizeMul64];
	char * arrReversed = new char[sizeMul64];

	printf("Starting value validation tests with %d chunks (%d elements)!\n", size, sizeMul64);

	// Fill with some random values.
	srand(0);
	for (size_t i = 0; i < sizeMul64; ++i)
		arr[i] = arrReversed[i] = rand() % 256 - 128; // -128 to 127

	// Prints the origin values.
	if (flagForPrintValues)
		printArr(arr, sizeMul64);

	// Reverse it in every chunk
	pFunc(arrReversed, size);

	if (flagForPrintValues)
	{
		printf("\n[REVERSED]\n\n");

		// Prints the reversed values values.
		printArr(arrReversed, sizeMul64);
		printf("\nLet us check the values:\n");
	}

	// Check if it`s reversed correctly.
	if (compareArrayWithReversedOneByChunks(arr, arrReversed, size, flagForPrintValues))
		printf(" ---> [Passed!]\n");
	else
		printf(" ---> [Failed!]\n");
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



/*
*
*
*	BONUS PART:
*
*
*/
inline static void foo(float(&inout)[8])
{
	static const size_t idx[][2] = {
			{ 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
			{ 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
			{ 1, 2 }, { 5, 6 },
			{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
			{ 2, 4 }, { 3, 5 },
			{ 1, 2 }, { 3, 4 }, { 5, 6 }
	};
	static const size_t size = sizeof(idx) / sizeof(idx[0]);
	float temp;

	for (size_t i = 0; i < size; ++i) {
		if (inout[idx[i][0]] > inout[idx[i][1]])
		{
			temp = inout[idx[i][0]];
			inout[idx[i][0]] = inout[idx[i][1]];
			inout[idx[i][1]] = temp;
		}
	}
}


inline static void bar(float(&inout)[8])
{
	__m128 leftSideElements[6],
		rightSideElements[6],
		leftGERight[6],
		leftLTRight[6],
		leftElementsGE[6],  // swaped elements on the left part of comparison
		leftElementsLT[6],  // not-swaped elements on the left part of comparison
		rightElementsGE[6], // swaped elements on the right part of comparison
		rightElementsLT[6]; // not-swaped elements on the right part of comparison
	float resultLeftElements[6][4], resultRightElements[6][4];

	const size_t idx[][2] = {
			{ 0, 1 }, { 3, 2 }, { 4, 5 }, { 7, 6 },
			{ 0, 2 }, { 1, 3 }, { 6, 4 }, { 7, 5 },
			{ 0, 1 }, { 2, 3 }, { 5, 4 }, { 7, 6 },
			{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
			{ 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
			{ 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 }
	};

	// First row
	leftSideElements[0] = _mm_set_ps(inout[idx[3][0]], inout[idx[2][0]], inout[idx[1][0]], inout[idx[0][0]]);
	rightSideElements[0] = _mm_set_ps(inout[idx[3][1]], inout[idx[2][1]], inout[idx[1][1]], inout[idx[0][1]]);

	leftGERight[0] = _mm_cmpge_ps(leftSideElements[0], rightSideElements[0]); // Something like 0 0 -1 -1.
	leftLTRight[0] = _mm_cmplt_ps(leftSideElements[0], rightSideElements[0]); // Something like -1 -1 0 0.

	// Calculates the values of the elements on the left.
	leftElementsGE[0] = _mm_and_ps(rightSideElements[0], leftGERight[0]); // If the element on left side is bigger or equal to the element on the right side - swaps, so writes the element on the left side to be the element on the right.
	leftElementsLT[0] = _mm_and_ps(leftSideElements[0], leftLTRight[0]);  // If the element on the left side is less than element on the right side - don`t swap and writes the element on left side on it`s place.

	// Calculates the values of the elements on the right
	rightElementsGE[0] = _mm_and_ps(leftSideElements[0], leftGERight[0]);  // If the element on the left side is bigger or equal to the element on the right side - swaps, so writes on the element on the right side to be the element on the left.
	rightElementsLT[0] = _mm_and_ps(rightSideElements[0], leftLTRight[0]); // If the element on the left side is less than element on the right side - don`t swap and writes the element on the right side on it`s place.

	// Now let`s combine the elements, because we have two vectors @leftGERight and @leftLTRight, which are basically inverted, so one OR operation will do it.
	// (in the @leftElemetnsGE will have something like [0, 0, element, element] and in the @leftElemetnsLT will be [element, element, 0, 0]) 
	leftSideElements[0] = _mm_or_ps(leftElementsGE[0], leftElementsLT[0]);
	rightSideElements[0] = _mm_or_ps(rightElementsGE[0], rightElementsLT[0]);

	// Now let`s write them in our array so we can put them in their original places on the given @inout.
	_mm_storeu_ps(resultLeftElements[0], leftSideElements[0]);
	_mm_storeu_ps(resultRightElements[0], rightSideElements[0]);

	// Puts the swaped(if needed) elements on their places.
	inout[idx[0][0]] = resultLeftElements[0][0];
	inout[idx[0][1]] = resultRightElements[0][0];
	inout[idx[1][0]] = resultLeftElements[0][1];
	inout[idx[1][1]] = resultRightElements[0][1];
	inout[idx[2][0]] = resultLeftElements[0][2];
	inout[idx[2][1]] = resultRightElements[0][2];
	inout[idx[3][0]] = resultLeftElements[0][3];
	inout[idx[3][1]] = resultRightElements[0][3];

	// Second row
	leftSideElements[1] = _mm_set_ps(inout[idx[7][0]], inout[idx[6][0]], inout[idx[5][0]], inout[idx[4][0]]);
	rightSideElements[1] = _mm_set_ps(inout[idx[7][1]], inout[idx[6][1]], inout[idx[5][1]], inout[idx[4][1]]);

	leftGERight[1] = _mm_cmpge_ps(leftSideElements[1], rightSideElements[1]);
	leftLTRight[1] = _mm_cmplt_ps(leftSideElements[1], rightSideElements[1]);

	leftElementsGE[1] = _mm_and_ps(rightSideElements[1], leftGERight[1]);
	leftElementsLT[1] = _mm_and_ps(leftSideElements[1], leftLTRight[1]);

	rightElementsGE[1] = _mm_and_ps(leftSideElements[1], leftGERight[1]);
	rightElementsLT[1] = _mm_and_ps(rightSideElements[1], leftLTRight[1]);

	leftSideElements[1] = _mm_or_ps(leftElementsGE[1], leftElementsLT[1]);
	rightSideElements[1] = _mm_or_ps(rightElementsGE[1], rightElementsLT[1]);

	_mm_storeu_ps(resultLeftElements[1], leftSideElements[1]);
	_mm_storeu_ps(resultRightElements[1], rightSideElements[1]);

	inout[idx[4][0]] = resultLeftElements[1][0];
	inout[idx[4][1]] = resultRightElements[1][0];
	inout[idx[5][0]] = resultLeftElements[1][1];
	inout[idx[5][1]] = resultRightElements[1][1];
	inout[idx[6][0]] = resultLeftElements[1][2];
	inout[idx[6][1]] = resultRightElements[1][2];
	inout[idx[7][0]] = resultLeftElements[1][3];
	inout[idx[7][1]] = resultRightElements[1][3];

	// Third row
	leftSideElements[2] = _mm_set_ps(inout[idx[11][0]], inout[idx[10][0]], inout[idx[9][0]], inout[idx[8][0]]);
	rightSideElements[2] = _mm_set_ps(inout[idx[11][1]], inout[idx[10][1]], inout[idx[9][1]], inout[idx[8][1]]);

	leftGERight[2] = _mm_cmpge_ps(leftSideElements[2], rightSideElements[2]);
	leftLTRight[2] = _mm_cmplt_ps(leftSideElements[2], rightSideElements[2]);

	leftElementsGE[2] = _mm_and_ps(rightSideElements[2], leftGERight[2]);
	leftElementsLT[2] = _mm_and_ps(leftSideElements[2], leftLTRight[2]);

	rightElementsGE[2] = _mm_and_ps(leftSideElements[2], leftGERight[2]);
	rightElementsLT[2] = _mm_and_ps(rightSideElements[2], leftLTRight[2]);

	leftSideElements[2] = _mm_or_ps(leftElementsGE[2], leftElementsLT[2]);
	rightSideElements[2] = _mm_or_ps(rightElementsGE[2], rightElementsLT[2]);

	_mm_storeu_ps(resultLeftElements[2], leftSideElements[2]);
	_mm_storeu_ps(resultRightElements[2], rightSideElements[2]);

	inout[idx[8][0]] = resultLeftElements[2][0];
	inout[idx[8][1]] = resultRightElements[2][0];
	inout[idx[9][0]] = resultLeftElements[2][1];
	inout[idx[9][1]] = resultRightElements[2][1];
	inout[idx[10][0]] = resultLeftElements[2][2];
	inout[idx[10][1]] = resultRightElements[2][2];
	inout[idx[11][0]] = resultLeftElements[2][3];
	inout[idx[11][1]] = resultRightElements[2][3];

	// Fourth row
	leftSideElements[3] = _mm_set_ps(inout[idx[15][0]], inout[idx[14][0]], inout[idx[13][0]], inout[idx[12][0]]);
	rightSideElements[3] = _mm_set_ps(inout[idx[15][1]], inout[idx[14][1]], inout[idx[13][1]], inout[idx[12][1]]);

	leftGERight[3] = _mm_cmpge_ps(leftSideElements[3], rightSideElements[3]);
	leftLTRight[3] = _mm_cmplt_ps(leftSideElements[3], rightSideElements[3]);

	leftElementsGE[3] = _mm_and_ps(rightSideElements[3], leftGERight[3]);
	leftElementsLT[3] = _mm_and_ps(leftSideElements[3], leftLTRight[3]);

	rightElementsGE[3] = _mm_and_ps(leftSideElements[3], leftGERight[3]);
	rightElementsLT[3] = _mm_and_ps(rightSideElements[3], leftLTRight[3]);

	leftSideElements[3] = _mm_or_ps(leftElementsGE[3], leftElementsLT[3]);
	rightSideElements[3] = _mm_or_ps(rightElementsGE[3], rightElementsLT[3]);

	_mm_storeu_ps(resultLeftElements[3], leftSideElements[3]);
	_mm_storeu_ps(resultRightElements[3], rightSideElements[3]);

	inout[idx[12][0]] = resultLeftElements[3][0];
	inout[idx[12][1]] = resultRightElements[3][0];
	inout[idx[13][0]] = resultLeftElements[3][1];
	inout[idx[13][1]] = resultRightElements[3][1];
	inout[idx[14][0]] = resultLeftElements[3][2];
	inout[idx[14][1]] = resultRightElements[3][2];
	inout[idx[15][0]] = resultLeftElements[3][3];
	inout[idx[15][1]] = resultRightElements[3][3];

	// Fifth row
	leftSideElements[4] = _mm_set_ps(inout[idx[19][0]], inout[idx[18][0]], inout[idx[17][0]], inout[idx[16][0]]);
	rightSideElements[4] = _mm_set_ps(inout[idx[19][1]], inout[idx[18][1]], inout[idx[17][1]], inout[idx[16][1]]);

	leftGERight[4] = _mm_cmpge_ps(leftSideElements[4], rightSideElements[4]);
	leftLTRight[4] = _mm_cmplt_ps(leftSideElements[4], rightSideElements[4]);

	leftElementsGE[4] = _mm_and_ps(rightSideElements[4], leftGERight[4]);
	leftElementsLT[4] = _mm_and_ps(leftSideElements[4], leftLTRight[4]);

	rightElementsGE[4] = _mm_and_ps(leftSideElements[4], leftGERight[4]);
	rightElementsLT[4] = _mm_and_ps(rightSideElements[4], leftLTRight[4]);

	leftSideElements[4] = _mm_or_ps(leftElementsGE[4], leftElementsLT[4]);
	rightSideElements[4] = _mm_or_ps(rightElementsGE[4], rightElementsLT[4]);

	_mm_storeu_ps(resultLeftElements[4], leftSideElements[4]);
	_mm_storeu_ps(resultRightElements[4], rightSideElements[4]);

	inout[idx[16][0]] = resultLeftElements[4][0];
	inout[idx[16][1]] = resultRightElements[4][0];
	inout[idx[17][0]] = resultLeftElements[4][1];
	inout[idx[17][1]] = resultRightElements[4][1];
	inout[idx[18][0]] = resultLeftElements[4][2];
	inout[idx[18][1]] = resultRightElements[4][2];
	inout[idx[19][0]] = resultLeftElements[4][3];
	inout[idx[19][1]] = resultRightElements[4][3];

	// Sixth row
	leftSideElements[5] = _mm_set_ps(inout[idx[23][0]], inout[idx[22][0]], inout[idx[21][0]], inout[idx[20][0]]);
	rightSideElements[5] = _mm_set_ps(inout[idx[23][1]], inout[idx[22][1]], inout[idx[21][1]], inout[idx[20][1]]);

	leftGERight[5] = _mm_cmpge_ps(leftSideElements[5], rightSideElements[5]);
	leftLTRight[5] = _mm_cmplt_ps(leftSideElements[5], rightSideElements[5]);

	leftElementsGE[5] = _mm_and_ps(rightSideElements[5], leftGERight[5]);
	leftElementsLT[5] = _mm_and_ps(leftSideElements[5], leftLTRight[5]);

	rightElementsGE[5] = _mm_and_ps(leftSideElements[5], leftGERight[5]);
	rightElementsLT[5] = _mm_and_ps(rightSideElements[5], leftLTRight[5]);

	leftSideElements[5] = _mm_or_ps(leftElementsGE[5], leftElementsLT[5]);
	rightSideElements[5] = _mm_or_ps(rightElementsGE[5], rightElementsLT[5]);

	_mm_storeu_ps(resultLeftElements[5], leftSideElements[5]);
	_mm_storeu_ps(resultRightElements[5], rightSideElements[5]);

	inout[idx[20][0]] = resultLeftElements[5][0];
	inout[idx[20][1]] = resultRightElements[5][0];
	inout[idx[21][0]] = resultLeftElements[5][1];
	inout[idx[21][1]] = resultRightElements[5][1];
	inout[idx[22][0]] = resultLeftElements[5][2];
	inout[idx[22][1]] = resultRightElements[5][2];
	inout[idx[23][0]] = resultLeftElements[5][3];
	inout[idx[23][1]] = resultRightElements[5][3];
}


static void insertion_sort(float(&inout)[8])
{
	for (size_t i = 1; i < 8; ++i) {
		size_t pos = i;
		const float val = inout[pos];

		while (pos > 0 && val < inout[pos - 1]) {
			inout[pos] = inout[pos - 1];
			--pos;
		}

		inout[pos] = val;
	}
}

// Realy simple test function.
void test()
{
	printf("\n\nSome realy simple basic test for @bar function\n");
	float arr[8] = { 20, 10, -20, -20, 50, 60, -125, -2301 };
	for (size_t j = 0; j < 7; ++j)
		printf("%.2f ", arr[j]);
	printf("\n");

	bar(arr);

	for (size_t j = 0; j < 7; ++j)
		printf("%.2f ", arr[j]);
	printf("\n");
}

/*
*
*
*	END OF BONUS PART:
*
*
*/


int main()
{
	//testValueValidation(reverseBasicWithoutSecondLoop, 3, true);
	//testValueValidation(reverse, 3, true);

	printf("\n\Value validation testing Basic:\n");
	testValueValidation(reverseBasic, 1 << 4, false);  // 2 ^ 4 chuncks
	testValueValidation(reverseBasic, 1 << 10, false); // 2 ^ 10 chuncks
	testValueValidation(reverseBasic, 1 << 15, false); // 2 ^ 15 chuncks 	

	printf("\n\Value validation testing without secon loop:\n");
	testValueValidation(reverseBasicWithoutSecondLoop, 1 << 4, false);
	testValueValidation(reverseBasicWithoutSecondLoop, 1 << 10, false);
	testValueValidation(reverseBasicWithoutSecondLoop, 1 << 15, false);

	printf("\n\Value validation testing with intrinsics:\n");
	testValueValidation(reverse, 1 << 4, false);
	testValueValidation(reverse, 1 << 10, false);
	testValueValidation(reverse, 2 << 15, false);

	//printf("\n\nTime testing (Basic, without second loop, intrinsics):\n");
	//testTime(reverseBasic, 1 << 16);
	//testTime(reverseBasicWithoutSecondLoop, 1 << 16);
	//testTime(reverse, 1 << 16);
	//printf("\n");

	//testTime(reverseBasic, 1 << 20);
	//testTime(reverseBasicWithoutSecondLoop, 1 << 20);
	//testTime(reverse, 1 << 20);	
	//printf("\n");

	//testTime(reverseBasic, 1 << 24);
	//testTime(reverseBasicWithoutSecondLoop, 1 << 24);
	//testTime(reverse, 1 << 24);
	//printf("\n");

	test();

	return 0;
}