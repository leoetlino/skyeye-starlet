/**********************************/
/*                                */
/*  Copyright 2000, David Grant   */
/*                                */
/*  see LICENSE for more details  */
/*                                */
/**********************************/

#include "coldfire.h"

/* AND instruction */

/* Format 
   
 15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 1 | 1 | 0 | 0 | Register  |  OPmode   |  EA Mode  |EA Register|
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

*/

int ANDTime[2][8]={{ 1, 3, 3, 3, 3, 4, 3, 1},  
	           {-1, 3, 3, 3, 3, 4, 3, -1}};



INSTRUCTION_5ARGS(AND,
	unsigned Code1,4,
	unsigned Register,3,
	unsigned OPMode,3,
	unsigned EAMode,3,
	unsigned EARegister,3);

static void execute(void)
{
	struct _Address Source,Destination;
	unsigned int Result, SValue, DValue;
	AND_Instr Instr;
	Memory_RetrWordFromPC(&Instr.Code);

	if(Instr.Bits.OPMode==2) { /* <EA>y & Dx */
		if(Instr.Bits.EAMode==1) {
			ERR("May not specify Ax for source");
			return;
		}
		if(!EA_GetFromPC(&Source, 32, Instr.Bits.EAMode, Instr.Bits.EARegister)) return;
		if(!EA_GetFromPC(&Destination, 32, 0, Instr.Bits.Register)) return;
	} else if (Instr.Bits.OPMode==6) { /* Dy & <EA>x -> <EA>x */
		if(Instr.Bits.EAMode==0) {
			ERR("May not specify Dx for destination when source is Dx");
			return;
		} else if (Instr.Bits.EAMode==1) {
			ERR("May not specify Ax for destination when source is Dx");
			return;
		} else if (Instr.Bits.EAMode==7 && Instr.Bits.EARegister==4) {
			ERR("May not specify Immediate Addressing for destination");
			return;
		}
		if(!EA_GetFromPC(&Source, 32, 0, Instr.Bits.Register)) return;
		if(!EA_GetFromPC(&Destination, 32, Instr.Bits.EAMode, Instr.Bits.EARegister)) return;
	} else {
		ERR("Unknown OPMode %d", Instr.Bits.OPMode);
		return;
	}

	EA_GetValue(&SValue, &Source);
	EA_GetValue(&DValue, &Destination);

	Result = SValue & DValue;

	EA_PutValue(&Destination, Result);

	/* Set the status register
	 *  X - Not affected
	 *  N - Set if source is -ve, cleared otherwise
	 *  Z - Set if source is zero, cleared otherwise
	 *  V - always cleared
	 *  C - always cleared 
	 */
	memory_core.sr &= 0xFFF0;
	SRBits->N = ((int)Result < 0);
	SRBits->Z = (Result == 0);
	
	
	if (Instr.Bits.OPMode==2)
		cycle(ANDTime[0][cycle_EA(Instr.Bits.EARegister,Instr.Bits.EAMode)]);
	else cycle(ANDTime[1][cycle_EA(Instr.Bits.EARegister,Instr.Bits.EAMode)]);
	
	return;
}

static int disassemble(char *Instruction, char *Arg1, char *Arg2)
{
	AND_Instr Instr;
	Memory_RetrWordFromPC(&Instr.Code);
	sprintf(Instruction, "AND.L");
	if(Instr.Bits.OPMode==2) { /* <EA>y & Dx */
		Addressing_Print(32, Instr.Bits.EAMode, Instr.Bits.EARegister, Arg1);
		Addressing_Print(32, 0, Instr.Bits.Register, Arg2);	
	} else {
		Addressing_Print(32, 0, Instr.Bits.Register, Arg1);
		Addressing_Print(32, Instr.Bits.EAMode, Instr.Bits.EARegister, Arg2);	
	}
	return 0;
}

int and_5206_register(void)
{
	instruction_register(0xC000, 0xF000, &execute, &disassemble);
	return 1;
}
