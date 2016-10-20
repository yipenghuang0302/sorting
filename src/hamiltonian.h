#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <boost/numeric/odeint/stepper/symplectic_rkn_sb3a_mclachlan.hpp>
#include <boost/numeric/odeint/integrate/integrate_const.hpp>

typedef std::vector< double > container_type;

// symplectic_euler
// symplectic_rkn_sb3a_mclachlan
// symplectic_rkn_sb3a_m4_mclachlan
typedef boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan< container_type > stepper_type;

struct momentum {
    // system function defining the ODE
    void operator()( const container_type &q , container_type &dpdt ) const {
        size_t n = q.size();
        for( size_t i=1 ; i<n-1 ; i++ ) {
            dpdt[i] = exp(q[i-1]-q[i]) - exp(q[i]-q[i+1]);
        }
    }
};

struct streaming_observer
{
    std::ostream& m_out;
    size_t m_write_every;
    size_t m_count;

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