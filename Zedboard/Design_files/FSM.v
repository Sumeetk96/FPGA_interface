`timescale 1ns / 1ps


module FSM(
    input clk,
    input [7:0] rreg,
    output reg [11:0] rdaddr,
    output reg [11:0] wraddr,
    output reg [7:0] data,
    output reg we,wren,rden,regce,
    output reg reset_out,
    input reset_in,
    output reg [3:0] stateop,
    output reg ten
    );
    reg [3:0] next_state;
    reg [3:0] state;
    reg [15:0] count;
    reg [15:0] current_count;
    reg [7:0] temp_data;
    reg [11:0] address = 10'h00;
   
    parameter Start_Byte1 = 8'h19;
    parameter Start_Byte2 = 8'h1e;
    parameter Stop_Byte = 8'h1c;
   
    parameter st0 = 4'b0000;
    parameter st1 = 4'b0001;
    parameter st2 = 4'b0010;
    parameter st3 = 4'b0011;
    parameter st4 = 4'b0100;
    parameter st5 = 4'b0101;
    parameter st6 = 4'b0110;
    parameter st7 = 4'b0111;
    parameter st8 = 4'b1000; 
    parameter st9 = 4'b1001; 
    parameter st10 = 4'b1010;
    parameter st11= 4'b1011;
    parameter st12 = 4'b1100;
        
    initial
    begin
       state <= 4'b0000;
       next_state <= 4'b0000;
        reset_out <= 1'b0;
        regce <= 1'b0;
        rdaddr <= 8'h01;
        data<=8'h0;
        temp_data<=0;
        wraddr <= 0;
       // en = 1'b1;
    end
   
    always @(negedge clk)   //always loop to change state
    begin
        if(!reset_in)
        state = st0;
        else
        state = next_state;
     end
     
     always @ (state)   //always loop to compute output
     begin 
     //state<=st0;
     case(state)
        st0: begin
                stateop <= 4'hf; 
             end
        st1: begin
                stateop <= 4'h1;    
             end
        st2: begin
                stateop <= 4'h2;         
             end
        st3: begin
                stateop<= 4'h3; 
             end
        st4: begin
                stateop <= 4'h4;            
             end
        st5: begin        
                stateop <= 4'h5;                    
             end
        st6: begin
                stateop <= 4'h6;            
             end
        st7: begin 
                stateop<= 4'h7;              
             end
         
        st8: begin
                stateop<=4'h8;
             end
        st9: begin
                stateop<=4'h9;
             end
        st10: begin
               stateop<=4'hA;
              end
        st11: begin
               stateop<=4'hb;
              end
        st12: begin
               stateop <= 4'hc;
              end
        default: begin            
                   stateop<=0;
                 end

      endcase
   end

   
   
   
    always @ (state or rreg or current_count or count)   //always loop to compute next state
     begin
     next_state <= st0;
     case(state)
         st0:
         begin
            if(rreg == Start_Byte1)
            begin
                next_state  <= st1;
                rden<=0;
                ten<=0;
                we<=0;
                wren<=0;
            end
            else
            begin
                next_state  <= st0;
                rden<=0;
                ten<=0;
                we<=0;
                wren<=0;
                //wraddr<=0;
            end
         end
         
         st1:
         begin
            if(rreg == Start_Byte2)
            begin
            
                 next_state  <= st7;
                 rden<=0;
                 ten<=0;
                 we<=0;
                 wren<=0;
                //wraddr<=0;
            end     
            else if(rreg == Start_Byte1)
            begin
                 next_state  <= st1;
                 rden<=0;
                 ten<=0;
                 we<=0;
                 wren<=0;
               //  wraddr<=0;
            end
            else 
            begin
                 next_state <= st0;
                 rden<=0;
                 ten<=0;
                 we<=0;
                 wren<=0;
             end
          end
          st7:
          begin 
                  next_state <= st2;
                  rden<=0;
                  ten<=0;
                  we<=0;
                  wren<=0;
          end

         st2:
         begin
            if(rreg[0] == 1)
            begin
                next_state <= st4;//wraddr<=1;
                rden<=0;
                ten<=0;
                we<=0;
                wren<=0;
            end
          else if  (rreg[0] == 0)
          begin
            next_state <= st8;
            rden<=0;
            ten<=0;
            we<=0;
            wren<=0;
          end
          else 
          begin 
            next_state<=st0;
            rden<=0;
            ten<=0;
            we<=0;
            wren<=0;
          end
         end
          
         st3: 
         begin
            next_state <= st4;
            rden<=0;
            ten<=0;
            we<=0;
            wren<=0;
         end
         st4:
         begin
            next_state <= st5;
            we<=1;
            wren<=1;
            rden<=0;
            ten<=0;
         end
         st5: 
         begin
            if(rreg == Stop_Byte)
            begin
                next_state <= st0;
                we<=0;
                wren<=0;
                rden<=0;
                ten<=0;
           end
           else
           begin
                next_state <= st6;
                we<=1;
                wren<=1;
                rden<=0;
                ten<=0;
           end
         end
         st6:
         begin 
            if(rreg == Stop_Byte)
            begin
                next_state <= st0;
                we<=0;
                wren<=0;
                rden<=0;
                ten<=0;
             
            end
            else 
            begin
                next_state <= st6;
                we<=1;
                wren<=1;
                rden<=0;
                ten<=0;
            end
         end

          st8:begin 
                next_state<=st9;
                rden<=0;
                ten<=0;
                we<=0;
                wren<=0;
              end  
          st9:begin
                next_state<=st10;
                rden<=0;
                ten<=0;
                we<=0;
                wren<=0;
              end  
          st10:begin
                next_state<=st11;
                rden<=1;
                ten<=0;
                we<=0;
                wren<=0;
               end
          st11: 
          begin
          if(current_count==count)
          begin
                rden<=0;
                ten<=0;
                 we<=0;
                wren<=0;
                next_state<=st0;
          end
          else
                begin
                next_state<=st11;
                rden<=1;
                ten<=1;
                 we<=0;
               wren<=0;
                end
          end
         default: 
         begin
           next_state <= st0;
           we<=0;
           wren<=0;
           ten<=0;
           rden<=0;
           //wraddr<=0;
         end        
     endcase
   end

always @ (posedge clk)
begin
case(state)
         st0:
         begin 
         end
         
         st1:
         begin
            
         end
          st7: 
                   
            begin
              wraddr[11:7] <= rreg[4:0];
              rdaddr[11:7] <= rreg[4:0];
           end
         st2:
         begin
           wraddr[6:0]<=rreg[7:1];
           rdaddr[6:0]<=rreg[7:1];
          end

         st4:
         begin
          //wraddr<=wraddr>>1; 
          data<=rreg; 
          end
         st5: 
         begin
            wraddr<=wraddr+1;
            data<=rreg;
         end
         st6: 
        
         begin
            wraddr<=wraddr+1;
            data<=rreg;
         end
        
       
        st8:
        begin
          count[15:8]<=rreg;
          current_count<=0;
             //rdaddr<=rdaddr>>1;
        end
        
     
        st9:
        begin
         count[7:0] <= rreg;
          current_count <= 0;         
        end
        
        st10:
         begin
          rdaddr<=rdaddr+1;
         end
        st11:
        begin
          rdaddr<=rdaddr+1;
          current_count<=current_count+1;
        end
         default: 
         begin
           wraddr<=0;
           rdaddr<=0;
           current_count<=0;
         end        
     endcase
end
endmodule


     
    
   
   
   
