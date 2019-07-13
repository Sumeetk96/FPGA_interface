`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/28/2019 07:01:49 PM
// Design Name: 
// Module Name: user_application
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


module user_application(
    input [31:0] data_in,
    output reg [31:0] data_out,
    output reg [8:0] read_address,
    output reg [8:0] write_address,
    output reg [3:0] rd_we,
    output reg rd_wren,
    input clock,
    output reg wr_rden,
    input wr_wren,
    input rd_rden
    );
    
    initial begin
    read_address <= -1;
    write_address <= -2;
    rd_wren <= 0;
    wr_rden <= 0;
    rd_we <= 0;
    data_out<=0;
    end
    
    always @ (negedge clock)
    begin
        if(!wr_wren && !rd_rden)
        begin
    
            
                read_address<=read_address + 1;
                write_address <= write_address + 1;
                data_out <= data_in;
                rd_wren <= 1;
                rd_we <= 4'hF;
                wr_rden <= 1;
                
                
            end
        
     
        else
        begin
            read_address <= -1;
            write_address <= -2;
            rd_wren <= 0;
            rd_we <= 4'h0;
            wr_rden<=0;
        end    
    end
    
    
endmodule
