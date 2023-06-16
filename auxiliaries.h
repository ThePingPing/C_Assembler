#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* That struct stores the "code image" and the "data image" data */
typedef struct image {
	int address;
	char* binaryMachineCode;
	int NumberOfWords;
	struct image* next;
} image;

/* That struct stores the symbol table */
typedef struct symbolTable {
	char* symbol;
	int value;
	char* codeData;
	char* externalEntry;
	struct symbolTable* next;
} symbolTable;

/* That struct stores the compiler mapping table */
typedef struct mapping {
	char* command;
	char* opcode;
	char* funct;
	char* allowedSourceAddressing;
	char* allowedDestAddressing;
} mapping;

/* That struct stores a list of words that use an external label  */
typedef struct externalList {
	char* label;
	int address;
	struct externalList* next;
} externalList;

/* That function creates the mapping table   */
void createMappingTable(mapping* mappingArray) {
	mappingArray[0].command = "mov";    /* command number 0  */
	mappingArray[0].opcode = "000000";
	mappingArray[0].funct = "00000";
	mappingArray[0].allowedSourceAddressing = "013";
	mappingArray[0].allowedDestAddressing = "13";

	mappingArray[1].command = "cmp";	   /* command number 1  */
	mappingArray[1].opcode = "000001";
	mappingArray[1].funct = "00000";
	mappingArray[1].allowedSourceAddressing = "013";
	mappingArray[1].allowedDestAddressing = "013";

	mappingArray[2].command = "add";	   /* command number 2  */
	mappingArray[2].opcode = "000010";
	mappingArray[2].funct = "00001";
	mappingArray[2].allowedSourceAddressing = "013";
	mappingArray[2].allowedDestAddressing = "13";

	mappingArray[3].command = "sub";	   /* command number 3  */
	mappingArray[3].opcode = "000010";
	mappingArray[3].funct = "00010";
	mappingArray[3].allowedSourceAddressing = "013";
	mappingArray[3].allowedDestAddressing = "13";

	mappingArray[4].command = "lea";	   /* command number 4  */
	mappingArray[4].opcode = "000100";
	mappingArray[4].funct = "00000";
	mappingArray[4].allowedSourceAddressing = "1";
	mappingArray[4].allowedDestAddressing = "13";

	mappingArray[5].command = "clr";	   /* command number 5  */
	mappingArray[5].opcode = "000101";
	mappingArray[5].funct = "00001";
	mappingArray[5].allowedSourceAddressing = "X";	/* "X" means that there is no source operand!  */
	mappingArray[5].allowedDestAddressing = "13";

	mappingArray[6].command = "not";	   /* command number 6  */
	mappingArray[6].opcode = "000101";
	mappingArray[6].funct = "00010";
	mappingArray[6].allowedSourceAddressing = "X";
	mappingArray[6].allowedDestAddressing = "13";

	mappingArray[7].command = "inc";	   /* command number 7  */
	mappingArray[7].opcode = "000101";
	mappingArray[7].funct = "00011";
	mappingArray[7].allowedSourceAddressing = "X";
	mappingArray[7].allowedDestAddressing = "13";

	mappingArray[8].command = "dec";	   /* command number 8  */
	mappingArray[8].opcode = "000101";
	mappingArray[8].funct = "00100";
	mappingArray[8].allowedSourceAddressing = "X";
	mappingArray[8].allowedDestAddressing = "13";

	mappingArray[9].command = "jmp";	   /* command number 9  */
	mappingArray[9].opcode = "001001";
	mappingArray[9].funct = "00001";
	mappingArray[9].allowedSourceAddressing = "X";
	mappingArray[9].allowedDestAddressing = "12";

	mappingArray[10].command = "bne";	   /* command number 10  */
	mappingArray[10].opcode = "001001";
	mappingArray[10].funct = "00010";
	mappingArray[10].allowedSourceAddressing = "X";
	mappingArray[10].allowedDestAddressing = "12";

	mappingArray[11].command = "jsr";	   /* command number 11  */
	mappingArray[11].opcode = "001001";
	mappingArray[11].funct = "00011";
	mappingArray[11].allowedSourceAddressing = "X";
	mappingArray[11].allowedDestAddressing = "12";

	mappingArray[12].command = "red";	   /* command number 12  */
	mappingArray[12].opcode = "001100";
	mappingArray[12].funct = "00000";
	mappingArray[12].allowedSourceAddressing = "X";
	mappingArray[12].allowedDestAddressing = "13";

	mappingArray[13].command = "prn";	   /* command number 13  */
	mappingArray[13].opcode = "001101";
	mappingArray[13].funct = "00000";
	mappingArray[13].allowedSourceAddressing = "X";
	mappingArray[13].allowedDestAddressing = "013";

	mappingArray[14].command = "rts";	   /* command number 14  */
	mappingArray[14].opcode = "001110";
	mappingArray[14].funct = "00000";
	mappingArray[14].allowedSourceAddressing = "X";
	mappingArray[14].allowedDestAddressing = "X";

	mappingArray[15].command = "stop";	   /* command number 15  */
	mappingArray[15].opcode = "001111";
	mappingArray[15].funct = "00000";
	mappingArray[15].allowedSourceAddressing = "X";
	mappingArray[15].allowedDestAddressing = "X";
}

/* That function creates a new line in the data image or code image linked lists  */
void addImageLine(image* Head, int address, char* binaryMachineCode, int numberOfWords) {
	image* newLine;
	char* receivedBinaryMachineCode;
	int k;

	receivedBinaryMachineCode = (char*)malloc(25 * sizeof(char));
	for (k = 0; binaryMachineCode[k] != '\0'; k++) { receivedBinaryMachineCode[k] = binaryMachineCode[k]; }
	receivedBinaryMachineCode[k] = '\0';

	if (Head->address == -1) {
		Head->binaryMachineCode = receivedBinaryMachineCode;
		Head->address = address;
		Head->NumberOfWords = numberOfWords;
		return;
	}
	newLine = Head;
	while (newLine->next != NULL) { newLine = newLine->next; }
	newLine->next = (image*)malloc(sizeof(image));
	newLine->next->binaryMachineCode = receivedBinaryMachineCode;
	newLine->next->address = address;
	newLine->next->NumberOfWords = numberOfWords;
	newLine->next->next = NULL;
}

