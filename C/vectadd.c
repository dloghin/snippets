/* Copyright 2013 Dumi Loghin <dumiloghin@gmail.com>
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
 *  @file	vectadd.c
 *  @brief	Add two dynamically allocated vectors
 */

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int main() {
	int *a, *b, *c;
	int i, n = (1<<24);

	a = (int*)malloc(n * sizeof(int));
	b = (int*)malloc(n * sizeof(int));
	c = (int*)malloc(n * sizeof(int));
	if (!a || !b || !c) {
		fprintf(stderr, "Error allocating arrays!\n");
		return -1;
	}

#pragma omp parallel for private(i) shared (a, b, n)
	for (i=0; i<n; i++) {
		a[i] = rand() % 10;
		b[i] = rand() % 10;
	}

#pragma omp parallel for private(i) shared (a, b, c, n)
	for (i=0; i<n; i++)
		c[i] = a[i] + b[i];

	free(a);
	free(b);
	free(c);

	printf("Done.\n");

	return 0;	 
}
