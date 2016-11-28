// east and west from u perspective
void parallelConnect (
  // NewtonTile * east,
  // NewtonTile * west
) {
  // Fabric::Chip::Connection ( east->u_out_parallel_coeff, west->u_in_parallel_coeff ).setConn();
  // Fabric::Chip::Connection ( east->u_out_parallel_const, west->u_in_parallel_const ).setConn();
  // Fabric::Chip::Connection ( east->u_out_jacob_parallel, west->u_in_jacob_parallel ).setConn();
  // Fabric::Chip::Connection ( east->quoti_out_parallel_coeff, west->quoti_in_parallel_coeff ).setConn();
  // Fabric::Chip::Connection ( east->quoti_out_parallel_const, west->quoti_in_parallel_const ).setConn();

  // Fabric::Chip::Connection ( west->u_out_parallel_coeff, east->u_in_parallel_coeff ).setConn();
  // Fabric::Chip::Connection ( west->u_out_parallel_const, east->u_in_parallel_const ).setConn();
  // Fabric::Chip::Connection ( west->u_out_jacob_parallel, east->u_in_jacob_parallel ).setConn();
  // Fabric::Chip::Connection ( west->quoti_out_parallel_coeff, east->quoti_in_parallel_coeff ).setConn();
  // Fabric::Chip::Connection ( west->quoti_out_parallel_const, east->quoti_in_parallel_const ).setConn();
  return;
}

// north and south from u perspective
void orthoConnect (
  // NewtonTile * north,
  // NewtonTile * south
) {
  // Fabric::Chip::Connection ( north->u_out_ortho_coeff, south->u_in_ortho_coeff ).setConn();
  // Fabric::Chip::Connection ( north->u_out_ortho_const, south->u_in_ortho_const ).setConn();
  // Fabric::Chip::Connection ( north->quoti_out_ortho_coeff, south->quoti_in_ortho_coeff ).setConn();
  // Fabric::Chip::Connection ( north->quoti_out_ortho_const, south->quoti_in_ortho_const ).setConn();

  // Fabric::Chip::Connection ( south->u_out_ortho_coeff, north->u_in_ortho_coeff ).setConn();
  // Fabric::Chip::Connection ( south->u_out_ortho_const, north->u_in_ortho_const ).setConn();
  // Fabric::Chip::Connection ( south->quoti_out_ortho_coeff, north->quoti_in_ortho_coeff ).setConn();
  // Fabric::Chip::Connection ( south->quoti_out_ortho_const, north->quoti_in_ortho_const ).setConn();
  return;
}