/* That function creates a new line in the symbol table  */
char* addSymbolLine(symbolTable* Head, char* symbol, int value, char* codeData, char* externalEntry) {
	symbolTable* newLine;
	char* receivedSymbol;
	int k;

	if (Head == NULL)		/* That can never happen since we initialize manually the first node. I only write it to avoid warning  */
		return NULL;

	receivedSymbol = (char*)malloc(strlen(symbol) * (sizeof(char)));
	for (k = 0; symbol[k] != '\0'; k++) { receivedSymbol[k] = symbol[k]; }
	receivedSymbol[k] = '\0';

	if (Head->value == -1) {
		Head->symbol = receivedSymbol;
		Head->value = value;
		Head->codeData = codeData;
		Head->externalEntry = externalEntry;
		return (char*)"Success";
	}
	newLine = Head;
	while (newLine != NULL) {
		if (strcmp(newLine->symbol, receivedSymbol) == 0) {
									/* if the new line is extern and there is another extern already in the table with the same label, do nothing  */
			if ((strcmp(newLine->externalEntry, "external") == 0) && (strcmp(externalEntry, "external") == 0)) 
				return (char*)"Success";
			return (char*)"ERROR: Symbol already exist.";			/* if we are attempting to insert a symbol that is already exists (and it is not an external one) that's an error  */
		}
		newLine = newLine->next;
	}

	newLine = Head;
	while (newLine->next != NULL) { newLine = newLine->next; }
	newLine->next = (symbolTable*)malloc(sizeof(symbolTable));
	newLine->next->symbol = receivedSymbol;
	newLine->next->value = value;
	newLine->next->codeData = codeData;
	newLine->next->externalEntry = externalEntry;
	newLine->next->next = NULL;
	return (char*)"Success";
}

/* That function creates a new line of addresses of words which use an external label  */
void addExternalListItem(externalList* Head, char* label, int address) {
	externalList* newLine;
	char* receivedLabel;
	int k;

	receivedLabel = (char*)malloc(strlen(label) * (sizeof(char)));
	for (k = 0; label[k] != '\0'; k++) { receivedLabel[k] = label[k]; }
	receivedLabel[k] = '\0';

	if (Head->address == -1) {
		Head->label = receivedLabel;
		Head->address = address;
		return;
	}
	for (newLine = Head; newLine->next != NULL; newLine = newLine->next);		/* bring newLine to the last existing item, since its "next" is NULL  */
	newLine->next = (externalList*)malloc(sizeof(externalList));
	newLine->next->address = address;
	newLine->next->label = receivedLabel;
	newLine->next->next = NULL;
}

/* That function gets the type of line: .data/.string/.entry/.extern or any valid command  */
char* getLineType(char line[83], mapping* mappingArray, int i) {
	int i2, k, segmentEnd;		/* When we get to that function we know if there is a label or not. if there is, we know that its length is i.       */
								/* if there is a label, we know that line[i] == ':' and we begin to parse the remaining of the line from line[i+1].  */
								/* if there is no label, i=-1 and we start to parse the remaining of the line from line[i+1] which is line[0].	     */
								/* when we get to that function we know that the line is not empty and not a comment line.							 */
								/* the next thing that we expect to see is: ".data" / ".string" / ".entry" / ".extern" / command_name.				 */
								/* those are the only valid possibilities.																			 */

	for (i2 = i + 1; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);		/* find the first char which is not ' ' or '\t'  */
	if ((line[i2] == '\n') || (line[i2] == '\0'))
		return (char*)"ERROR: line has only a label in it.";	   /* It must be a label in the line, since if it weren't we would have found that it is an empty line at the first for loop of function "firstRun"  */

	for (segmentEnd = i2; ((line[segmentEnd] != '\n') && (line[segmentEnd] != ' ') && (line[segmentEnd] != '\t') && (line[segmentEnd] != '\0')); segmentEnd++);		/* let us hold the end of segment (next ' ' or '\t' or \n)  */
	if (line[i2] == '.') {							/* now check if it is data/string/entry/extern   */
		if ((segmentEnd - i2 == 5) && (line[i2 + 1] == 'd') && (line[i2 + 2] == 'a') && (line[i2 + 3] == 't') && (line[i2 + 4] == 'a') && ((line[i2 + 5] == ' ') || (line[i2 + 5] == '\t')))
			return (char*)".data";
		if ((segmentEnd - i2 == 6) && (line[i2 + 1] == 'e') && (line[i2 + 2] == 'n') && (line[i2 + 3] == 't') && (line[i2 + 4] == 'r') && (line[i2 + 5] == 'y') && ((line[i2 + 6] == ' ') || (line[i2 + 6] == '\t')))
			return (char*)".entry";
		if (segmentEnd - i2 == 7) {
			if ((line[i2 + 1] == 'e') && (line[i2 + 2] == 'x') && (line[i2 + 3] == 't') && (line[i2 + 4] == 'e') && (line[i2 + 5] == 'r') && (line[i2 + 6] == 'n') && ((line[i2 + 7] == ' ') || (line[i2 + 7] == '\t')))
				return (char*)".extern";
			if ((line[i2 + 1] == 's') && (line[i2 + 2] == 't') && (line[i2 + 3] == 'r') && (line[i2 + 4] == 'i') && (line[i2 + 5] == 'n') && (line[i2 + 6] == 'g') && ((line[i2 + 7] == ' ') || (line[i2 + 7] == '\t')))
				return (char*)".string";
		}
		return (char*)"ERROR: invalid instruction. must be data/string/entry/extern and then parameters.";
	}	/* at this point the only valid possibility is a command line, with one of the 16 legal commands   */


	if (((segmentEnd - i2) == 3) && ((line[segmentEnd] == ' ') || (line[segmentEnd] == '\t'))) {		/* we would accept any command which is not "stop"/"rts", followed by ' ' or '\t'  */
		for (k = 0; k < 14; k++) {
			if ((line[i2] == mappingArray[k].command[0]) && (line[i2 + 1] == mappingArray[k].command[1]) && (line[i2 + 2] == mappingArray[k].command[2]))
				return (char*)mappingArray[k].command;
		}
	}
	if ((line[segmentEnd] == ' ') || (line[segmentEnd] == '\t') || (line[segmentEnd] == '\n') || (line[segmentEnd] == '\0')) {		/* after "stop"/"rts" commands we would accept ' ' or '\t' or '\n' or end of file only  */
		if (((segmentEnd - i2) == 3) && (line[i2] == 'r') && (line[i2 + 1] == 't') && (line[i2 + 2] == 's'))
			return (char*)mappingArray[14].command;
		if (((segmentEnd - i2) == 4) && (line[i2] == 's') && (line[i2 + 1] == 't') && (line[i2 + 2] == 'o') && (line[i2 + 3] == 'p'))
			return (char*)mappingArray[15].command;
	}
	return (char*)"ERROR: invalid line.";			/* there is no legal command/instruction in that line. it is Invalid  */
}

