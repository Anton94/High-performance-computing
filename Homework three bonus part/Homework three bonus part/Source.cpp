#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>



#include <xmmintrin.h> // SSE intrinsics



#define NOMINMAX
#include <Windows.h>

static uint64_t timer_nsec()
{
	static LARGE_INTEGER freq;
	static BOOL once = QueryPerformanceFrequency(&freq);

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	return 1000000000ULL * t.QuadPart / freq.QuadPart;
}

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
		leftElementsGE[6],
		leftElementsLT[6],
		rightElementsGE[6],
		rightElementsLT[6];
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

	// Now let`s combine the elements, because we have two vectors with inverted zeros, so on OR operation will do it.
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


static size_t verify(const size_t count, float* const input)
{
	assert(0 == count % 8);

	for (size_t i = 0; i < count; i += 8)
		for (size_t j = 0; j < 7; ++j)
			if (input[i + j] > input[i + j + 1])
				return i + j;

	return -1;
}

void test()
{
	float arr[8] = { 20, 10, -20, -20, 50, 60, -125, -2301 };
	for (size_t j = 0; j < 7; ++j)
		std::cout << arr[j] << " ";

	std::cout << std::endl;
	bar(arr);
	for (size_t j = 0; j < 7; ++j)
		std::cout << arr[j] << " ";

	std::cout << std::endl;
}
//
//int main()
//{
//	test();
//	return 0;
//}


int main(int argc, char** argv)
{
	unsigned alt = 1;
	const bool err = argc > 2 || argc == 2 && 1 != sscanf(argv[1], "%u", &alt);

	if (err || alt > 2) {
		std::cerr << "usage: " << argv[0] << " [opt]\n"
			"\t0 foo (default)\n"
			"\t1 bar\n"
			"\t2 insertion_sort\n"
			<< std::endl;
		return -3;
	}

	const size_t count = 1 << 27;
	float* input = (float*)malloc(sizeof(float) * count + 63);
	float* input_aligned = reinterpret_cast< float* >(uintptr_t(input) + 63 & -64);

	std::cout << std::hex << std::setw(8) << input << " (" << input_aligned << ") : " << std::dec << count << " elements" << std::endl;

	for (size_t i = 0; i < count; ++i)
		input_aligned[i] = rand() % 42;

	uint64_t t0;
	uint64_t t1;

	std::cout << "Running function #" << alt << std::endl;

	switch (alt) {
	case 0: // foo
	{
		t0 = timer_nsec();

		for (size_t i = 0; i < count; i += 8)
			foo(*reinterpret_cast< float(*)[8] >(input_aligned + i));

		t1 = timer_nsec();

		const size_t err = verify(count, input_aligned);
		if (-1 != err)
			std::cerr << "error at " << err << std::endl;
	}
		break;

	case 1: // bar
	{
		t0 = timer_nsec();

		for (size_t i = 0; i < count; i += 8)
			bar(*reinterpret_cast< float(*)[8] >(input_aligned + i));

		t1 = timer_nsec();

		const size_t err = verify(count, input_aligned);
		if (-1 != err)
			std::cerr << "error at " << err << std::endl;
	}
		break;

	case 2: // insertion_sort
	{
		t0 = timer_nsec();

		for (size_t i = 0; i < count; i += 8)
			insertion_sort(*reinterpret_cast< float(*)[8] >(input_aligned + i));

		t1 = timer_nsec();

		const size_t err = verify(count, input_aligned);
		if (-1 != err)
			std::cerr << "error at " << err << std::endl;
	}
		break;
	}

	const double sec = double(t1 - t0) * 1e-9;
	std::cout << "elapsed time: " << sec << " s" << std::endl;

	free(input);

	test();

	return 0;
}
