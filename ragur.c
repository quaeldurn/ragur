#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define VERSION 0.2

#define CNT_INSTR 4	/* there are 4 instructions that we need to loop through */

#define comparestr(a, b) (strcmp(a, b) == 0)

int seeker(FILE *, long int);

/* the registers */
int A = 4;
int B = 3;
int C = 0;

/* The program counter */
int PC = 0;

/* the ram */
int *ram = NULL;

/* the interface for the registers, [3] is defined in main() */
int *registers[4] = {&A, &B, &C, NULL};

const char match_register[4][3] = {"A", "B", "C", "[A]"};


void halt()
{
	exit(0);
}

void clear(int *pntr)
{
	*pntr = 0;
}

void inc(int *pntr)
{
	*pntr = *pntr + 1;
}

/* JIE is is called differently, with values instead of refrences unlike
   clear and inc.
 */
void jie(int value1, int value2, long int z)
{
	if (value1 == value2) {
		PC = z;	
	}
}


int main(int argc, char *argv[]) 
{

	{
		char c;

		int vflag = 0;
		int helpflag = 0;

		while ((c = getopt(argc, argv, "v")) != -1) {
			switch (c) {
			  case 'v':
			  	vflag = 1;
			  case '?':
			  	helpflag = 1;
			}
		}

		if (vflag == 1) {
			printf("Version 0.2\n");
			return 0;
		} else if (helpflag == 1) {
			printf("Usage: ragur file\n");
			printf("file being a file parsed by anal.pl");
		}

	}

	/* allocates the area available through derefrencing A */
	ram = malloc(60 * sizeof (int));

	char *line = NULL;
	unsigned int buffer = 0;

	char *operator = malloc(6 * sizeof (char));
	char *register1 = malloc(5 * sizeof (char));
	char *register2 = malloc(5 * sizeof (char));

	long int z = 0;

	if (argc == 1) {
		fprintf(stderr, "need argument please, add - behaviour while your at it.\n");
		return 1;
	}

	FILE *file = fopen(argv[1], "r");

	while (getline(&line, &buffer, file) != EOF) {

		/* initalizes [3] to A index, *ram */
		registers[3] = (ram + A);

		/* parses line, putting each word into a variable */
		sscanf(line, "%s %s %s %ld", operator, register1, register2, &z);

		printf(" ++%s++ =%d=  a: %d  b: %d  c: %d\n", operator, PC, A, B, C); 

		/* when halt: exit and return success */
		if (comparestr(operator, "HALT")) {
			halt();	
		} else if (comparestr(operator, "CLEAR")) {
			int i;
			for (i = 0; i < CNT_INSTR; i++) {
				if (comparestr(register1, match_register[i])) {
					clear(registers[i]);
					break;
				}
				
			}
		} else if (comparestr(operator, "INC")) {
			int i;
			for (i = 0; i < CNT_INSTR; i++) {
				if (comparestr(register1, match_register[i])) {
					inc(registers[i]);
					break;
				}
			}
		/* The second register may be a number, 
		   but only the second one. For my sanity.
		 */ 
		} else if (comparestr(operator, "JIE")) {
			int i;
			int val1, val2;

			/* there was a destructive comment here */
			z--;

			for (i = 0; i < CNT_INSTR; i++) {
				if (comparestr(register1, match_register[i])) {
					val1 = *registers[i];
					break;
				}
			}

			for (i = 0; i < CNT_INSTR; i++) {
				if (comparestr(register2, match_register[i])) {
					val2 = *registers[i];
					break;
				}
			}
			
			/* if a matching register was not found in register2 then assume
			   it's an integer in string form.
			 */
			if (i > CNT_INSTR - 1) {
				val2 = atoi(register2);	
				jie(val1, val2, z);
			} else {
				jie(val1, val2, z);
			}

		} else {
			fprintf(stderr, "error\n");
		}

		PC++;

		/* sets the file position to PC */
		seeker(file, PC);
	}
	/* if HALT didn't stop the program something went wrong */
	return 1;
}

/* changes the file position-inticator to line where,
   it reads until it hits where many '\n's 
 */
int seeker(FILE *stream, long int where)
{
	char c;
	int long ns = 0;
	int long bytes = 0;

	/* start from the beginning*/
	fseek(stream, 0, SEEK_SET);

	while (ns < where && c != EOF) {
		c = getc(stream);
		if (c == '\n') {
			ns++;
		}
		bytes++;
	}
	fseek(stream, 0, SEEK_SET + bytes);

	/* returns where in file the file position indicator is, indexed from SEEK_SET */
	return bytes;
}