/* That function checks if there is a label at line beginning   */
int getLabel(char line[83]) {
	int i;		/* i>0 describes the length of label.						*/
				/* i=-1 means that there is no label.						*/
				/* i=-2 means that there is an error regarding the label.	*/

	if (!(((line[0] >= 'a') && (line[0] <= 'z')) || ((line[0] >= 'A') && (line[0] <= 'Z'))))		/* If the line doesn't start with a-z or A-Z so there is no label, return -1  */
		return -1;

	for (i = 1; (((line[i] >= 'a') && (line[i] <= 'z')) || ((line[i] >= 'A') && (line[i] <= 'Z')) || ((line[i] >= '0') && (line[i] <= '9'))); i++);
	if (line[i] == ':') {
		if (i < 32)
			return i; 	/* if the label has a legal length return its length  */
		return -2;         		    /* if the label is too long return (-2) which means that there is an error    */
	}
	return -1;				/* if the label does not end with a ':' it is not a label.   */
}

/* That function receives an int number and returns its representation in binary (2's complement) */
char* twoComplement(int numInInt, int lengthOfRepresentation) {
	int reference = 1;
	int i, num;
	char* binaryNumToBeSent;

	for (i = 1; i < lengthOfRepresentation; i++) { reference = reference * 2; }		/* we want the reference to be 2^(lengthOfRepresentation-1)   */

	binaryNumToBeSent = (char*)malloc(25 * sizeof(char));
	binaryNumToBeSent[24] = '\0';

	num = numInInt;
	if (numInInt < 0)
		num = num * -1;		/* we are working with positive numbers at this stage     */

	for (i = 0; i < lengthOfRepresentation; i++) {
		if (num >= reference) {
			num = num - reference;
			binaryNumToBeSent[i] = '1';
		}
		else {
			binaryNumToBeSent[i] = '0';
		}
		reference = reference / 2;
	}

	if (numInInt < 0) {
		for (i = 0; i < lengthOfRepresentation; i++) {
			if (binaryNumToBeSent[i] == '1')
				binaryNumToBeSent[i] = '0';
			else
				binaryNumToBeSent[i] = '1';
		}
		i = lengthOfRepresentation - 1;
		while (binaryNumToBeSent[i] == '1') {
			binaryNumToBeSent[i] = '0';
			i--;
		}
		binaryNumToBeSent[i] = '1';
	}
	return binaryNumToBeSent;
}

/* That function parses a ".data" instruction	*/
int parseDataInstruction(int i, char line[83], image* dataImageHead, int address) {
	int i2, j, k;    /* whether there is a label or not, we start to parse from line[i+1]. at this point we know that there is ".data" and ' ' or '\t' afterwards.  */
	int numOfLines = 0;
	int number;
	char* stringNumToBeInt;
	char* binaryNumToBeSent;

	for (i2 = i + 1; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);     /* after that line, line[i2] == '.' and line[i2+5] == ' ' or '\t'   */
	i2 = i2 + 5;		/* at this point i2 holds the first char after ".data". we know that it is a ' ' or a '\t'   */
	while (i2 < 84) {		/* actually we could write here while(1), but it will issue a warning   */
		for (; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);	   /* we are looking for the first char which is not ' ' or '\t'  */
		if (!(((line[i2] >= '0') && (line[i2] <= '9')) || (line[i2] == '-'))) { return -1; } /* if the following argument after the ".data" instruction or after ',' (excluding ' ' and '\t') is not a number, that's an error  */
		/* at this point we know that line[i2] == '-' or line[i2] is a digit  */
		if (line[i2] == '-') {
			k = i2 + 1;
		}
		else {
			k = i2;
		}
		for (; ((line[k] >= '0') && (line[k] <= '9')); k++);	/* k holds the first char after the number  */
		if ((line[k] != ' ') && (line[k] != '\t') && (line[k] != '\n') && (line[k] != ',') && (line[k] != '\0')) { return -1; }	/* the first tab after the number must be ' ' or '\t' or '\n' or ',' or end of file  */

		stringNumToBeInt = (char*)malloc((k - i2 + 1) * (sizeof(char)));
		for (j = 0; (i2 + j) < k; j++) { stringNumToBeInt[j] = line[i2 + j]; }
		stringNumToBeInt[j] = '\0';
		number = atoi(stringNumToBeInt);
		free(stringNumToBeInt);
		binaryNumToBeSent = twoComplement(number, 24);
		addImageLine(dataImageHead, (address + numOfLines), binaryNumToBeSent, 0);			/* here we send the number to be added to the dataImage linked list  */
		free(binaryNumToBeSent);
		numOfLines++;
		for (; ((line[k] == ' ') || (line[k] == '\t')); k++);		/* now k holds the first char after the number which is not ' ' or '\t'  */
		if ((line[k] == '\n') || (line[k] == '\0')) { return numOfLines; }				/* if there are no more arguments, finish here and retun the number of lines we added to the dataImage linked list  */
		if (line[k] != ',') { return -1; }						/* if the first char after the number and the spaces or '\t' that follow the number is not '\n' and not ',' that's an error  */
		i2 = k + 1;			/* now i2 holds the first char after the ','  */
	}
	return numOfLines;
}

