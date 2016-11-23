#include "hamiltonian.h"
#include <thrust/sort.h>

template <typename T> struct square {
	__host__ __device__
	T operator()(const T& x) const { 
		return x * x;
	}
};

double rms ( size_t n, device_type a, device_type b ) {
	double sum = 0.0;
	for ( size_t idx=0; idx<n; idx++ )
		sum += (a[idx]-b[idx]) * (a[idx]-b[idx]);
	return sqrt(sum/n);
}

int main( int argc , char **argv ) {

	srand (time(NULL));

	// dynamic range of real numbers we are sorting
	for (double dt=1.0/256.0; 1.0/256.0<=dt; dt=dt/2.0) {
		// range of off-diagonals
		for (double range=2.0; range<=2.0; range*=2.0) {
			for (double interval=16.0; interval<=16.0; interval*=2.0) {

				// number of reals to sort
				// we pad with two boundary elements at front and back
				for (size_t n=16; n<=16; n*=2) {
					for (size_t trial=0; trial<=0; trial++) {

						// q is position in Hamiltonian system; in sorting system they are related to off-diagonals
						// p is momentum in Hamiltonian system; in sorting system they are related to the set to sort
						device_type q( n , 0.0 ) , p_sort( n , 0.0 ), p_curr( n , 0.0 ), p_prev( n, 0.0 );
						q[0] = -HUGE_VAL; // q_0 is initialized to negative infinity
						for( size_t i=1 ; i<n-1 ; i++ )
						{
							q[i] = i*interval; // we choose this to ensure that the off-diagonals are small
							p_curr[i] = p_sort[i] = -range + (float)(rand()) / (float)(RAND_MAX/(range-(-range)));
						}
						q[n-1] = HUGE_VAL; // q_0 is initialized to positive infinity
						thrust::sort (p_sort.begin()+1, p_sort.end()-1);

						// copy to GPU side
						// device_type p_device = p_host;
						// device_type q_device = q_host;

						thrust::copy( p_curr.begin() , p_curr.end() , std::ostream_iterator< double >( std::cout , ", " ) );
						std::cout << std::endl;
						thrust::copy( p_sort.begin() , p_sort.end() , std::ostream_iterator< double >( std::cout , ", " ) );
						std::cout << std::endl;

						stepper_type stepper;
						double error = DBL_MAX;
						double time = 0.0;
						do {
							p_prev = p_curr;
							stepper.do_step (
								momentum(n),
								std::make_pair( boost::ref( q ) , boost::ref( p_curr ) ) ,
								time,
								dt  // time step
								// streaming_observer( std::cout , 16*16*16 )
							);
							time += dt;
							thrust::transform (
								p_curr.begin(),
								p_curr.end(),
								p_prev.begin(),
								p_prev.begin(),
								thrust::minus<double>()
							);
							square<float> unary_op;
							thrust::plus<double> binary_op;
							error = std::sqrt(
								thrust::transform_reduce(
									p_prev.begin(),
									p_prev.end(),
									unary_op,
									0.0,
									binary_op
								)/n
							);
							printf("error =\t%e\n", error);
						} while ( DBL_EPSILON<error );

						double quality = rms(n, p_sort, p_curr);

						thrust::copy( p_curr.begin() , p_curr.end() , std::ostream_iterator< double >( std::cout , ", " ) );
						std::cout << std::endl;

						printf("dt =\t%e\trange =\t%e\tinterval =\t%e\tn =\t%ld\ttime =\t%e\tquality = \t%e\n", dt, range, interval, n, time, quality);
					}
				}
			}
		}
	}	
	return 0;
}