#define ACK_ENABLE	    "D5/d"		    //	1101 0101 + delay
#define ACK_DISABLED	"D6/d"		    //	1101 0110 + delay
#define BACKGROUND	    "90/d"		    //	1001 0000 + delay
#define GO		        "08/d"		    //	0000 1000 + delay
#define TRACE1		    "10/d"		    //	0001 0000 + delay
#define TAGGO		    "18/d"		    //	0001 1000 + delay

//-------------------------------------------------------
#define READ_A		    "68/d/RD"	    //	0110 1000 + delay + read
#define READ_CCR	    "69/d/RD"		//	0110 1001 + delay + read	
#define READ_PC		    "6B/d/RD16"	    //  0110 1011 + delay + read(16)
#define READ_HX		    "6C/d/RD16"	    //	0110 1100 + delay + read(16)
#define READ_SP		    "6F/d/RD16"	    //	0110 1111 + delay + read(16)
#define READ_NEXT	    "70/d/RD"		//	0111 0000 + delay + read
#define READ_NEXT_WS	"71/d/SS/RD"	//	0111 0001 + delay + read SS + read data

//--------------------------------------------------
#define WRITE_A 	    "48/WD/d" 	    //	0100 1000 + write + delay
#define WRITE_CCR  	    "49/WD/d" 	    //	0100 1001 + write + delay
#define WRITE_PC  	    "4B/WD16/d" 	//	0100 1011 + write + delay
#define WRITE_HX  	    "4C/WD16/d" 	//	0100 1100 + write + delay
#define WRITE_SP  	    "4F/WD16/d" 	//	0100 1111 + write + delay
#define WRITE_NEXT  	"50/WD/d" 	    //	0101 0000 + write + delay
#define WRITE_NEXT_WS 	"51/WD/d/SS" 	//	0101 0001 + write data + delay + read SS

//--------------------------------------------------
#define READ_STATUS 	"E4/SS" 		    //	1110 0100 + read SS
#define WRITE_CONTROL 	"C4/CC"		        //	1100 0100 + write CC
#define READ_BYTE 	    "E0/AAAA/d/RD"	    //	0101 0000 + tx address + delay + read
#define READ_BYTE_WS 	"E1/AAAA/d/SS/RD"   //	1110 0000 + tx address + delay + read SS + read
#define READ_LAST  	    "E8/SS/RD"	        //	1110 1000 + read SS + read
#define WRITE_BYTE 	    "C0/AAAA/WD/d" 	    //	1100 0000 + tx address + write + delay
#define WRITE_BYTE_WS   "C1/AAAA/WD/d/SS"	//	1100 0001 + tx address + write + delay + read SS
#define READ_BKPT 	    "E2/RBKP"		    //	1110 0010 + read BKPT
#define WRITE_BKPT 	    "C2/WBKP"		    //	1100 0010 + write BKPT