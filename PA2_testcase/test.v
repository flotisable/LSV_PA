module test( out1, out2, in1, in2, in3, in4 );

  output  out1, out2;
  input   in1, in2, in3, in4;

  assign out1 = in1 & in2;
  assign out2 = in3 & in4;

endmodule
