`timescale 1ns / 1ps
module iic_com(
			clk,rst_n,
			sw1,
			scl,sda,
			vdd_en,
			clk_mclk,
			mclk
			,test_ack
			);

input clk;		// 25MHz
input rst_n;	//reset
input sw1;	//switch
output scl;		// scl
inout sda;		// sda
input clk_mclk;//25MHz
output reg vdd_en;
output mclk;
output reg test_ack;



//---------------------------------------------
reg[2:0] cnt;
reg[8:0] cnt_delay;
reg scl_r;	//scl register

always @ (posedge clk or negedge rst_n)
	if(!rst_n) cnt_delay <= 9'd0;
	else if(cnt_delay == 9'd499) cnt_delay <= 9'd0;
	else cnt_delay <= cnt_delay+1'b1;
	
always @ (posedge clk or negedge rst_n) begin
	if(!rst_n) cnt <= 3'd5;
	else begin
		case (cnt_delay)
			9'd124:	cnt <= 3'd1;
			9'd249:	cnt <= 3'd2;
			9'd374:	cnt <= 3'd3;
			9'd499:	cnt <= 3'd0;
			default: cnt <= 3'd5;
			endcase
		end
end


always @ (posedge clk or negedge rst_n)
	if(!rst_n) scl_r <= 1'b0;
	else if(cstate == DEV_ACK | UP_ADD_ACK | LOW_ADD_ACK | UP_DATA_ACK | LOW_DATA_ACK | IDLE) scl_r <= 1'b1;
	else if(cnt==3'd0) scl_r <= 1'b1;
   else if(cnt==3'd2) scl_r <= 1'b0;

assign scl = scl_r;	

//scl assigned

`define SCL_POS		(cnt==3'd0)	//posedge
`define SCL_HIG		(cnt==3'd1)	//high
`define SCL_NEG		(cnt==3'd2)	//negedge
`define SCL_LOW		(cnt==3'd3)	//low

//sda control

parameter 	IDLE	 		= 4'd0;
parameter 	START 			= 4'd1;
parameter 	DEV_ADD 		= 4'd2;
parameter 	DEV_ACK 		= 4'd3;
parameter 	UP_ADD	 		= 4'd4;
parameter 	UP_ADD_ACK 		= 4'd5;
parameter 	LOW_ADD 		= 4'd6;
parameter 	LOW_ADD_ACK 	= 4'd7;
parameter 	UP_DATA			= 4'd8;
parameter 	UP_DATA_ACK 	= 4'd9;
parameter 	LOW_DATA		= 4'd10;
parameter 	LOW_DATA_ACK 	= 4'd11;
parameter 	STOP1		 	= 4'd12;
parameter 	STOP2		 	= 4'd13;

//work flow state machine
parameter CAM_IDLE    = 2'b00;
parameter CAM_POWERUP = 2'b01;
parameter CAM_CONFIG  = 2'b10;
parameter CAM_STREAMING =2'b11;

reg[1:0] cam_state;
reg [30:0] cam_cnt;
reg mclkEnabled;

reg configDone=1'b0;
reg startTx;
reg init;

assign mclk=(mclkEnabled)?clk_mclk:1'b0;


//tx state machine
reg[3:0] cstate;
reg sda_r;//sda register
reg sda_link;//sda in out mux
reg[3:0] num;	//local counter

reg[7:0] upperAddr;
reg[7:0] lowerAddr;
reg[7:0] upperData;
reg[7:0] lowerData;

reg busy;	//bus busy
always@(posedge clk or negedge rst_n) begin

	if(!rst_n)begin
		cam_state<=CAM_IDLE;

		init<=1'b1;
		mclkEnabled<=1'b0;
		cam_cnt<=30'b0;
		
		configDone<=1'b0;		
		startTx<=1'b0;//start transmit internal signal
		test_ack<=1'b0;							

	end
	else begin
		case(cam_state)
			CAM_IDLE: begin
					mclkEnabled<=1'b0;
						cam_cnt<=30'b0;
					if(sw1==1&&init==1)begin
						cam_state<=CAM_POWERUP;
						vdd_en<=1'b1;
						init<=1'b0;
					end
					else begin
						cam_state<=CAM_IDLE;
						init<=1'b1;
						vdd_en<=1'b0;						
					end
				end
			CAM_POWERUP: begin
					if(cam_cnt==30'd3000)begin
						cam_state<=CAM_CONFIG;
						cam_cnt<=30'b0;
						mclkEnabled<=1'b1;
					end
					else begin
						cam_cnt<=cam_cnt+1'b1;
						cam_state<=CAM_POWERUP;
						case(cam_cnt)
							30'd0:	   mclkEnabled<=1'b0;
							30'd2047:  mclkEnabled<=1'b1;
							default:   mclkEnabled<=1'b0;
						endcase
					end
						
				end
			CAM_CONFIG: begin
					if(configDone==1'b1 && busy==1'b0)begin
						cam_state<=CAM_STREAMING;
						cam_cnt<=30'b0;
						startTx<=1'b0;
					end
					else begin
						cam_state<=CAM_CONFIG;
						if(busy==1'b1)startTx<=1'b0;
						else if(busy==1'b0 && startTx==1'b0)begin
							case(cam_cnt)
								30'd0:{upperAddr,lowerAddr,upperData,lowerData}<='h3386_0501;
								30'd1:{upperAddr,lowerAddr,upperData,lowerData}<='h3386_0500;
								30'd2:{upperAddr,lowerAddr,upperData,lowerData}<='h3214_0D85;
								30'd3:{upperAddr,lowerAddr,upperData,lowerData}<='h341E_8F0B;
								30'd4:{upperAddr,lowerAddr,upperData,lowerData}<='h3202_0008;
								//CONFIG variables
								30'd5:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2797;
								30'd6:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0030;//rgb
								30'd7:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2707;
								30'd8:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_00C8;//width
								30'd9:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2709;
								30'd10:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0097;//height
								30'd11:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_275F;
								30'd12:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0000;//x0
								30'd13:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2763;
								30'd14:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0000;//y0
								30'd15:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2761;
								30'd16:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_00C8;//x1
								30'd17:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2765;
								30'd18:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0096;//y1
								30'd19:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2741;
								30'd20:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0169;
								30'd21:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_A120;
								30'd22:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_00F2;
								30'd23:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_A103;
								30'd24:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0006;//
								30'd25:begin
								{upperAddr,lowerAddr,upperData,lowerData}<='h301A_02CC; 
								configDone<=1'b1;
								end
								default:{upperAddr,lowerAddr,upperData,lowerData}<='h1111_1111;
							endcase
//							case(cam_cnt)//WITH PLL
//								30'd0:{upperAddr,lowerAddr,upperData,lowerData}<='h3386_0501;
//								30'd1:{upperAddr,lowerAddr,upperData,lowerData}<='h3386_0500;//reset mcu
//								30'd2:{upperAddr,lowerAddr,upperData,lowerData}<='h3214_0D85;
//								30'd3:{upperAddr,lowerAddr,upperData,lowerData}<='h341E_8F0B;
//								30'd4:{upperAddr,lowerAddr,upperData,lowerData}<='h341C_0340;
//								30'd5:{upperAddr,lowerAddr,upperData,lowerData}<='h341E_8F09;
//								30'd6:{upperAddr,lowerAddr,upperData,lowerData}<='h341E_8F08;//PLL CONFIGURED
//								30'd7:{upperAddr,lowerAddr,upperData,lowerData}<='h3202_0008;
								//CONFIG variables
//								30'd8:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2797;
//								30'd9:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0030;//rgb
//								30'd10:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2707;
//								30'd11:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_00C8;//width
//								30'd12:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2709;
//								30'd13:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0097;//height
//								30'd14:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_275F;
//								30'd15:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0000;//x0
//								30'd16:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2763;
//								30'd17:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0000;//y0
//								30'd18:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2761;
//								30'd19:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_00C8;//x1
//								30'd20:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2765; 
//								30'd21:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0096;//y1
//								30'd22:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_2741;
//								30'd23:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0169;
//								30'd24:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_A120;
//								30'd25:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_00F2;
//								30'd8:{upperAddr,lowerAddr,upperData,lowerData}<='h338C_A103;
//								30'd9:{upperAddr,lowerAddr,upperData,lowerData}<='h3390_0000;//
//								30'd10:begin
////									{upperAddr,lowerAddr,upperData,lowerData}<='h301A_02CC;
//									configDone<=1'b1;
//									end
//								default:{upperAddr,lowerAddr,upperData,lowerData}<='hffff_ffff;
//							endcase

							cam_cnt<=cam_cnt+1'b1;
							startTx<=1'b1;					
						end
						
						end
					end
				CAM_STREAMING:begin 
					cam_state<=CAM_STREAMING;
//					mclkEnabled<=1'b1;
//					vdd_en<=1'b1;
					test_ack<=1'b1;
					
					
					end
			endcase
	end			
end

always @ (posedge clk or negedge rst_n) begin
	if(!rst_n) begin
			cstate <= IDLE;
			sda_r <= 1'b1;
			sda_link <= 1'b0;
			num <= 4'd0;
			busy<=1'b0;
//			test_ack<=1'b0;
		end
	else 	  
		case (cstate)
			IDLE:	begin
					sda_link <= 1'b0;			
					sda_r <= 1'b1;
					if(startTx) begin	
						cstate <= START;
						busy<=1'b1;
						end
					else begin
						cstate <= IDLE;
						busy<=1'b0;
					end
				end
			START: begin
					if(`SCL_HIG) begin		
						sda_link <= 1'b1;	//sda to be output
						sda_r <= 1'b0;		//pull down the sda signal
						cstate <= DEV_ADD;
						num <= 4'd0;		
						end
					else cstate <= START; //wait until scl is high
				end
			DEV_ADD:	begin
					if(`SCL_LOW) begin
							if(num == 4'd8) begin	
									num <= 4'd0; 			
									sda_r <= 1'b1;
									sda_link <= 1'b0;		//sda to be input
									cstate <= DEV_ACK;
								end
							else begin
									cstate <= DEV_ADD;
									num <= num+1'b1;
									case (num)
										4'd0: sda_r <= 1'b0;
										4'd1: sda_r <= 1'b1;
										4'd2: sda_r <= 1'b1;
										4'd3: sda_r <= 1'b1;
										4'd4: sda_r <= 1'b1;
										4'd5: sda_r <= 1'b0;
										4'd6: sda_r <= 1'b0;
										4'd7: sda_r <= 1'b0;
										default: ;
										endcase
								end
						end
					else cstate <= DEV_ADD;//wait until scl is low
				end
			DEV_ACK:	begin
					if(!sda/*`SCL_NEG*/) begin	
							cstate <= UP_ADD;	
						end
					else cstate <= DEV_ACK;		
				end
			UP_ADD:	begin
					if(`SCL_LOW) begin
							if(num==4'd8) begin	
									num <= 4'd0;			
									sda_r <= 1'b1;
									sda_link <= 1'b0;		
									cstate <= UP_ADD_ACK;
								end
							else begin
									sda_link <= 1'b1;		//sda to be output
									num <= num+1'b1;
									case (num)
										4'd0: sda_r <= upperAddr[7];
										4'd1: sda_r <= upperAddr[6];
										4'd2: sda_r <= upperAddr[5];
										4'd3: sda_r <= upperAddr[4];
										4'd4: sda_r <= upperAddr[3];
										4'd5: sda_r <= upperAddr[2];
										4'd6: sda_r <= upperAddr[1];
										4'd7: sda_r <= upperAddr[0];
										default: sda_r<=1'b1;
									endcase
									cstate <= UP_ADD;					
								end
						end
					else cstate <= UP_ADD;				
				end
				
			UP_ADD_ACK:	begin
					if(!sda/*`SCL_NEG*/) begin	
							cstate <= LOW_ADD;	
					end
					else cstate <= UP_ADD_ACK;	
				end
				
			LOW_ADD:	begin
					if(`SCL_LOW) begin
							if(num==4'd8) begin	
									num <= 4'd0;			
									sda_r <= 1'b1;
									sda_link <= 1'b0;		
									cstate <= LOW_ADD_ACK;
								end
							else begin
									sda_link <= 1'b1;		
									num <= num+1'b1;
									case (num)
										4'd0: sda_r <= lowerAddr[7];
										4'd1: sda_r <= lowerAddr[6];
										4'd2: sda_r <= lowerAddr[5];
										4'd3: sda_r <= lowerAddr[4];
										4'd4: sda_r <= lowerAddr[3];
										4'd5: sda_r <= lowerAddr[2];
										4'd6: sda_r <= lowerAddr[1];
										4'd7: sda_r <= lowerAddr[0];
										default: sda_r<=1'b1;
									endcase
									cstate <= LOW_ADD;					
								end
						end
					else cstate <= LOW_ADD;				
				end
				
			LOW_ADD_ACK:	begin
					if(!sda/*`SCL_NEG*/) begin	
							cstate <= UP_DATA;	
							end
					else cstate <= LOW_ADD_ACK;	
				end
				
			UP_DATA:	begin
					if(`SCL_LOW) begin
							if(num==4'd8) begin	
									num <= 4'd0;			
									sda_r <= 1'b1;
									sda_link <= 1'b0;		
									cstate <= UP_DATA_ACK;
								end
							else begin
									sda_link <= 1'b1;		
									num <= num+1'b1;
									case (num)
										4'd0: sda_r <= upperData[7];
										4'd1: sda_r <= upperData[6];
										4'd2: sda_r <= upperData[5];
										4'd3: sda_r <= upperData[4];
										4'd4: sda_r <= upperData[3];
										4'd5: sda_r <= upperData[2];
										4'd6: sda_r <= upperData[1];
										4'd7: sda_r <= upperData[0];
										default: sda_r<=1'b1;
									endcase
									cstate <= UP_DATA;					
								end
						end
					else cstate <= UP_DATA;				
				end
				
			UP_DATA_ACK:	begin
					if(!sda/*`SCL_NEG*/) begin	
							cstate <= LOW_DATA;	
						end
					else cstate <= UP_DATA_ACK;	
				end
				
			LOW_DATA:	begin
					if(`SCL_LOW) begin
							if(num==4'd8) begin	
									num <= 4'd0;
									sda_r <= 1'b1;
									sda_link <= 1'b0;
									cstate <= LOW_DATA_ACK;
								end
							else begin
									sda_link <= 1'b1;
									num <= num+1'b1;
									case (num)
										4'd0: sda_r <= lowerData[7];
										4'd1: sda_r <= lowerData[6];
										4'd2: sda_r <= lowerData[5];
										4'd3: sda_r <= lowerData[4];
										4'd4: sda_r <= lowerData[3];
										4'd5: sda_r <= lowerData[2];
										4'd6: sda_r <= lowerData[1];
										4'd7: sda_r <= lowerData[0];
										default: sda_r<=1'b1;
									endcase
									cstate <= LOW_DATA;					
								end
						end
					else cstate <= LOW_DATA;				
				end

				
			LOW_DATA_ACK:	begin
					if(!sda/*`SCL_NEG*/) begin
							cstate <= STOP1;		
							
						end
					else cstate <= LOW_DATA_ACK;
				end
				
			STOP1:	begin
					if(`SCL_LOW) begin
							sda_link <= 1'b1;
							sda_r <= 1'b0;
							cstate <= STOP1;
						end
					else if(`SCL_HIG) begin
							sda_r <= 1'b1;	
							sda_link<=1'b1;
							cstate <= STOP2;
						end
					else cstate <= STOP1;
				end
			STOP2:	begin
					
					if(num==4'd8) begin cstate <= IDLE;busy<=1'b0;end
					else if(`SCL_LOW)begin num<=num+1'b1;cstate<=STOP2;end
					else cstate<=STOP2;
				end
			default: cstate <= IDLE;
			endcase
end

assign sda = sda_link ? sda_r:1'bz;

//---------------------------------------------

endmodule


