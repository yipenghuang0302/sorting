#include "hamiltonian.h"

// dynamic range of real numbers we are sorting
double big_rng = 256.0;

int main( int argc , char **argv ) {

	srand (time(NULL));

	// get command line parameters
	if (argc!=2) {
		std::cout << "usage: ./test <N>\n";
		exit(0);
	}
	// user chooses number of reals to sort
	// we pad with two boundary elements at front and back
	const size_t n = atoi(argv[1]) + 2;

	// q is position in Hamiltonian system; in sorting system they are related to off-diagonals
	// p is momentum in Hamiltonian system; in sorting system they are related to the set to sort
	container_type q( n , 0.0 ) , p( n , 0.0 );
	q[0] = -HUGE_VAL; // q_0 is initialized to negative infinity
	for( size_t i=1 ; i<n-1 ; i++ )
	{
		q[i] = i*2.0; // we choose this to ensure that the off-diagonals are small
		p[i] = -big_rng + (float)(rand()) / (float)(RAND_MAX/(big_rng-(-big_rng)));
	}
	q[n-1] = HUGE_VAL; // q_0 is initialized to positive infinity

	// time step for ODE solver
	const double dt = 0.0625/16.0;

	// we use a symplectic ODE solver (likely based on Verlet integration)
	// symplectic solvers are ideal for Hamiltonian systems
	// http://www.boost.org/doc/libs/1_57_0/libs/numeric/odeint/doc/html/boost_numeric_odeint/concepts/simple_symplectic_system.html

	typedef boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan< container_type > stepper_type;
	// other symplectic integrator in Boost library include:
	// symplectic_euler
	// symplectic_rkn_sb3a_mclachlan
	// symplectic_rkn_sb3a_m4_mclachlan

	integrate_const(
		stepper_type() , // choice of integrator
        momentum() , // Hamiltonian system is defined by the function that describes change of momentum
        // std::make_pair( coord() , momentum() ) ,
		std::make_pair( boost::ref( q ) , boost::ref( p ) ) ,
		0.0 , // initial time
		16.0 , // final time
		dt , // time step
		streaming_observer( std::cout , 16*16*16 )
	);

	return 0;
}