#define _DUE
#include <HCDC_DEMO_API.h>

#include "XSlice.h"
#include "YSlice.h"
#include "SortingTile.h"

Fabric * fabric;

void setup() {

  // instantiate HCDC fabric
  fabric = new Fabric();
  fabric->calibrate();

  // check memory use
  Serial.print ("dynamic_memory = ");
  Serial.println (dynamic_memory);

  XSlice x0 (fabric->chips[0].tiles[1].slices[0], 128);
  YSlice y0 (fabric->chips[0].tiles[1].slices[1], 192);
  XSlice x1 (fabric->chips[0].tiles[1].slices[2], 128);

  //    x0.y_in_prev_pos;
  //    x0.y_in_prev_neg;

  Fabric::Chip::Connection ( y0.y_out_curr_pos0, x0.y_in_curr_pos0 ).setConn();
  Fabric::Chip::Connection ( y0.y_out_curr_pos1, x0.y_in_curr_pos1 ).setConn();

  Fabric::Chip::Connection ( x1.x_out_prev, y0.x_in_next ).setConn();
  Fabric::Chip::Connection ( x0.x_out_curr, y0.x_in_curr ).setConn();

  Fabric::Chip::Connection ( y0.y_out_next_pos, x1.y_in_prev_pos ).setConn();
  Fabric::Chip::Connection ( y0.y_out_next_neg, x1.y_in_prev_neg ).setConn();

  //    x1.y_in_curr_pos0;
  //    x1.y_in_curr_pos1;

  Fabric::Chip::Connection ( x0.x_out_prev, x0.slice.tileOuts[0].in0 ).setConn();
  Fabric::Chip::Connection ( x0.slice.tileOuts[0].out0, fabric->chips[0].tiles[3].slices[2].chipOutput->in0 ).setConn();
  Fabric::Chip::Connection ( x1.x_out_curr, x1.slice.tileOuts[0].in0 ).setConn();
  Fabric::Chip::Connection ( x1.slice.tileOuts[0].out0, fabric->chips[0].tiles[3].slices[3].chipOutput->in0 ).setConn();

  // cells = new NewtonTile[8] {
  //   NewtonTile ( fabric->chips[0].tiles[0], 1.0, 128, 128, 5.0, 0.0, 0.0, true, true, true ),
  //   NewtonTile ( fabric->chips[0].tiles[1], 1.0, 128, 128, 5.0, 0.0, 0.0, true, false, true ),
  //   NewtonTile ( fabric->chips[0].tiles[2], 1.0, 128, 128, 5.0, 0.0, 0.0, true, true, false ),
  //   NewtonTile ( fabric->chips[0].tiles[3], 1.0, 128, 128, 5.0, 0.0, 0.0, true, false, false ),
  //   NewtonTile ( fabric->chips[1].tiles[0], 1.0, 128, 128, 5.0, 0.0, 0.0, false, true, true ),
  //   NewtonTile ( fabric->chips[1].tiles[1], 1.0, 128, 128, 5.0, 0.0, 0.0, false, false, true ),
  //   NewtonTile ( fabric->chips[1].tiles[2], 1.0, 128, 128, 5.0, 0.0, 0.0, false, true, false ),
  //   NewtonTile ( fabric->chips[1].tiles[3], 1.0, 128, 128, 5.0, 0.0, 0.0, false, false, false )
  // };

  Serial.print ("dynamic_memory = ");
  Serial.println (dynamic_memory);
  Serial.println("done");
  Serial.flush();

  // calculate function evaluation
  // delete[] cells;
  while (true) {

    for (short x0Init = 0; x0Init < 256; x0Init += 32) {
      for (short x1Init = 0; x1Init < 256; x1Init += 32) {
        x0.setInitial(x0Init);
        x1.setInitial(x1Init);
        fabric->serialOutReq();

        fabric->cfgCommit();
        fabric->execStart();
        //        delay(1);
        short x0Final = x0.slice.adc->readHelper();
        fabric->execStop();

        fabric->cfgCommit();
        fabric->execStart();
        //        delay(1);
        short x1Final = x1.slice.adc->readHelper();
        fabric->execStop();

        Serial.print("x0Init=");
        Serial.print(x0Init);
        Serial.print(" x1Init=");
        Serial.print(x1Init);
        Serial.print(" x0Final=");
        Serial.print(x0Final);
        Serial.print(" x1Final=");
        Serial.println(x1Final);

        fabric->serialOutStop();
      }
    }
  }
}

void loop () {
  //  test();

  // float f[8];
  // f[0] = u[0] * (u[1] + coeff_parallel[0]) + u[4] * (u[2] - coeff_ortho[0]) - u[1] - u[2] - rhs[0];
  // f[1] = u[1] * (coeff_parallel[1] - u[0]) + u[5] * (u[3] - coeff_ortho[1]) - u[0] - u[3] - rhs[1];
  // f[2] = u[2] * (u[3] + coeff_parallel[2]) + u[6] * (coeff_ortho[2] - u[0]) - u[3] - u[0] - rhs[2];
  // f[3] = u[3] * (coeff_parallel[3] - u[2]) + u[7] * (coeff_ortho[3] - u[1]) - u[2] - u[1] - rhs[3];
  // f[4] = u[4] * (u[6] + coeff_parallel[4]) + u[0] * (u[5] - coeff_ortho[4]) - u[5] - u[6] - rhs[4];
  // f[5] = u[5] * (u[7] + coeff_parallel[5]) + u[1] * (coeff_ortho[5] - u[4]) - u[4] - u[7] - rhs[5];
  // f[6] = u[6] * (coeff_parallel[6] - u[4]) + u[2] * (u[7] - coeff_ortho[6]) - u[7] - u[4] - rhs[6];
  // f[7] = u[7] * (coeff_parallel[7] - u[5]) + u[3] * (coeff_ortho[7] - u[6]) - u[6] - u[5] - rhs[7];

  // Serial.println("");
  // for (unsigned char cellIndx = 0; cellIndx < 8; cellIndx++) {
  //   Serial.print(f[cellIndx]);
  //   Serial.print(", ");
  // }
  // Serial.println("");
  // fabric->execStop();
}