/* That function parses a ".string" instruction  */
int parseStringInstruction(int i, char line[83], image* dataImageHead, int address) {
	int i2, k, endOfLine;    /* whether there is a label or not, we start to parse from line[i+1]. at this point we know that there is ".data" and ' ' or '\t' afterwards.  */
	int numOfLines = 0;
	int charInAsci;
	char* binaryNumToBeSent;

	for (i2 = i + 1; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);     /* after that line, line[i2] == '.' and line[i2+7] == ' ' or '\t'  */
	for (i2 = i2 + 7; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);	   /* we are looking for the first char which is not ' ' or '\t' after ".string"   */
	if (line[i2] != '"') { return -1; }								/* if the following argument after the ".string" instruction (excluding ' ' and '\t') is not a '"', that's an error   */
	for (k = i2 + 1; ((line[k] != '\n') && (line[k] != '"') && (line[k] != '\0')); k++);	/* let k hold the '"' at the end of the string argument  */
	if ((line[k] == '\n') && (line[k] != '\0')) { return -1; }								/* if there is no '"' at the end of the string argument, that's an error  */
	/* at this point we know that line[i2] == '"' and that line[k] == '"' and that the whole string is between them  */
	for (endOfLine = k + 1; ((line[endOfLine] == ' ') || (line[endOfLine] == '\t')); endOfLine++);	/* after the second '"', we will accept only ' ' or '\t' until the end of line  */
	if ((line[endOfLine] != '\n') && (line[endOfLine] != '\0')) { return -1; }				/* if there is anything which is not ' ' or '\t' or '\n' or end of file after the string argument, that's an error  */
	while ((i2 + 1) < k) {		/* insert the asci codes of the chars in the string argument  */
		charInAsci = line[i2 + 1];
		binaryNumToBeSent = twoComplement(charInAsci, 24);
		addImageLine(dataImageHead, (address + numOfLines), binaryNumToBeSent, 0);			/* here we send the number to be added to the dataImage linked list  */
		free(binaryNumToBeSent);
		numOfLines++;
		i2++;
	} /* once we are done, insert the '\0' char  */
	addImageLine(dataImageHead, (address + numOfLines), "000000000000000000000000", 0);
	numOfLines++;
	return numOfLines;
}

/* That function parses a ".extern" instruction   */
char* parseExternInstruction(int i, char line[83]) {
	int i2, i3, i4;		/* whether there is a label or not, we start to parse from line[i+1]. at this point we know that there is ".extern" and ' ' or '\t' afterwards.  */
	char* labelArgument;

	for (i2 = i + 1; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);		/* after that line, line[i2] == '.' and line[i2+7] == ' ' or '\t'    */
	for (i2 = i2 + 7; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);		/* we are looking for the first char which is not ' ' or '\t' after ".extern"   */

	if (!(((line[i2] >= 'a') && (line[i2] <= 'z')) || ((line[i2] >= 'A') && (line[i2] <= 'Z'))))		/* If the argument doesn't start with a-z or A-Z that's not a legal label argument   */
		return NULL;		/* return NULL if there is an error  */
	/* at this point line[i2] is the first char of the label argument. we know that it is a valid start of a label argument  */

	for (i3 = i2 + 1; (((line[i3] >= 'a') && (line[i3] <= 'z')) || ((line[i3] >= 'A') && (line[i3] <= 'Z')) || ((line[i3] >= '0') && (line[i3] <= '9'))); i3++);
	/* at this point line[i3] is the first char after the end of the label argument  */

	for (i4 = i3; ((line[i4] == ' ') || (line[i4] == '\t')); i4++);			/* after the end of the label we allow only ' ' or '\t' until the '\n'  */
	if ((line[i4] != '\n') && (line[i4] != '\0')) { return (char*)"-1"; }	/* the first char which is not ' ' or '\t' after the end of label argument must be '\n' or end of file  */
	if ((i3 - i2) > 31) { return NULL; }								    /* label can't be longer than 31 chars  */
					/* at this point we have our label argument from line[i2] to line[i3-1]. we also know that the instruction is legal  */
	labelArgument = (char*)malloc((i3 - i2 + 1) * sizeof(char));
	for (i4 = i2; i4 < i3; i4++)
		labelArgument[i4 - i2] = line[i4];
	labelArgument[i4 - i2] = '\0';
	return labelArgument;
}

/* That function returns the following operand in the line  */
char* getOperand(int i, char line[83]) {
	int operandBeginning, operandEnd;	/* that function gets the line when line[i] is the first char of the next operand (NOT ' ' or '\t' or ',')  */
	int j;
	char* operand;

	operandBeginning = i;
	/* the next line looks for the first char after the operand. it might be ' ' or '\t' or '\n' or ',' or end of file only  */
	for (operandEnd = i + 1; ((line[operandEnd] != ' ') && (line[operandEnd] != '\t') && (line[operandEnd] != '\n') && (line[operandEnd] != ',') && (line[operandEnd] != '\0')); operandEnd++);
	operandEnd--;
	/* at this point we know that operand is between line[operandBeginning] and line[operandEnd]. line[operandEnd] is NOT ' ' or '\t' or '\n' or ',' or end of file   */
	operand = (char*)malloc((operandEnd - operandBeginning + 2) * sizeof(char));
	for (j = 0; j < (operandEnd - operandBeginning + 1); j++)
		operand[j] = line[i + j];
	operand[j] = '\0';
	return operand;
}

/* That function creates a binary machine code to be added to the code image  */
char* concatinator(char* opcode, int sourceAddressing, int sourceReg, int destAddressing, int destReg, char* funct, int ARE, int numberOnly, int takeNumberOnly) {
	int i;
	char* binaryMachineCodeToBeSent;
	char* binSourceAddressing;
	char* binSourceReg;
	char* binDestAddressing;
	char* binDestReg;
	char* binARE;
	char* binNumberOnly;

	binaryMachineCodeToBeSent = (char*)malloc(25 * sizeof(char));
	binaryMachineCodeToBeSent[24] = '\0';

	if (takeNumberOnly == 0) {			/* if it is 0, relate to the "opcode", "funct", ... the meaning is that this is the first word of the command  */
		for (i = 0; i <= 5; i++) { binaryMachineCodeToBeSent[i] = opcode[i]; }

		binSourceAddressing = twoComplement(sourceAddressing, 2);
		for (; i <= 7; i++) { binaryMachineCodeToBeSent[i] = binSourceAddressing[i - 6]; }
		free(binSourceAddressing);

		binSourceReg = twoComplement(sourceReg, 3);
		for (; i <= 10; i++) { binaryMachineCodeToBeSent[i] = binSourceReg[i - 8]; }
		free(binSourceReg);

		binDestAddressing = twoComplement(destAddressing, 2);
		for (; i <= 12; i++) { binaryMachineCodeToBeSent[i] = binDestAddressing[i - 11]; }
		free(binDestAddressing);

		binDestReg = twoComplement(destReg, 3);
		for (; i <= 15; i++) { binaryMachineCodeToBeSent[i] = binDestReg[i - 13]; }
		free(binDestReg);

		for (; i <= 20; i++) { binaryMachineCodeToBeSent[i] = funct[i - 16]; }
	}
	else {	/* at this point we know that we should take "numberOnly" instead of "opcode", "funct", ...   */
		binNumberOnly = twoComplement(numberOnly, 21);
		for (i = 0; i <= 20; i++) { binaryMachineCodeToBeSent[i] = binNumberOnly[i]; }
		free(binNumberOnly);
	}		/* at this point we filled the 21 left bits of the binary code. i == 21 now.  */

	binARE = twoComplement(ARE, 3);		/* the ARE field is present in both cases (first word, second word, third word)  */
	for (; i <= 23; i++) { binaryMachineCodeToBeSent[i] = binARE[i - 21]; }
	free(binARE);

	return binaryMachineCodeToBeSent;
}

