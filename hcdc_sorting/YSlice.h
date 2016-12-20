// dy_n/d_t = -(x_n-x_{n+1})y_n
// dy_n/d_t = (-x_n+x_{n+1})y_n

// 1 integrator
// 1 multiplier
// 1 fanout
class YSlice {

  public:
    YSlice (
      Fabric::Chip::Tile::Slice & slice,
      unsigned char initial // initial condition for integrator
    ) :
      slice ( slice ),
      // input
      x_in_curr ( slice.muls[0].in0 ),
      x_in_next ( slice.muls[0].in0 ),
      // connections
      conn0 ( slice.integrator->out0, slice.fans[0].in0 ),
      conn1 ( slice.fans[0].out0, slice.fans[1].in0 ),
      conn2 ( slice.fans[1].out0, slice.muls[0].in1 ),
      conn3 ( slice.muls[0].out0, slice.integrator->in0 ),
      // output
      y_out_curr_pos0 ( slice.fans[0].out1 ),
      y_out_curr_pos1 ( slice.fans[0].out2 ),
      y_out_next_pos ( slice.fans[1].out1 ),
      y_out_next_neg ( slice.fans[1].out2 )
    {
      slice.integrator->setInitial(initial);
      slice.fans[1].out2->setInv(true);
      conn0.setConn();
      conn1.setConn();
      conn2.setConn();
      conn3.setConn();
      Serial.println("y ctor");
    };

    ~YSlice () {
      slice.integrator->setEnable(false);
      slice.fans[0].setThird(false);
      slice.fans[0].setEnable(false);
      slice.fans[1].out2->setInv(false);
      slice.fans[1].setThird(false);
      slice.fans[1].setEnable(false);
      slice.muls[0].setEnable(false);
      conn0.brkConn();
      conn1.brkConn();
      conn2.brkConn();
      conn3.brkConn();
      Serial.println("y dtor");
    };

    void setInitial (
      unsigned char initial
    ) {
      slice.integrator->setInitial(initial);
    }

    Fabric::Chip::Tile::Slice & slice;

    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * x_in_curr;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * x_in_next;

    Fabric::Chip::Connection conn0;
    Fabric::Chip::Connection conn1;
    Fabric::Chip::Connection conn2;
    Fabric::Chip::Connection conn3;

    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_out_curr_pos0;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_out_curr_pos1;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_out_next_pos;
    Fabric::Chip::Tile::Slice::FunctionUnit::Interface * y_out_next_neg;
};
