#include <stdio.h>
// #include <papi.h> 
#include "hamiltonian.h"

double rms ( size_t n, container_type a, container_type b ) {
	double sum = 0.0;
	for ( size_t idx=0; idx<n; idx++ )
		sum += (a[idx]-b[idx]) * (a[idx]-b[idx]);
	return sqrt(sum/n);
}

int main() {

	srand (time(NULL));

	for (double dt=1/256.0; 1.0/256.0<=dt; dt=dt/2.0) {
		// dynamic range of real numbers we are sorting
		for (double range=2.0; range<=2.0; range*=2.0) {
			// range of off-diagonals
			for (double interval=16.0; interval<=16.0; interval*=2.0) {

				// number of reals to sort
				// we pad with two boundary elements at front and back
				for (size_t n=2; n<=256; n*=2) {
					for (size_t trial=0; trial<=4; trial++) {

						// q is position in Hamiltonian system; in sorting system they are related to off-diagonals
						// p is momentum in Hamiltonian system; in sorting system they are related to the set to sort
						container_type q( n , 0.0 ) , p_sort( n , 0.0 ), p_curr( n , 0.0 ), p_prev( n, 0.0 );
						q[0] = -HUGE_VAL; // q_0 is initialized to negative infinity
						for( size_t i=1 ; i<n-1 ; i++ )
						{
							q[i] = i*interval; // we choose this to ensure that the off-diagonals are small
							p_curr[i] = p_sort[i] = -range + (float)(rand()) / (float)(RAND_MAX/(range-(-range)));
						}
						q[n-1] = HUGE_VAL; // q_0 is initialized to positive infinity

						/* Setup PAPI library and begin collecting data from the counters */
						int retval;
						float real_time, proc_time_start, proc_time_stop, mflops;
						long long flpins_start, flpins_stop;
						// if((retval=PAPI_flops( &real_time, &proc_time_start, &flpins_start, &mflops))<PAPI_OK)
						// 	printf ("PAPI_flops");
						// golden model sort
						std::sort (p_sort.begin()+1, p_sort.end()-1);
						// if((retval=PAPI_flops( &real_time, &proc_time_stop, &flpins_stop, &mflops))<PAPI_OK)
						// 	printf ("PAPI_flops");
						float proc_time = proc_time_stop - proc_time_start;
						long long flpins = flpins_stop - flpins_start;

						// we use a symplectic ODE solver (likely based on Verlet integration)
						// symplectic solvers are ideal for Hamiltonian systems
						// http://www.boost.org/doc/libs/1_57_0/libs/numeric/odeint/doc/html/boost_numeric_odeint/concepts/simple_symplectic_system.html

						boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan< container_type > stepper;
						// other symplectic integrator in Boost library include:
						// symplectic_euler
						// symplectic_rkn_sb3a_mclachlan
						// symplectic_rkn_sb3a_m4_mclachlan

						// for( size_t idx=0 ; idx<n ; idx++ ) {
						// 	printf("initial p[%ld] =\t%e\tinitial q[%ld] =\t%e\n", idx, p_curr[idx], idx, q[idx]);
						// }
						// for( size_t idx=0 ; idx<n ; idx++ ) {
						// 	printf("sorted p[%ld] =\t%e\tsorted q[%ld] =\t%e\n", idx, p_sort[idx], idx, q[idx]);
						// }

						double time = 0.0;
						double error = DBL_MAX;
						do {
							p_prev = p_curr;
							stepper.do_step (
								momentum(),
								std::make_pair( boost::ref( q ) , boost::ref( p_curr ) ) ,
								time,
								dt  // time step
								// streaming_observer( std::cout , 16*16*16 )
							);
							time += dt;

							error = rms(n, p_prev, p_curr);
							// printf("error =\t%e\n", error);

						} while ( DBL_EPSILON<error );

						double quality = rms(n, p_sort, p_curr);

						// for( size_t idx=0 ; idx<n ; idx++ ) {
						// 	printf("final p[%ld] =\t%e\tfinal q[%ld] =\t%e\n", idx, p_curr[idx], idx, q[idx]);
						// }

						printf("dt =\t%e\trange =\t%e\tinterval =\t%e\tn =\t%ld\tproc_time =\t%e\tflpins =\t%lld\ttime =\t%e\tquality = \t%e\n", dt, range, interval, n, proc_time, flpins, time, quality);
					}
				}
			}
		}
	}	
	return 0;
}