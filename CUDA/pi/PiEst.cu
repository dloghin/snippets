/* Pi estimation using Monte Carlo simulation
 * implemented using XorShift rng
 *
 * Dumi Loghin, 2015-2017
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

typedef float sFloat;
typedef int64_t s64Int;

#define MaxCudaBlocks			2
#define MaxCudaThreadsBlock 		512
//#define MaxCudaBlocks			8
//#define MaxCudaThreadsBlock 		1024


#define SIZE 100000

#define MAX64 0x7FFFFFFFFFFFFFFF

__host__ __device__ s64Int xorshift64star(s64Int x) {
	x ^= x >> 12; 	// a
	x ^= x << 25; 	// b
	x ^= x >> 27; 	// c
	return x * UINT64_C(2685821657736338717);
}

__global__ void pikernel(s64Int samples, s64Int* nIn, s64Int* nOut) {
	s64Int i;	
	sFloat xx, yy;
	s64Int ni = 0;	// points inside the circle
	s64Int no = 0;	// points outside the circle

	int tid = blockIdx.x * blockDim.x + threadIdx.x;

	// initialize the random generator (non-zero)
	s64Int offset = tid * samples;
	s64Int x = 12345678 + offset * 89482311;
	s64Int y = 87654321 + offset + 12345678;

	// generate points and count
	for(i = 0; i < samples; i++) {
		x = xorshift64star(x);
		y = xorshift64star(y);
		xx = ((sFloat)x)/(sFloat)MAX64;
		yy = ((sFloat)y)/(sFloat)MAX64;
		if (xx * xx + yy * yy > 1.0)
			no++;
		else
			ni++;
	}

	// update output
	nIn[tid] = ni;
	nOut[tid] = no;
}

unsigned long long int get_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (unsigned long long int)(tv.tv_usec + tv.tv_sec * 1000000);
}

int main(int argc, char** argv) {
	int src_line;
	cudaError_t rc;
	s64Int size = SIZE, i, sIn = 0, sOut = 0;
	s64Int *dIn, *dOut;
	long long int t1, t2;

	printf("Using %d blocks and %d threads per block\n", MaxCudaBlocks, MaxCudaThreadsBlock);

	// alloc memory using unified memory
	src_line = __LINE__;
	rc = cudaMallocManaged((void **)&dIn, MaxCudaBlocks * MaxCudaThreadsBlock * sizeof(s64Int));
	if (rc != cudaSuccess)
		goto lbl_err;
	src_line = __LINE__;
	rc = cudaMallocManaged((void **)&dOut, MaxCudaBlocks * MaxCudaThreadsBlock * sizeof(s64Int));
	if (rc != cudaSuccess)
		goto lbl_err;

	// call kernel
	t1 = get_time();
	pikernel<<<MaxCudaBlocks,MaxCudaThreadsBlock>>>(size, dIn, dOut);
	src_line = __LINE__;
	rc = cudaDeviceSynchronize();
	t2 = get_time();
	if (rc != cudaSuccess)
                goto lbl_err;
	
	// compute pi
	for (i = 0; i < MaxCudaBlocks * MaxCudaThreadsBlock; i++) {
		sIn += dIn[i];
		sOut += dOut[i];
	}

	cudaFree(dIn);
	cudaFree(dOut);

	printf("Estimated value of Pi is %3.6f\n", 4.0 * sIn / (sIn + sOut));
	printf("Estimation took %lld usec\n", t2-t1);

	return 0;

lbl_err:
    	printf("Launch CUDA error %s al line %d\n", cudaGetErrorString(rc), src_line);
	return -1;
}
