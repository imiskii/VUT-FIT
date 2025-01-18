/**
 * @file BatchMandelCalculator.cc
 * @author Michal Ľaš <xlasmi00@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date 23.10.2024
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <stdexcept>

#include <immintrin.h>	// _mm_malloc()
#include <cstring>	    // memcpy()

#include "BatchMandelCalculator.h"

BatchMandelCalculator::BatchMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "BatchMandelCalculator")
{
	data = (int*)(_mm_malloc(height * width * sizeof(int), 64));
	rBuffer = (float*)(_mm_malloc(width * sizeof(float), 64));
	iBuffer = (float*)(_mm_malloc(width * sizeof(float), 64));
}

BatchMandelCalculator::~BatchMandelCalculator() {
	_mm_free(data);
	_mm_free(rBuffer);
	_mm_free(iBuffer);
	data = nullptr;
	rBuffer = nullptr;
	iBuffer = nullptr;
}


int * BatchMandelCalculator::calculateMandelbrot () {

	// initialize data (there can also be loop over the data with #pragma omp simd, but the efficiency is the same)
	std::memset(data, 0, height * width * sizeof(int));

	// Choose block size
	const int blockSize = 64; // tried: 512, 256, 128, 64, 32, 16 (64 was the best, it is size of cache line)

	// Iterate rows (due to symmetricity just half of the rows, the second half will be mem-copied)
	for (int i = 0; i < height / 2; ++i){
		int *pdata = data + width * i;
		float y = y_start + i * dy; // current imaginary value
		
		// Iterate blocks in the row
		for (int blockN = 0; blockN < width / blockSize; ++blockN){
			
			// Iterate limits for each block
			for (int l = 0; l < limit; ++l){
	
				int limitCnt = 0;

				// Iterate elements in the block
				#pragma omp simd reduction(+:limitCnt)
				for (int j = 0; j < blockSize; ++j){
					
					int jGlobal = blockN * blockSize + j;

					float x = x_start + jGlobal * dx; // current real value

					float zReal = (l == 0) ? x : rBuffer[jGlobal];
					float zImag = (l == 0) ? y : iBuffer[jGlobal];

					// Calculate limit
					float r2 = zReal * zReal;
					float i2 = zImag * zImag;

					// Calculate the condition
					int cond = (r2 + i2) >= 4.0f;
					pdata[jGlobal] += !cond;
					limitCnt += cond;
					
					// Update values in buffers
					!cond && (rBuffer[jGlobal] = (r2 - i2 + x));
					!cond && (iBuffer[jGlobal] = (2.0f * zReal * zImag + y));
				}

				// Stop if the row is fully computed
				if (limitCnt >= blockSize) break;
			}
		}
		// Copy the row to next half of the image
		std::memcpy(data + (height-i-1) * width, data + (i * width), width * sizeof(int));
	}
	return data;
}
