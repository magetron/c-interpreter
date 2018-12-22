#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"
#define DEBUG 1

int token; 			// current token
char *src, *old_src;		// pointer to src string
int poolsize;			// default size
int line;			// current line number

int *text, *old_text, *stack; 	// text segment, dump text segment, stack
char *data;			// data segment

int *pc, *bp, *sp, ax, cycle; 	// VM registers, program counter, base pointer, stack pointer (from high addr -> low addr), general-purpose registers (GPRs)

// Instructions supported (intel x86-based)
enum { LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH, 
	OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
	OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT };



void next () {
	token = *src++;
	return;
}

void expression (int level) {
	
}

void program () {
	next();
	while (token > 0) {
		printf("token = %c\n", token);
		next();
	}
}



int eval () {
	int op, *tmp;
	cycle = 0;

	while (1) {
		// Get next command
		cycle++;
		op = *pc++;
            	
		if (DEBUG) {
			printf("cycle %d > %.4s", cycle,
					& 	"LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,"
						"OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL  SHR  ADD ,SUB ,MUL ,DIV ,MOD ,"
						"OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT"[op * 5]);
			if (op <= ADJ) printf(" pc = %d\n", *pc);
			else printf("\n");
		}

		switch (op) {
			// Operations / Instructions

			// MOV
			// MOV dest, source (basically moving the stuff in source to destination, could be anything)
			// In pcc, we split MOV into 5 commands which only takes in at most 1 argument
			// IMM <num> : Put <num> to ax
			// LC : Load Char to ax addr
			// LI : Load Integer to ax addr
			// SC : Save Char from ax addr to Stack Top addr
			// SI : Save Integer from ax addr to Stack Top addr
			case IMM :
				ax = *pc++;			//load immediate value to ax
				break;
			case LC :
				ax = *(char *)ax;		//load char to ax addr;
				break;
			case LI :
				ax = *(int *)ax;		//load int to ax addr;
				break;
			case SC :
				*(char *)*sp++ = ax;		//save char to stack top addr
				break;
			case SI :
				*(int *)*sp++ = ax;		//save int to stack top addr
				break;
			
				// PUSH
			// PUSH : push the value of ax to the stack
			case PUSH :
				*--sp = ax;
				break;
			// JMP
			// JMP <addr> : set program counter to the new <addr>
			case JMP :
				pc = (int *)*pc;		//pc is storing the next command, which is the new <addr> we want to JMP to.
				break;

			// JZ / JNZ
			// if statement is implement using JZ and JNZ (jump when is zero, jump when is not zero)
			// JZ : jump when ax is zero
			// JNZ : jump when ax is not zero
			case JZ :
				pc = ax ? pc + 1 : (int *)*pc;
				break;
			case JNZ :
				pc = ax ? (int *)*pc : pc + 1;
				break;

			// Subroutine
			// CALL <addr> : call subroutine at <addr>. Note this is different from JMP since we need to store the current pc for future coming back to.
			// RET : return from subroutine (replaced by LEV)
			// ENT <size> : enter (make new call frame) stores the current stack pointer and saves <size> on stack to store local variables. 
			// ADJ <size> : (remove arguments from frame) removes the <size> amount of data in stack done by ENT
			// LEV : leave (restore call frame and pc) restore the stored call frame before ENT and restores pc
			// LEA : load address for arguments, a tmp fix for our ADD, since ADD only operates on ax, we need a command to operate the offset

			// sub_function(arg1, arg2, arg3)
			// |    ....       | high address
			// +---------------+
			// | arg: 1        |    new_bp + 4
			// +---------------+
			// | arg: 2        |    new_bp + 3
			// +---------------+
			// | arg: 3        |    new_bp + 2
			// +---------------+
			// |return address |    new_bp + 1
			// +---------------+
			// | old BP        | <- new BP
			// +---------------+
			// | local var 1   |    new_bp - 1
			// +---------------+
			// | local var 2   |    new_bp - 2
			// +---------------+
			// |    ....       |  low address
			
			case CALL :
				*--sp = (int)(pc + 1);
				pc = (int *)*pc;
				break;

			//case RET :
			//	pc = (int *)*sp++;
			// Above is an ideal RET function. In pcc, we use LEV to replace RET since we need to consider the return value of subroutines.
			case ENT :
				*--sp = (int)bp;
				bp = sp;
				sp = sp - *pc++;
				break;
			case ADJ :
				sp = sp + *pc++;
				break;
			case LEV :
				sp = bp;
				bp = (int *)*sp++; 
				pc = (int *)*sp++;
				break;
			case LEA :
				ax = (int)(bp + *pc++);
				break;

			// Operator Instructions
			// These are built-in basic operations. 
			case OR : 	ax = *sp++ | ax; 	break;
			case XOR : 	ax = *sp++ ^ ax; 	break;
			case AND : 	ax = *sp++ & ax; 	break;
			case EQ : 	ax = *sp++ == ax;	break;
			case NE :	ax = *sp++ != ax;	break;
			case LT :	ax = *sp++ < ax;	break;
			case LE :	ax = *sp++ <= ax;	break;
			case GT :	ax = *sp++ > ax;	break;
			case GE :	ax = *sp++ >= ax;	break;
			case SHL :	ax = *sp++ << ax;	break;
			case SHR :	ax = *sp++ >> ax;	break;
			case ADD :	ax = *sp++ + ax;	break;
			case SUB :	ax = *sp++ - ax;	break;
			case MUL :	ax = *sp++ * ax;	break;
			case DIV :	ax = *sp++ / ax;	break;
			case MOD :	ax = *sp++ % ax;	break;				
			
			// System Commands
			// These commands including open and closing files, IO from console, memory allocation and etc.
			// These commands requires extensive knowledge to implement, such that we will simply use built-in functions provided.
			case EXIT :
				printf("EXIT : %d\n", *sp);
				return *sp;
				break;
			case OPEN :
				ax = open( (char *)sp[1], sp[0]);
				break;
			case CLOS :
				ax = close(*sp);
				break;
			case READ :
				ax = read(sp[2], (char *)sp[1], *sp);
				break;
			case PRTF :
				tmp = sp + pc[1];
				ax = printf( (char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]);
				break;
			case MALC :
				ax = (int)malloc(*sp);
				break;
			case MSET :
				ax = (int)memset( (char *)sp[2], sp[1], *sp);
				break;
			case MCMP :
				ax = memcmp( (char *)sp[2], (char *)sp[1], *sp);
				break;
			
			// ERROR fallback
			// If op doesn't belong to any of the above instructions, there must be something wrong, therefore we exit the VM.
			default :
				printf("ERROR : unknown instruction %d\n", op);
				return -1;
				break;
		}
	}
	return 0;
}

