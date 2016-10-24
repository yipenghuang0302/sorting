#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <boost/numeric/odeint/stepper/symplectic_rkn_sb3a_mclachlan.hpp>
#include <boost/numeric/odeint/integrate/integrate_const.hpp>

// ODE function object
struct momentum {
	// system function defining the ODE
	// time derivative of momentum is a function of position
	void operator()( const container_type &q , container_type &dpdt ) const {
		size_t n = q.size();
		for( size_t i=1 ; i<n-1 ; i++ ) {
			dpdt[i] = exp(q[i-1]-q[i]) - exp(q[i]-q[i+1]);
		}
	}
};

// provides printout of ODE
struct streaming_observer
{
	std::ostream& m_out;
	size_t m_write_every;
	size_t m_count;

	// constructor
	streaming_observer(
		std::ostream &out ,
		size_t write_every = 16
	) :
		m_out( out ) ,
		m_write_every( write_every ) ,
		m_count( 0 )
	{}

	template< class State >
	void operator()( const State &x , double t )
	{
		if( ( m_count % m_write_every ) == 0 )
		{
			container_type &q = x.first;
			container_type &p = x.second;
			for( size_t i=0 ; i<q.size() ; i++ )
			{
				m_out << t << "\t" << i << "\t" << q[i] << "\t" << p[i] << "\n";
			}
			m_out << "\n";
		}
		++m_count;
	}
};