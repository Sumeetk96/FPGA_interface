`timescale 1ns / 1ps

module spi_slave (rstb,ten,ss,sck,sdin,tdata, sdout,rdata,done);
input rstb,ss,sck,sdin,ten;
input [7:0] tdata;
output sdout;           //slave out   master in 
output reg done;
output reg [7:0] rdata;
reg [7:0] treg,rreg;
reg [3:0] nb;
wire sout;
reg mlb;
initial begin
mlb<=1;
end

assign sout=mlb?treg[7]:treg[0];
assign sdout=( (!ss)&&ten )?sout:1'bz; //if 1=> send data  else TRI-STATE sdout


//read from  sdout
always @(posedge sck or negedge rstb)
begin
if (rstb==0)
    begin rreg = 8'h00;  rdata = 8'h00; done = 0; nb = 0; end   //
else if (!ss) begin 
        if(mlb==0)  //LSB first, in@msb -> right shift
            begin rreg ={sdin,rreg[7:1]}; end
        else     //MSB first, in@lsb -> left shift
            begin rreg ={rreg[6:0],sdin}; end  
    //increment bit count
        nb=nb+1;
        if(nb!=8) done=0;
        else  begin rdata=rreg; done=1; nb=0; end
    end     //if(!ss)_END  if(nb==8)
end

//send to  sdout
always @(negedge sck or negedge rstb)
begin
if (rstb==0)
    begin treg = 8'hFF; end
else begin
    if(!ss) begin            
        if(nb==0) treg=tdata;
        else begin
           if(mlb==0)  //LSB first, out=lsb -> right shift
                begin treg = {1'b1,treg[7:1]}; end
           else     //MSB first, out=msb -> left shift
                begin treg = {treg[6:0],1'b1}; end            
        end
    end //!ss
 end //rstb    
end //always

endmodule