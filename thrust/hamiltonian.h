#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <boost/numeric/odeint/stepper/symplectic_rkn_sb3a_mclachlan.hpp>
#include <boost/numeric/odeint/integrate/integrate_const.hpp>
#include <boost/numeric/odeint/external/thrust/thrust.hpp>

typedef std::vector< double > host_type;
typedef thrust::device_vector< size_t > index_vector_type;
typedef thrust::device_vector< double > device_type;
// symplectic_euler
// symplectic_rkn_sb3a_mclachlan
// symplectic_rkn_sb3a_m4_mclachlan
typedef boost::numeric::odeint::symplectic_rkn_sb3a_mclachlan< device_type > stepper_type;

class momentum {

public:
    struct sys_functor {
        template< class Tuple >
        __host__ __device__
        void operator()( Tuple t )  // this functor works on tuples of values
        {
            // first, unpack the tuple into value, neighbors
            const double q = thrust::get<0>(t);
            const double q_l = thrust::get<1>(t);  // left neighbor
            const double q_r = thrust::get<2>(t); // right neighbor
            // the dynamical equation
            thrust::get<3>(t) = exp( q_l - q ) - exp( q - q_r );
        }
    };

    momentum ( const size_t n ) :
        m_N( n ) ,
        m_prev( n ) ,
        m_next( n )
    {
        // build indices pointing to left and right neighbours
        thrust::counting_iterator<size_t> c( 0 );
        thrust::copy( c , c+m_N-1 , m_prev.begin()+1 );
        m_prev[0] = 0; // m_prev = { 0 , 0 , 1 , 2 , 3 , ... , N-1 }

        thrust::copy( c+1 , c+m_N , m_next.begin() );
        m_next[m_N-1] = m_N-1; // m_next = { 1 , 2 , 3 , ... , N-1 , N-1 }
    }

    // system function defining the ODE
    void operator() ( const device_type &q , device_type &dpdt /*, const double dt*/ )
    {
        thrust::for_each(
            // where to start
            thrust::make_zip_iterator(
                    thrust::make_tuple(
                            q.begin()+1 ,
                            thrust::make_permutation_iterator( q.begin() , m_prev.begin()+1 ) ,
                            thrust::make_permutation_iterator( q.begin() , m_next.begin()+1 ) ,
                            dpdt.begin()+1
                            ) ),
            // where to stop
            thrust::make_zip_iterator(
                    thrust::make_tuple(
                            q.end()-1 ,
                            thrust::make_permutation_iterator( q.begin() , m_prev.end()-1 ) ,
                            thrust::make_permutation_iterator( q.begin() , m_next.end()-1 ) ,
                            dpdt.end()-1
                            ) ) ,
            sys_functor()
        );
    }

private:

    const size_t m_N;
    index_vector_type m_prev;
    index_vector_type m_next;
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
            device_type &q = x.first;
            device_type &p = x.second;
            for( size_t i=0 ; i<q.size() ; i++ )
            {
                m_out << t << "\t" << i << "\t" << q[i] << "\t" << p[i] << "\n";
            }
            m_out << "\n";
        }
        ++m_count;
    }
};