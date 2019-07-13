`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/31/2019 11:50:55 AM
// Design Name: 
// Module Name: bram
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

//BRAM instansiation at bram.v:71
//FSM definition at bram.v:191

module top(
input appclk,clk,rst_spi,sdin,ss,rst_fsm,
output sdout,
output [3:0] state
);


wire reset;
wire regce;
wire done;

wire [7:0] rreg;
wire ten;

wire [7:0] wr_di;
wire [31:0] wr_do;
wire [7:0] rd_di;
wire [7:0] rd_do;

wire wr_rden;
wire rd_rden;
wire wr_wren;
wire rd_wren;

wire wr_we;
wire rd_we;

wire [11:0] wr_wraddr;
wire [9:0] wr_rdaddr;
wire [11:0] rd_wraddr;
wire [11:0] rd_rdaddr;


bram bram1(
    .DO(wr_do),
    .DI(wr_di),
    .RDADDR(wr_rdaddr),
    .WRADDR(wr_wraddr),
    .REGCE(regce),
    .RST(reset) , 
    .RDCLK(clk),
    .WRCLK(clk),
    .RDEN(wr_rden),
    .WREN(wr_wren),
    .WE(wr_we)
    );
    
    

 FSM controller(
 .clk(done),
 .rdaddr(rd_rdaddr),
 .wraddr(wr_wraddr),
 .data(wr_di),
 .we(wr_we),
 .wren(wr_wren),
 .rden(rd_rden),
 .regce(regce),
 .reset_out(reset),
 .reset_in(rst_fsm),
 .rreg(rreg),
 .stateop(state),
 .ten(ten)
 );
 
 spi_slave slave1(
 .rdata(rreg),
 .done(done),
 .sck(clk),
 .rstb(rst_spi),
 .tdata(rd_do),
 .ten(ten),
 .sdout(sdout),
 .ss(ss),
 .sdin(sdin)
 );
//spi_slave (rstb,ten,ss,sck,sdin,tdata, sdout,rdata,done);
bram_rd bram2(
    .DO(rd_do),
    .DI(wr_di),
    .RDADDR(rd_rdaddr),
    .WRADDR(wr_wraddr),
    .REGCE(regce),
    .WE(wr_we),
    .RST(reset) , 
    .RDCLK(clk),
    .WRCLK(clk),
    .RDEN(rd_rden),
    .WREN(wr_wren)
    );
    
    user_application app1(
        .data_in(wr_do),
        .data_out(rd_di),
        .read_address(wr_rdaddr),
        .write_address(rd_wraddr),
        .rd_we(rd_we),
        .rd_wren(rd_wren),
        .clock(appclk),
        .wr_rden(wr_rden),
        .wr_wren(wr_wren),
        .rd_rden(rd_rden)
        );
        
        
endmodule




    