/* That function modifies the operand and returns the modified version and the addressing type  */
char* getFinalOperand(char* operand, int* addressingType, int* hasErrors) {
	int k;

	if (operand[0] == '#') {			/* is it addressing number 0?  */
		k = 1;
		if (operand[1] == '-') {
			k++;
			operand[0] = '-';
		}  /* after this line, all of chars since firstOperand[k] up to the end should be digits  */
		if (operand[k] == '\0') {
			*hasErrors = 1;			/* we have found "#" or "#-" and that's illegal  */
			printf("ERROR: No number found after '#'.");
			return (char*)-1;
		}
		for (; k < (int)strlen(operand); k++) {
			if ((operand[k] >= '0') && (operand[k] <= '9')) {
				operand[k - 1] = operand[k];
			}
			else {		/* if we got here, it means that we have found a non digit char   */
				*hasErrors = 1;		/* after "#" and "-"(optionally) we allow only digits up to the end of operand   */
				printf("ERROR: Found chars which are not digits in a '#' operand.");
				return (char*)-1;
			}
		}
		operand[k - 1] = '\0';			/* The result of that segment is the number which is the operand  */
		*addressingType = 0;
		return operand;
	}
	if ((operand[0] == 'r') && (operand[1] >= '0') && (operand[1] <= '9') && ((int)strlen(operand) == 2)) {		/* is it addressing number 3?  */
		/* if we got here, the operand is exactly: "r0" or "r1" or ... or "r7"  */
		operand[0] = operand[1];
		operand[1] = '\0';			/* The result of that segment is the number which represents the register number  */
		*addressingType = 3;
		return operand;
	}	/* at this point we know that if the operand starts with a 'r', it can only be addressing 1  */
		/* we expect a label or a '&' and then a label  */
	k = 0;
	*addressingType = 1;		/* we assume that that's addressing number 1 and if we find '&' we change it to 2  */
	if (operand[0] == '&') {
		*addressingType = 2;
		k++;
	}
	if (((int)strlen(operand)) > (31 + k)) {
		*hasErrors = 1;		/* label's maximum length is 31 (32 including the '&')  */
		printf("ERROR: Label at operand exceeds maximum length.");
		return (char*)-1;
	}
	if (!(((operand[k] >= 'a') && (operand[k] <= 'z')) || ((operand[k] >= 'A') && (operand[k] <= 'Z')))) {
		*hasErrors = 1;		/* label must start with a letter  */
		printf("ERROR: Found a label operand which does not start with a letter.");
		return (char*)-1;
	}
	for (k++; (((operand[k] >= 'a') && (operand[k] <= 'z')) || ((operand[k] >= 'A') && (operand[k] <= 'Z')) || ((operand[k] >= '0') && (operand[k] <= '9'))); k++);
	if (k < (int)strlen(operand)) {
		*hasErrors = 1;			/* there are illegal chars in the label  */
		printf("ERROR: Found a label operand which contains illegal chars.");
		return (char*)-1;
	}
	if (operand[0] == '&') {						/* if there is '&' in the label beginning, move all chars 1 place left  */
		for (k = 1; k < (int)strlen(operand); k++)
			operand[k - 1] = operand[k];
		operand[k - 1] = '\0';
	}		/* at this point we know the addressing type of the operand  */
	return operand;
}

/* That function does casting from int to char */
char* intToChar(int addressingTypeInInt) {
	char* addressingTypeInChar;
	
	addressingTypeInChar = (char*)malloc(2 * sizeof(char));			/* creating a char version of the addressing type so we can check the mapping table  */
	addressingTypeInChar[1] = '\0';

	if (addressingTypeInInt == 0) {
		addressingTypeInChar[0] = '0';
		return addressingTypeInChar;
	}
	if (addressingTypeInInt == 1) {
		addressingTypeInChar[0] = '1';
		return addressingTypeInChar;
	}

	if (addressingTypeInInt == 2) {
		addressingTypeInChar[0] = '2';
		return addressingTypeInChar;
	}
	
	addressingTypeInChar[0] = '3';
	return addressingTypeInChar;
}

