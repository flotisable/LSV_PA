module test( out1, out2, in1, in2, in3 );

  output  out1, out2;
  input   in1, in2, in3;

  wire and1, and2;

  assign and1 = in1   & in2;
  assign out1 = and1  & in3;
  assign and2 = in2   & in3;
  assign out2 = and2  & in1;

endmodule