int main (int argc, char **argv) {
	int i, fd;

	argc--;
	argv++;

	// default size && init
	poolsize = 256 * 1024; 
	line = 1;


	// read file && deal with ERROR msgs
	if ( (fd = open(*argv, 0)) < 0 ) {
		printf("ERROR : could not open file %s\n", *argv);
		return -1;
	}
	
	if ( !(src = old_src = malloc(poolsize)) ) {
		printf("ERROR : could not malloc size of %d for source area\n", poolsize);
		return -1;
	}

	if ( (i = read(fd, src, poolsize - 1)) <= 0) {
		printf("ERROR : read src failed; return value %d\n", i);
		return -1;
	}
	
	// add EOF
	src[i] = 0;
	close(fd);

	// read file finished, ready to do some real stuff
	// +------------------+
	// |    stack   |     |      high address
	// |    ...     v     |
	// |                  |
	// |                  |
	// |                  |
	// |                  |
	// |    ...     ^     |
	// |    heap    |     |
	// +------------------+
	// | bss  segment     |
	// +------------------+
	// | data segment     |
	// +------------------+
	// | text segment     |      low address
	// +------------------+
	//
	// Note : currently pcc does not support uninitialised variables, such that there would be no bss segment.


	// allocate memory for VM
	if ( !(text = old_text = malloc(poolsize)) ) {
		printf("ERROR : could not malloc size of %d for text area\n", poolsize);
		return -1;
	}

	if ( !(data = malloc(poolsize)) ) {
		printf("ERROR : could not malloc size of %d for data area\n", poolsize);
		return -1;
	}

	if ( !(stack = malloc(poolsize)) ) {
		printf("ERROR : could not malloc size of %d for stack area\n", poolsize);
		return -1;
	}
	
	// init value for VM
	memset(text, 0, poolsize);
	memset(data, 0, poolsize);
	memset(stack, 0, poolsize);
	
	bp = sp = (int *)( (int)stack + poolsize );
	ax = 0;
	
	program();
	return eval();
}