/* That function parses command lines */
int parseCommand(int i, char line[83], int address, mapping* mappingArray, char* lineType, image* codeImageHead) {
	int i2, k;		/* whether there is a label or not, we start to parse from line[i+1]. at this point we know that there is a VALID command and ' ' or '\t' or '\n' afterwards.  */
	int numberOfOperands = 0;
	int numberOfLines = 1;
	int commandIndexInMappingTable;
	int addressingTypeFirstOperand = -1;
	int addressingTypeSecondOperand = -1;
	int hasErrors;
	char* firstOperand = NULL;
	char* secondOperand = NULL;
	char* binaryMachineCodeToBeSent = NULL;
	char* addressingTypeInChar;

	for (i2 = i + 1; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);	/* find the first char of the command name  */
	for (commandIndexInMappingTable = 0; commandIndexInMappingTable < 16; commandIndexInMappingTable++) {		/* here we find the expected number of operands in accordance to the command name  */
		if (strcmp(mappingArray[commandIndexInMappingTable].command, lineType) == 0) {
			if (commandIndexInMappingTable < 5) {
				numberOfOperands = 2;
				break;
			}
			if (commandIndexInMappingTable < 14) {
				numberOfOperands = 1;
				break;
			}
			numberOfOperands = 0;
			break;
		}
	}		/* at this point we know that our command should require "numberOfOperands" operands, according to its name.  */
			/* we also know that the command index in the mapping table is "commandIndexInMappingTable"  */
	i2 = i2 + (int)strlen(lineType);			/* now line[i2] is the first char AFTER the command. we know that it is ' ' or '\t' or '\n'  */

	for (; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);	/* now line[i2] is the first char after the command which is not ' ' or '\t'  */
	if (line[i2] == ',') {
		printf("ERROR: A ',' comes right after the command.");
		return -1;
	}
	if ((line[i2] == '\n') || (line[i2] == '\0')) {				/* There are no operands in the line  */
		if (numberOfOperands != 0) {
			printf("ERROR: There are no operands in the line even though expected.");
			return -1;
		}
		binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, 0, 0, 0, 0, mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);	/* here we send the binaryMachineCode to be added to the codeImage linked list  */
		free(binaryMachineCodeToBeSent);
		return numberOfLines;
	}
	/* at this point line[i2] holds the first char of the operand  */
	firstOperand = getOperand(i2, line);
	i2 = i2 + (int)strlen(firstOperand);		/* note that we increase i2 here since "getFinalOperand" function might change operand length  */
	hasErrors = 0;
	firstOperand = getFinalOperand(firstOperand, &addressingTypeFirstOperand, &hasErrors);
	if (hasErrors == 1) {
		free(firstOperand);
		return -1;
	}		/* Found errors in the operand parsing  */
	/* at this point line[i2] is the first char after the first operand. it might be ' ' or '\t' or '\n' or ',' or end of file  */

	for (; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);	/* now line[i2] is the first char which is not ' ' or '\t' after the first operand  */

	if ((line[i2] == '\n') || (line[i2] == '\0')) {			/* there is one operand in the line  */
		if (numberOfOperands != 1) {
			printf("ERROR: Found 1 operand. Expected different number of operands.");
			return -1;
		}
		
		addressingTypeInChar = intToChar(addressingTypeFirstOperand);			/* creating a char version of the addressing type so we can check the mapping table */
		
		for (k = 0; k < (int)strlen(mappingArray[commandIndexInMappingTable].allowedDestAddressing); k++)
			if (addressingTypeInChar[0] == mappingArray[commandIndexInMappingTable].allowedDestAddressing[k])		/* look in the string of allowed addressing types  */
				break;			/* if the addressing type of our operand is allowed in accordance to the command we have, break  */
		if (k == (int)strlen(mappingArray[commandIndexInMappingTable].allowedDestAddressing)) {
			printf("ERROR: Addressing type %c is not allowed for %s command's operand.", addressingTypeInChar[0], lineType);
			free(addressingTypeInChar);
			return -1;
		}			/* at this point we know that the number of operands found is OK and that the addressing type is OK  */
		free(addressingTypeInChar);
		switch (addressingTypeFirstOperand) {
		case 0:			/* addressing type 0    */
			numberOfLines++;			/* increase numberOfLines to 2  */
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, 0, 0, addressingTypeFirstOperand, 0, mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
			addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);	/* add the first word to the code image  */
			free(binaryMachineCodeToBeSent);
			binaryMachineCodeToBeSent = concatinator(NULL, 0, 0, 0, 0, NULL, 4, atoi(firstOperand), 1);
			addImageLine(codeImageHead, address + 1, binaryMachineCodeToBeSent, numberOfLines);
			break;
		case 1:			/* addressing type 1  */
		case 2:			/* addressing type 2  */
			numberOfLines++;			/* increase numberOfLines to 2  */
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, 0, 0, addressingTypeFirstOperand, 0, mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
			addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);	/* add the first word to the code image */
			free(binaryMachineCodeToBeSent);
			binaryMachineCodeToBeSent = (char*)malloc(25 * sizeof(char));
			for (k = 0; k <= 23; k++) { binaryMachineCodeToBeSent[k] = '?'; }	/* we will fill it in the second run  */
			binaryMachineCodeToBeSent[24] = '\0';
			addImageLine(codeImageHead, address + 1, binaryMachineCodeToBeSent, numberOfLines);
			break;
		case 3:			/* addressing type 3  */
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, 0, 0, addressingTypeFirstOperand, atoi(firstOperand), mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
			addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);
			break;
		default:
			break;
		}
		if (firstOperand != NULL) free(firstOperand);
		free(binaryMachineCodeToBeSent);
		return numberOfLines;
	}
	/* at this point line[i2] must be a ',' */
	if (line[i2] != ',') {
		printf("ERROR: Expected ',' between the operands.");
		return -1;
	}
	/* at this point we know that line[i2] == ','  */
	for (i2 = i2 + 1; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);		/* look for the first char which is not ' ' or '\t' after the ','  */
	if (line[i2] == ',') {
		printf("ERROR: There are two ',' with no operand between them.");
		return -1;
	}
	if ((line[i2] == '\n') || (line[i2] == '\0')) {
		printf("ERROR: There is a ',' without operands afterwards.");
		return -1;
	}
	/* at this point we know that line[i2] is the first char of the second operand  */
	secondOperand = getOperand(i2, line);
	i2 = i2 + (int)strlen(secondOperand);    /* note that we increase i2 here since "getFinalOperand" function might change operand length  */
	hasErrors = 0;
	secondOperand = getFinalOperand(secondOperand, &addressingTypeSecondOperand, &hasErrors);
	if (hasErrors == 1) {
		free(secondOperand);
		return -1;
	}		/* Found errors in the operand parsing  */
	/* at this point line[i2] is the first char after the second operand */
	for (; ((line[i2] == ' ') || (line[i2] == '\t')); i2++);    /* inc i2 until line[i2] is the first char which is not ' ' or '\t' after the second operand  */
	if ((line[i2] != '\n') && (line[i2] != '\0')) {
		printf("ERROR: After the second operand only ' 's and '\t's are allowed, then '\n' or end of file.");
		return -1;
	}		/* there are 2 operands in the line */

	if (numberOfOperands != 2) {
		printf("ERROR: Found 2 operand. Expected different number of operands.");
		return -1;
	}

	addressingTypeInChar = intToChar(addressingTypeFirstOperand);			/* creating a char version of the addressing type so we can check the mapping table  */

	for (k = 0; k < (int)strlen(mappingArray[commandIndexInMappingTable].allowedSourceAddressing); k++)
		if (addressingTypeInChar[0] == mappingArray[commandIndexInMappingTable].allowedSourceAddressing[k])		/* look in the string of allowed addressing types  */
			break;			/* if the addressing type of our operand is allowed in accordance to the command we have, break  */
	if (k == (int)strlen(mappingArray[commandIndexInMappingTable].allowedSourceAddressing)) {
		printf("ERROR: Addressing type %c is not allowed for %s command's first operand.", addressingTypeInChar[0], lineType);
		free(addressingTypeInChar);
		return -1;
	}
	free(addressingTypeInChar);

	
	addressingTypeInChar = intToChar(addressingTypeSecondOperand);			/* creating a char version of the addressing type so we can check the mapping table  */
	
	for (k = 0; k < (int)strlen(mappingArray[commandIndexInMappingTable].allowedDestAddressing); k++)
		if (addressingTypeInChar[0] == mappingArray[commandIndexInMappingTable].allowedDestAddressing[k])		/* look in the string of allowed addressing types  */
			break;			/* if the addressing type of our operand is allowed in accordance to the command we have, break  */
	if (k == (int)strlen(mappingArray[commandIndexInMappingTable].allowedDestAddressing)) {
		printf("ERROR: Addressing type %c is not allowed for %s command's second operand.", addressingTypeInChar[0], lineType);
		free(addressingTypeInChar);
		return -1;
	}	/* at this point we know that the addressing types of both operands are OK and that the number of operands is OK  */
	free(addressingTypeInChar);
	/* at this point numberOfLines == 1  */
	if (addressingTypeFirstOperand < 2)
		numberOfLines++;
	if (addressingTypeSecondOperand < 2)
		numberOfLines++;
	/* at this point we know the numberOfLines that the command will require in the code image  */
	switch (addressingTypeFirstOperand) {
	case 0:			/* addressing type 0  */
		if (addressingTypeSecondOperand == 3)
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, addressingTypeFirstOperand, 0, addressingTypeSecondOperand, atoi(secondOperand), mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		else
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, addressingTypeFirstOperand, 0, addressingTypeSecondOperand, 0, mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);	/* add the first word to the code image  */
		free(binaryMachineCodeToBeSent);
		binaryMachineCodeToBeSent = concatinator(NULL, 0, 0, 0, 0, NULL, 4, atoi(firstOperand), 1);
		addImageLine(codeImageHead, address + 1, binaryMachineCodeToBeSent, numberOfLines);
		break;
	case 1:			/* addressing type 1  */
		if (addressingTypeSecondOperand == 3)
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, addressingTypeFirstOperand, 0, addressingTypeSecondOperand, atoi(secondOperand), mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		else
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, addressingTypeFirstOperand, 0, addressingTypeSecondOperand, 0, mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);	/* add the first word to the code image  */

		free(binaryMachineCodeToBeSent);
		binaryMachineCodeToBeSent = (char*)malloc(25 * sizeof(char));
		for (k = 0; k <= 23; k++) { binaryMachineCodeToBeSent[k] = '?'; }	/* we will fill it in the second run  */
		binaryMachineCodeToBeSent[24] = '\0';
		addImageLine(codeImageHead, address + 1, binaryMachineCodeToBeSent, numberOfLines);
		break;
	case 3:			/* addressing type 3  */
		if (addressingTypeSecondOperand == 3)
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, addressingTypeFirstOperand, atoi(firstOperand), addressingTypeSecondOperand, atoi(secondOperand), mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		else
			binaryMachineCodeToBeSent = concatinator(mappingArray[commandIndexInMappingTable].opcode, addressingTypeFirstOperand, atoi(firstOperand), addressingTypeSecondOperand, 0, mappingArray[commandIndexInMappingTable].funct, 4, 0, 0);
		addImageLine(codeImageHead, address, binaryMachineCodeToBeSent, numberOfLines);
		break;
	default:
		break;
	}
	if (firstOperand != NULL) free(firstOperand);
	free(binaryMachineCodeToBeSent);
	switch (addressingTypeSecondOperand) {
	case 0:
		binaryMachineCodeToBeSent = concatinator(NULL, 0, 0, 0, 0, NULL, 4, atoi(secondOperand), 1);
		addImageLine(codeImageHead, (address + numberOfLines - 1), binaryMachineCodeToBeSent, numberOfLines);		/* add the next word to the code image  */
		free(binaryMachineCodeToBeSent);
		break;
	case 1:
		binaryMachineCodeToBeSent = (char*)malloc(25 * sizeof(char));
		for (k = 0; k <= 23; k++) { binaryMachineCodeToBeSent[k] = '?'; }	/* we will fill it in the second run  */
		binaryMachineCodeToBeSent[24] = '\0';
		addImageLine(codeImageHead, (address + numberOfLines - 1), binaryMachineCodeToBeSent, numberOfLines);		/* add the next word to the code image  */
		free(binaryMachineCodeToBeSent);
		break;
	default:
		break;
	}
	if (secondOperand != NULL) free(secondOperand);
	return numberOfLines;
}

