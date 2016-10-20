#include "hamiltonian.h"

double big_rng = 256.0;
double sml_rng = 0.0;

int main( int argc , char **argv )
{
	srand (time(NULL));

	if (argc!=2) {
		std::cout << "usage: ./test <N>\n";
		exit(0);
	}
	const size_t n = atoi(argv[1]) + 2;

	container_type q( n , 0.0 ) , p( n , 0.0 );
	q[0] = -HUGE_VAL; // yuge
	for( size_t i=1 ; i<n-1 ; i++ )
	{
		q[i] = i*2.0;
		p[i] = -big_rng + (float)(rand()) / (float)(RAND_MAX/(big_rng-(-big_rng)));
	}
	q[n-1] = HUGE_VAL;

	const double dt = 0.0625/16.0;

	integrate_const(
		stepper_type() ,
        momentum() ,
        // std::make_pair( coord() , momentum() ) ,
		std::make_pair( boost::ref( q ) , boost::ref( p ) ) ,
		0.0 ,
		16.0 ,
		dt ,
		streaming_observer( std::cout , 16*16*16 )
	);

	return 0;
}