module test( out1, in1, in2, in3 );

  output  out1;
  input   in1, in2, in3;

  wire and1, and2, and3, and4;

  assign and1 = in1   & in2;
  assign and2 = and1  & in3;
  assign and3 = in2   & in3;
  assign and4 = and3  & in1;
  assign out1 = and2  & and4;

endmodule