/* That function fills the '?' left after the first run  */
int fillGapsInCodeImage(symbolTable* symbolTableHead, image* ICP, image* lineToFill, char* label, int addressingType) {
	symbolTable* labelAtSymbolTable;		/* if that function returns -1, there is an error  */
											/* if that function returns 1, everything is fine  */
											/* if that function returns a different number, that's the address of the word that uses an external label  */

	for (labelAtSymbolTable = symbolTableHead; labelAtSymbolTable != NULL; labelAtSymbolTable = labelAtSymbolTable->next)
		if (strcmp(labelAtSymbolTable->symbol, label) == 0)		/* if we found the label we received (the operand) in the symbol table  */
			break;
	if (labelAtSymbolTable == NULL) {
		printf("ERROR: The symbol %s does not exist in the symbol table.", label);
		return -1;
	}	/* at this point the label of the operand is identical to: labelAtSymbolTable.symbol  */

	free(lineToFill->binaryMachineCode);
	if (addressingType == 1) {
		if (strcmp(labelAtSymbolTable->externalEntry, "external") == 0) {
			lineToFill->binaryMachineCode = concatinator(NULL, 0, 0, 0, 0, NULL, 1, labelAtSymbolTable->value, 1);
			return lineToFill->address;
		}
		lineToFill->binaryMachineCode = concatinator(NULL, 0, 0, 0, 0, NULL, 2, labelAtSymbolTable->value, 1);
		return 1;		/* fill ended successfully  */
	}
	/* at this point we know that addressingType == 2  */
	if (strcmp(labelAtSymbolTable->externalEntry, "external") == 0) {
		printf("ERROR: Attempting to use an external label with addressing type 2.");
		return -1;
	}
	lineToFill->binaryMachineCode = concatinator(NULL, 0, 0, 0, 0, NULL, 4, ((labelAtSymbolTable->value) - ICP->address), 1);
	return 1;
}

