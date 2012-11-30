/* Copyright 2012 Dumi Loghin <dumiloghin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** 
 *  @file	prime_sieve.c
 *  @brief	The Sieve of Eratosthenes (Ciurul lui Eratostene)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX		2147483647	// (1<<31)-1
#define MAX_SQRT	46340		// sqrt((1<<31)-1)

/**
 * Find primes in range [2..n-1] and return their number.
 *
 * @param n - defines max range
 * @param primes - pointer to an integer array. If it is NULL, 
	the function returns only the number of primes
 * @return the number of primes
 */
int get_primes_classic(int n, int** primes)
{
	char* prime = (char*)malloc(n * sizeof(char));
	if (!prime) {
		printf("Error on allocating %d bytes!\n", n * sizeof(char));
		return -1;
	}
	memset(prime, 0, n);

	int nr = 0;
	int i = 2;
	int j;

	while (i<n) {
		if (prime[i]) {
			i++;
			continue;
		}
		nr++;
		if (i >= MAX_SQRT) {
			i++;
			continue;
		}

		for (j = i*i; j<n; j+=i)
			prime[j] = 1;
		i++;
	}

	if (primes != NULL) {
		int* list = (int*)malloc(nr * sizeof(int));
		j = 0;
		for (i=2; i<n; i++)
			if (!prime[i]) {
				list[j++] = i;
			}
		*primes = list;
	}

	free(prime);

	return nr;
}

/**
 * Find primes in range [2..n-1] and return their number.
 * Uses a smaller boolean array by assigning only one bit per number. 
 * 
 * @param n - defines max range
 * @param primes - pointer to an integer array. If it is NULL, 
	the function returns only the number of primes
 * @return the number of primes
 */
int get_primes(int n, int** primes)
{
	int bits = 8 * sizeof(char);
	int mem = n / bits;
	char* prime = (char*)malloc(mem * sizeof(char));
	if (!prime) {
		printf("Error on allocating %d bytes!\n", mem * sizeof(char));
		return -1;
	}
	memset(prime, 0, mem);

	int nr = 0;
	int i = 2;
	int j;
	int idx_prime, idx_mask;

	while (i<n) {
		idx_prime = i/bits;
		idx_mask = i%bits;	
		if (prime[idx_prime] & (1<<idx_mask)) {
			i++;
			continue;
		}
		nr++;
		if (i >= MAX_SQRT) {
			i++;
			continue;
		}

		for (j = i*i; j<n; j+=i)
			prime[j/bits] |= 1<<(j%bits);
		i++;
	}

	if (primes != NULL) {
		int* list = (int*)malloc(nr * sizeof(int));
		j = 0;
		for (i=2; i<n; i++)
			if ((prime[i/bits] & (1<<(i%bits))) == 0) {
				list[j++] = i;
			}
		*primes = list;
	}

	free(prime);

	return nr;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s N (where N>1 and N<2^32)\n", argv[0]);
		return -1;
	}
	int n = atoi(argv[1]);
	if (n<2 || n>MAX) {
		printf("Bad N: %d (choose N>1 and N<2^32)\n", n);
		return -1;
	}

#ifdef PRINT_PRIMES
	int i;
	int* list;	
	n = get_primes_classic(n, &list);	 
	printf("# %d\n", n);
	for (i=0; i<n; i++) 
		printf("%d ", list[i]);
	free(list);
#else
	n = get_primes_classic(n, NULL);	 
	printf("# %d\n", n);
#endif
	
	return 0;
}

