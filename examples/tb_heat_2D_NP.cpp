/*
 **********************************************************************************
 *  Copyright (C) 2010-2011  Massachusetts Institute of Technology
 *  Copyright (C) 2010-2011  Yuan Tang <yuantang@csail.mit.edu>
 * 		                     Charles E. Leiserson <cel@mit.edu>
 * 	 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Suggestsions:                  yuantang@csail.mit.edu
 *   Bugs:                          yuantang@csail.mit.edu
 *
 *********************************************************************************
 */

/* Test bench - 2D heat equation, Non-periodic version */
#include <cstdio>
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <cmath>

#include <pochoir.hpp>

using namespace std;
#define TIMES 20
#define N_RANK 2
#define TOLERANCE (1e-6)

void check_result(int t, int j, int i, double a, double b)
{
	if (abs(a - b) < TOLERANCE) {
//		printf("a(%d, %d, %d) == b(%d, %d, %d) == %f : passed!\n", t, j, i, t, j, i, a);
	} else {
		printf("a(%d, %d, %d) = %f, b(%d, %d, %d) = %f : FAILED!\n", t, j, i, a, t, j, i, b);
	}

}

Pochoir_Boundary_2D(heat_bv_2D, arr, t, i, j)
    return 0;
Pochoir_Boundary_End

int main(int argc, char * argv[])
{
	const int BASE = 1024;
	int t;
	struct timeval start, end;
    double min_tdiff = INF;
    int N_SIZE = 0, T_SIZE = 0;

    if (argc < 3) {
        printf("argc < 3, quit! \n");
        exit(1);
    }
    N_SIZE = StrToInt(argv[1]);
    T_SIZE = StrToInt(argv[2]);
    printf("N_SIZE = %d, T_SIZE = %d\n", N_SIZE, T_SIZE);
    Pochoir_Shape_2D heat_shape_2D[] = {{1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, -1, -1}, {0, 0, -1}, {0, 0, 1}, {0, 0, 0}};
	Pochoir_Array_2D(double) a(N_SIZE, N_SIZE);
    Pochoir_2D heat_2D(heat_shape_2D);

    Pochoir_Kernel_2D(heat_2D_fn, t, i, j)
	   a(t+1, i, j) = 0.125 * (a(t, i+1, j) - 2.0 * a(t, i, j) + a(t, i-1, j)) + 0.125 * (a(t, i, j+1) - 2.0 * a(t, i, j) + a(t, i, j-1)) + a(t, i, j);
    Pochoir_Kernel_End

    a.Register_Boundary(heat_bv_2D);
    heat_2D.Register_Array(a);

	for (int i = 0; i < N_SIZE; ++i) {
	for (int j = 0; j < N_SIZE; ++j) {
        a(0, i, j) = 1.0 * (rand() % BASE); 
        a(1, i, j) = 0;
	} }

    for (int times = 0; times < TIMES; ++times) {
	    gettimeofday(&start, 0);
        heat_2D.Run(T_SIZE, heat_2D_fn);
	    gettimeofday(&end, 0);
        min_tdiff = min(min_tdiff, 1.0e3 *((end.tv_sec - start.tv_sec)
						+ (end.tv_usec - start.tv_usec) / 1000000.0f));
    }
	std::cout << "Pochoir ET: consumed time : " << min_tdiff << " ms" << std::endl;

#if 0
    min_tdiff = INF;
    /* cilk_for + zero-padding */
    for (int times = 0; times < TIMES; ++times) {
	gettimeofday(&start, 0);
	for (int t = 0; t < T_SIZE; ++t) {
    cilk_for (int i = 1; i < N_SIZE+1; ++i) {
	for (int j = 1; j < N_SIZE+1; ++j) {
       b.interior(t+1, i, j) = 0.125 * (b.interior(t, i+1, j) - 2.0 * b.interior(t, i, j) + b.interior(t, i-1, j)) + 0.125 * (b.interior(t, i, j+1) - 2.0 * b.interior(t, i, j) + b.interior(t, i, j-1)) + b.interior(t, i, j); 
    } } }
	gettimeofday(&end, 0);
    min_tdiff = min(min_tdiff, (1.0e3 * tdiff(&end, &start)));
    }
	std::cout << "Naive Loop: consumed time :" << min_tdiff << "ms" << std::endl;

	t = T_SIZE;
	for (int i = 0; i < N_SIZE; ++i) {
	for (int j = 0; j < N_SIZE; ++j) {
		check_result(t, i, j, a.interior(t, i, j), b.interior(t, i+1, j+1));
	} } 
#endif
	return 0;
}
