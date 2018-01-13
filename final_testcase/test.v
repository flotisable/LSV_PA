module test();

input i1,i2,i3;
output o1;

assign o1 = (i1 & i2) | (~i1 & i3);
endmodule
