#include "hamiltonian.h"

// range of items to sort
double big_rng = 256.0;

// range of off-diagonals
double sml_rng = 0.0;

int main( int argc , char **argv ) {

	srand (time(NULL));

	if (argc!=2) {
		std::cout << "usage: ./test <N>\n";
		exit(0);
	}
	const size_t n = atoi(argv[1]) + 2;

	// randomize on CPU side
	host_type q_host( n , 0.0 ) , p_host( n , 0.0 );
	q_host[0] = -HUGE_VAL; // INFINITY available in c++11
	for( size_t i=1 ; i<n-1 ; i++ )
	{
		q_host[i] = i*2.0;
		p_host[i] = -big_rng + (float)(rand()) / (float)(RAND_MAX/(big_rng-(-big_rng)));
	}
	q_host[n-1] = HUGE_VAL; // yuge

	// copy to GPU side
	device_type p_device = p_host;
	device_type q_device = q_host;

	thrust::copy( p_device.begin() , p_device.end() , std::ostream_iterator< double >( std::cout , "\n" ) );
	std::cout << std::endl;

	const double dt = 0.0625/16.0;
	integrate_const(
		stepper_type() ,
		momentum( n ) ,
		// std::make_pair( coord() , momentum() ) ,
		std::make_pair( boost::ref( q_device ) , boost::ref( p_device ) ) ,
		0.0 ,
		16.0 ,
		dt
		, streaming_observer( std::cout , 16*16 )
	);

	thrust::copy( p_device.begin() , p_device.end() , std::ostream_iterator< double >( std::cout , "\n" ) );
	std::cout << std::endl;

	return 0;
}