module test( out1, in1, in2, in3, in4, in5, in6, in7, in8 );

  output  out1;
  input   in1, in2, in3, in4, in5, in6, in7, in8;

  wire and1, and2, and3, and4, and5, and6;

  assign and1 = in1 & in2;
  assign and2 = in3 & in4;
  assign and3 = in5 & in6;
  assign and4 = in7 & in8;
  assign and5 = and1 & and2;
  assign and6 = and3 & and4;
  assign out1 = and5 & and6;

endmodule