/* That function casts binary number to a hexadecimel number */
char* binToHex(char* binaryMachineCode, int i) {
	int num = 0;
	int ref = 8;
	int k;
	char* result;

	result = (char*)malloc(2 * sizeof(char));
	result[1] = '\0';

	for (k = 0; k < 4; k++) {		/* here we convert Binary to Hex  */
		if (binaryMachineCode[i + k] == '1')
			num = num + ref;
		ref = ref / 2;
	}

	switch (num) {
		case 0: result[0] = '0'; break;
		case 1: result[0] = '1'; break;
		case 2: result[0] = '2'; break;
		case 3: result[0] = '3'; break;
		case 4: result[0] = '4'; break;
		case 5: result[0] = '5'; break;
		case 6: result[0] = '6'; break;
		case 7: result[0] = '7'; break;
		case 8: result[0] = '8'; break;
		case 9: result[0] = '9'; break;
		case 10: result[0] = 'a'; break;
		case 11: result[0] = 'b'; break;
		case 12: result[0] = 'c'; break;
		case 13: result[0] = 'd'; break;
		case 14: result[0] = 'e'; break;
		case 15: result[0] = 'f'; break;
		default: result[0] = '0'; break;
	}
	return result;
}

/* That function prints the output files */
void createOutputFiles(char* fileName, image* dataImageHead, image* codeImageHead, symbolTable* symbolTableHead, externalList* externalListHead, int ICF, int DCF) {
	FILE* fptr = NULL;
	char* specificFileName;
	char* hexResult;
	image* imageIterator;
	symbolTable* symbolIterator;
	externalList* externalListIterator;
	int i, entriesFileIsOpened;

	specificFileName = fileName;
	specificFileName[strlen(fileName) - 2] = 'o';
	specificFileName[strlen(fileName) - 1] = 'b';

	fptr = fopen(specificFileName, "w");

	fprintf(fptr, "% 7d% -7d\n", ICF - 100, DCF);
	for (imageIterator = codeImageHead; imageIterator != NULL; imageIterator = imageIterator->next) {		/* printing the .ob file  */
		fprintf(fptr, "%07d ", imageIterator->address);
		for (i = 0; i <= 20; i = i + 4) {
			hexResult = binToHex(imageIterator->binaryMachineCode, i);
			fprintf(fptr, "%c", hexResult[0]);
			free(hexResult);
		}
		fprintf(fptr, "\n");
	}
	for (imageIterator = dataImageHead; imageIterator != NULL; imageIterator = imageIterator->next) {
		fprintf(fptr, "%07d ", imageIterator->address);
		for (i = 0; i <= 20; i = i + 4) {
			hexResult = binToHex(imageIterator->binaryMachineCode, i);
			fprintf(fptr, "%c", hexResult[0]);
			free(hexResult);
		}
		fprintf(fptr, "\n");
	}

	fclose(fptr);	/* close the .ob file  */


	entriesFileIsOpened = 0;		/* printing the .ent file  */
	for (symbolIterator = symbolTableHead; symbolIterator != NULL; symbolIterator = symbolIterator->next) {
		if (strcmp(symbolIterator->externalEntry, "entry") == 0) {
			if (entriesFileIsOpened == 0) {
				specificFileName = (char*)malloc((strlen(fileName) + 2) * sizeof(char));
				for (i = 0; i < ((int)strlen(fileName) - 2); i++) { specificFileName[i] = fileName[i]; }
				specificFileName[i] = 'e';
				specificFileName[i + 1] = 'n';
				specificFileName[i + 2] = 't';
				specificFileName[i + 3] = '\0';
				fptr = fopen(specificFileName, "w");
				free(specificFileName);
				entriesFileIsOpened = 1;
			}
			fprintf(fptr, "%s %07d\n", symbolIterator->symbol, symbolIterator->value);
			continue;
		}
	}
	if (entriesFileIsOpened == 1)
		fclose(fptr);

	if (externalListHead->address != -1) {			/* printing the .ext file  */
		specificFileName = (char*)malloc(((int)strlen(fileName) + 2) * sizeof(char));
		for (i = 0; i < ((int)strlen(fileName) - 2); i++) { specificFileName[i] = fileName[i]; }
		specificFileName[i] = 'e';
		specificFileName[i + 1] = 'x';
		specificFileName[i + 2] = 't';
		specificFileName[i + 3] = '\0';
		fptr = fopen(specificFileName, "w");
		free(specificFileName);
		for (externalListIterator = externalListHead; externalListIterator != NULL; externalListIterator = externalListIterator->next)
			fprintf(fptr, "%s %07d\n", externalListIterator->label, externalListIterator->address);
		fclose(fptr);
	}

}

/* That function frees the dynamically allocated memory */
void freeMemory(image* dataImageHead, image* codeImageHead, symbolTable* symbolTableHead, externalList* externalListhead) {
	image* imageIterator;
	symbolTable* symbolTableIterator;
	externalList* externalListIterator;

	while (dataImageHead != NULL) {
		imageIterator = dataImageHead;
		dataImageHead = dataImageHead->next;
		free(imageIterator);
	}

	while (codeImageHead != NULL) {
		imageIterator = codeImageHead;
		codeImageHead = codeImageHead->next;
		free(imageIterator);
	}

	while (symbolTableHead != NULL) {
		symbolTableIterator = symbolTableHead;
		symbolTableHead = symbolTableHead->next;
		free(symbolTableIterator);
	}

	while (externalListhead != NULL) {
		externalListIterator = externalListhead;
		externalListhead = externalListhead->next;
		free(externalListIterator);
	}
}
