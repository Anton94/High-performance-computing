#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>


#define NOMINMAX
#include <Windows.h>

		 static uint64_t timer_nsec() {

	static LARGE_INTEGER freq;
	static BOOL once = QueryPerformanceFrequency(&freq);

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	return 1000000000ULL * t.QuadPart / freq.QuadPart;
}

__inline static void foo(float(&inout)[8]) {

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


__inline static void bar(float(&inout)[8]) 
{
	static __m128 first;
	static __m128 second;
	static __m128 cmp1;
	static __m128 cmp2;
	static __m128 res1;
	static __m128 res2;
	static __m128 temp;
	static __m128 res3;
	static __m128 res4;
	static float result1[4];
	static float result2[4];

	const size_t idx[][2] = {
			{ 0, 1 }, { 3, 2 }, { 4, 5 }, { 7, 6 },
			{ 0, 2 }, { 1, 3 }, { 6, 4 }, { 7, 5 },
			{ 0, 1 }, { 2, 3 }, { 5, 4 }, { 7, 6 },
			{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
			{ 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
			{ 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 }
	};

	for (int i = 0; i < 24; i += 4)
	{
		first = _mm_set_ps(inout[idx[i + 3][0]], inout[idx[i + 2][0]], inout[idx[i + 1][0]], inout[idx[i][0]]);
		second = _mm_set_ps(inout[idx[i + 3][1]], inout[idx[i + 2][1]], inout[idx[i + 1][1]], inout[idx[i][1]]);

		cmp1 = _mm_cmpge_ps(first, second);
		cmp2 = _mm_cmpgt_ps(second, first);

		res1 = _mm_and_ps(second, cmp1);
		res2 = _mm_and_ps(first, cmp2);

		res3 = _mm_and_ps(first, cmp1);
		res4 = _mm_and_ps(second, cmp2);

		first = _mm_or_ps(res1, res2);
		second = _mm_or_ps(res3, res4);

		// put them on the positions
		_mm_storeu_ps(result1, first);
		_mm_storeu_ps(result2, second);

		for (int j = 0; j < 4; ++j)
		{
			inout[idx[i + j][0]] = result1[j];
			inout[idx[i + j][1]] = result2[j];
		}
	}
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

int main(int argc, char** argv) {

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
	float* const input = (float*)malloc(sizeof(float) * count + 63);
	float* const input_aligned = reinterpret_cast< float* >(uintptr_t(input) + 63 & -64);

	std::cout << std::hex << std::setw(8) << input << " (" << input_aligned << ") : " << std::dec << count << " elements" << std::endl;

	for (size_t i = 0; i < count; ++i)
		input_aligned[i] = rand() % 42;

	uint64_t t0;
	uint64_t t1;

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
	return 0;
}