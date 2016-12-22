// dx_n/d_t = 2 (y_n*y_n - y_{n-1}*y_{n-1})
// dx_n/d_t = 2 (y_n+y_{n-1}) (y_n-y_{n-1})

// 1 integrator
// 1 multiplier
// 2 fanout
class XSlice {

  public:
    XSlice (
      Fabric::Chip::Tile::Slice & slice,
      unsigned char initial // initial condition for integrator
    ) :
      slice ( slice ),
      // input
      y_in_prev_pos ( slice.muls[0].in0 ),
      y_in_prev_neg ( slice.muls[0].in1 ),
      y_in_curr_pos0 ( slice.muls[0].in0 ),
      y_in_curr_pos1 ( slice.muls[0].in1 ),
      // connections
      conn0 ( slice.integrator->out0, slice.fans[0].in0 ),
      conn1 ( slice.muls[0].out0, slice.fans[1].in0 ),
      conn2 ( slice.fans[1].out0, slice.integrator->in0 ),
      conn3 ( slice.fans[1].out1, slice.integrator->in0 ),
      // output
      x_out_prev ( slice.fans[0].out0 ),
      x_out_curr ( slice.fans[0].out1 ),
      conn4 ( slice.fans[0].out2, slice.adc->in0 )
    {
      slice.integrator->setInitial(initial);
      slice.fans[0].out1->setInv(true);
      conn0.setConn();
      conn1.setConn();
      conn2.setConn();
      conn3.setConn();
      conn4.setConn();
      Serial.println("x ctor");
    }

    ~XSlice () {
      slice.integrator->setEnable(false);
      slice.fans[0].out1->setInv(false);
      slice.fans[0].setEnable(false);
      slice.fans[1].setEnable(false);
      slice.muls[0].setEnable(false);
      conn0.brkConn();
      conn1.brkConn();
      conn2.brkConn();
      conn3.brkConn();
      conn4.setConn();
      Serial.println("x dtor");
    }

    void setInitial (
      unsigned char initial
    ) {
      slice.integrator->setInitial(initial);
    }

    Fabric::Chip::Tile::Slice & slice;

    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_in_prev_pos;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_in_prev_neg;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_in_curr_pos0;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_in_curr_pos1;

    Fabric::Chip::Connection conn0;
    Fabric::Chip::Connection conn1;
    Fabric::Chip::Connection conn2;
    Fabric::Chip::Connection conn3;
    Fabric::Chip::Connection conn4;

    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * x_out_prev;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * x_out_curr;
};
