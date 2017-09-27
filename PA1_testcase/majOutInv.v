module maj( out, in1, in2, in3 );

  output out;

  input in1, in2, in3;

  assign out = ~( in1 & in2 | in2 & in3 | in1 & in3 );

endmodule
