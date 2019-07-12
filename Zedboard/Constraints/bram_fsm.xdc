
create_clock -period 50 [get_ports {clk}];

create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports {appclk}];

    
set_property PACKAGE_PIN Y9         [get_ports {appclk}];
set_property IOSTANDARD LVCMOS33    [get_ports {appclk}];


#set_property IOSTANDARD LVCMOS33    [get_ports {done}];
#set_property PACKAGE_PIN AB10 [get_ports {done}];  # "LD0"

#set_property IOSTANDARD LVCMOS33    [get_ports {rdata[6]}];
#set_property PACKAGE_PIN U19 [get_ports {rdata[6]}];  # "LD0"

#set_property IOSTANDARD LVCMOS33    [get_ports {rdata[5]}];
#set_property PACKAGE_PIN W22 [get_ports {rdata[5]}];  # "LD0"

#set_property IOSTANDARD LVCMOS33    [get_ports {rdata[4]}];
#set_property PACKAGE_PIN V22 [get_ports {rdata[4]}];  # "LD0"

set_property IOSTANDARD LVCMOS33    [get_ports {state[3]}];
set_property PACKAGE_PIN U21 [get_ports {state[3]}];  # "LD0"

set_property IOSTANDARD LVCMOS33    [get_ports {state[2]}];
set_property PACKAGE_PIN U22 [get_ports {state[2]}];  # "LD0"

set_property IOSTANDARD LVCMOS33    [get_ports {state[1]}];
set_property PACKAGE_PIN T21 [get_ports {state[1]}];  # "LD0"

set_property IOSTANDARD LVCMOS33    [get_ports {state[0]}];
set_property PACKAGE_PIN T22 [get_ports {state[0]}];  # "LD0"

set_property PACKAGE_PIN AA9         [get_ports {clk}];
set_property IOSTANDARD LVCMOS33    [get_ports {clk}];

set_property PACKAGE_PIN Y10         [get_ports {sdout}];
set_property IOSTANDARD LVCMOS33    [get_ports {sdout}];

set_property PACKAGE_PIN AA11        [get_ports {sdin}];
set_property IOSTANDARD LVCMOS33    [get_ports {sdin}];

set_property PACKAGE_PIN Y11        [get_ports {ss}];
set_property IOSTANDARD LVCMOS33    [get_ports {ss}];

set_property PACKAGE_PIN AB11        [get_ports {rst_spi}];
set_property IOSTANDARD LVCMOS33    [get_ports {rst_spi}];

set_property PACKAGE_PIN AB10        [get_ports {rst_fsm}];
set_property IOSTANDARD LVCMOS33    [get_ports {rst_fsm}];



