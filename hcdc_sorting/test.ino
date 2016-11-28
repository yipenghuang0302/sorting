//#include <float.h>
//
//unsigned char problem_size = 2;
//float dynamic_range = 3.00;
//
//void test () {
//
//  randomSeed(0);
//
//  float ** rhs_u = (float **) malloc ((problem_size + 2) * sizeof(float *));
//  float ** rhs_v = (float **) malloc ((problem_size + 2) * sizeof(float *));
//
//  float ** u_next = (float **) malloc ((problem_size + 2) * sizeof(float *));
//  float ** v_next = (float **) malloc ((problem_size + 2) * sizeof(float *));
//  float ** u_curr = (float **) malloc ((problem_size + 2) * sizeof(float *));
//  float ** v_curr = (float **) malloc ((problem_size + 2) * sizeof(float *));
//
//  for (unsigned char row = 0; row < problem_size + 2; row++) {
//
//    rhs_u[row] = (float *) malloc ((problem_size + 2) * sizeof(float));
//    rhs_v[row] = (float *) malloc ((problem_size + 2) * sizeof(float));
//
//    u_next[row] = (float *) malloc ((problem_size + 2) * sizeof(float));
//    v_next[row] = (float *) malloc ((problem_size + 2) * sizeof(float));
//    u_curr[row] = (float *) malloc ((problem_size + 2) * sizeof(float));
//    v_curr[row] = (float *) malloc ((problem_size + 2) * sizeof(float));
//
//    for (unsigned char col = 0; col < problem_size + 2; col++) {
//
//      rhs_u[row][col] = -dynamic_range + (float)(rand()) / (float)(RAND_MAX / (dynamic_range - (-dynamic_range)));
//      rhs_v[row][col] = -dynamic_range + (float)(rand()) / (float)(RAND_MAX / (dynamic_range - (-dynamic_range)));
//
//      // Choose an initial guess x^{(0)} to the solution
//      u_next[row][col] = 0.0;
//      v_next[row][col] = 0.0;
//      u_curr[row][col] = 0.0;
//      v_curr[row][col] = 0.0;
//
//    }
//
//  }
//  Serial.println ("\nrhs_u = ");
//  print_sq_mat ( problem_size + 2, rhs_u );
//  Serial.println ("\nrhs_v = ");
//  print_sq_mat ( problem_size + 2, rhs_v );
//
//  float ** rhs_u_newton = (float **) malloc ((2 + 2) * sizeof(float *));
//  float ** rhs_v_newton = (float **) malloc ((2 + 2) * sizeof(float *));
//  float ** u_temp = (float **) malloc ((2 + 2) * sizeof(float *));
//  float ** v_temp = (float **) malloc ((2 + 2) * sizeof(float *));
//  float ** u_gold = (float **) malloc ((2 + 2) * sizeof(float *));
//  float ** v_gold = (float **) malloc ((2 + 2) * sizeof(float *));
//  for (unsigned char row = 0; row < 2 + 2; row++) {
//    rhs_u_newton[row] = (float *) malloc ((2 + 2) * sizeof(float));
//    rhs_v_newton[row] = (float *) malloc ((2 + 2) * sizeof(float));
//    u_temp[row] = (float *) malloc ((2 + 2) * sizeof(float));
//    v_temp[row] = (float *) malloc ((2 + 2) * sizeof(float));
//    u_gold[row] = (float *) malloc ((2 + 2) * sizeof(float));
//    v_gold[row] = (float *) malloc ((2 + 2) * sizeof(float));
//  }
//
//  // while convergence not reached do
//  int sor_iters = 0;
//  float jacobi_error = FLT_MAX;
//  float limit = 0.06125;
//
//  while ( jacobi_error > limit ) {
//
//    // solve newton subproblems
//    unsigned char jacobi_row = 1;
//    while (jacobi_row < problem_size + 2 - 2) {
//      unsigned char jacobi_col = 1;
//      while (jacobi_col < problem_size + 2 - 2) {
//
//        // if j != i then \sigma += a_{ij} x_j^{(k)}
//        // copy rhs function
//        // copy u_temp v_temp
//        for (unsigned char row = 0; row < 2 + 2; row++) {
//          for (unsigned char col = 0; col < 2 + 2; col++) {
//            rhs_u_newton[row][col] = rhs_u[jacobi_row + row - 1][jacobi_col + col - 1] / dynamic_range;
//            rhs_v_newton[row][col] = rhs_v[jacobi_row + row - 1][jacobi_col + col - 1] / dynamic_range;
//            u_temp[row][col] = u_curr[jacobi_row + row - 1][jacobi_col + col - 1] / dynamic_range;
//            v_temp[row][col] = v_curr[jacobi_row + row - 1][jacobi_col + col - 1] / dynamic_range;
//            u_gold[row][col] = u_curr[jacobi_row + row - 1][jacobi_col + col - 1] / dynamic_range;
//            v_gold[row][col] = v_curr[jacobi_row + row - 1][jacobi_col + col - 1] / dynamic_range;
//          }
//        }
//        Serial.println ("\nrhs_u_newton = ");
//        print_sq_mat ( 2 + 2, rhs_u_newton );
//        Serial.println ("\nrhs_v_newton = ");
//        print_sq_mat ( 2 + 2, rhs_v_newton );
//
//        // TODO: incorporate results using sor
//        // x_i^{(k+1)} = {{\left( {b_i - \sigma } \right)} \over {a_{ii} }}
//        newtonHCDC (
//          rhs_u_newton,
//          rhs_v_newton,
//          u_temp,
//          v_temp,
//          dynamic_range
//        );
//
//        newtonGold (
//          rhs_u_newton,
//          rhs_v_newton,
//          u_gold,
//          v_gold,
//          2,
//          dynamic_range,
//          0.5,
//          FLT_EPSILON
//        );
//
//        Serial.println ("\nu_gold = ");
//        print_sq_mat ( 2 + 2, u_gold );
//        Serial.println ("\nv_gold = ");
//        print_sq_mat ( 2 + 2, v_gold );
//
//        float hcdc_error =
//          two_norm_squared (
//            problem_size + 2,
//            u_temp,
//            u_gold
//          ) + two_norm_squared (
//            problem_size + 2,
//            v_temp,
//            v_gold
//          );
//
//        Serial.println ("\nu_temp = ");
//        print_sq_mat ( 2 + 2, u_temp );
//        Serial.println ("\nv_temp = ");
//        print_sq_mat ( 2 + 2, v_temp );
//
//        Serial.print ("\nhcdc_error = ");
//        Serial.println ( hcdc_error, 6 );
//
//        // copy rhs function
//        for (unsigned char row = 0; row < 2; row++) {
//          for (unsigned char col = 0; col < 2; col++) {
//            u_next[jacobi_row + row][jacobi_col + col] = u_temp[row + 1][col + 1] * dynamic_range;
//            v_next[jacobi_row + row][jacobi_col + col] = v_temp[row + 1][col + 1] * dynamic_range;
//          }
//        }
//
//        jacobi_col += 2;
//      }
//      jacobi_row += 2;
//    }
//    sor_iters += 1;
//
//    // check if convergence is reached
//    jacobi_error =
//      two_norm_squared (
//        problem_size + 2,
//        u_next,
//        u_curr
//      ) + two_norm_squared (
//        problem_size + 2,
//        v_next,
//        v_curr
//      );
//
//    for (unsigned char row = 0; row < problem_size + 2; row++) {
//      for (unsigned char col = 0; col < problem_size + 2; col++) {
//        u_curr[row][col] = u_next[row][col];
//        v_curr[row][col] = v_next[row][col];
//      }
//    }
//    Serial.println ("\nu_curr = ");
//    print_sq_mat ( problem_size + 2, u_curr );
//    Serial.println ("\nv_curr = ");
//    print_sq_mat ( problem_size + 2, v_curr );
//
//  }
//
//  for (unsigned char row = 0; row < 2 + 2; row++) {
//    free ( rhs_u_newton[row] );
//    free ( rhs_v_newton[row] );
//    free ( u_temp[row] );
//    free ( v_temp[row] );
//    free ( u_gold[row] );
//    free ( v_gold[row] );
//  }
//  free(rhs_u_newton);
//  free(rhs_v_newton);
//  free(u_temp);
//  free(v_temp);
//  free(u_gold);
//  free(v_gold);
//
//  float * b = (float *) malloc ( 2 * problem_size * problem_size * sizeof(float) );
//  burgersFunction (
//    rhs_u, // (size+2) * (size+2), combines rhs function from previous time step and boundary conditions
//    rhs_v, // (size+2) * (size+2), combines rhs function from previous time step and boundary conditions
//    u_curr, // (size+2) * (size+2), initial guess
//    v_curr, // (size+2) * (size+2), initial guess
//    problem_size,
//    1.0,
//    b // the function
//  );
//  Serial.println ("\nFINAL EVALUATED FUNCTION = ");
//  print_vec ( 2 * problem_size * problem_size, b );
//  free (b);
//
//  for (unsigned char row = 0; row < problem_size + 2; row++) {
//    free ( rhs_u[row] );
//    free ( rhs_v[row] );
//    free ( u_next[row] );
//    free ( v_next[row] );
//    free ( u_curr[row] );
//    free ( v_curr[row] );
//
//  }
//  free (rhs_u);
//  free (rhs_v);
//  free (u_next);
//  free (v_next);
//  free (u_curr);
//  free (v_curr);
//
//}
