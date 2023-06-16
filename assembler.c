#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auxiliaries.h"

/* Function declaration */
void firstRun(char* fileName, mapping* mappingArray, image* dataImageHead, image* codeImageHead, symbolTable* symbolTableHead);
void secondRun(char* fileName, image* dataImageHead, image* codeImageHead, symbolTable* symbolTableHead, mapping* mappingArray, int ICF, int DCF);

int main(int argc, char* argv[]) {
	mapping mappingArray[16];
	image* dataImageHead = NULL; 
	image* codeImageHead = NULL;
	symbolTable* symbolTableHead= NULL;	
	char* fileName;
	int i, k;

	if (argc < 2)  {		/* command line should be: assembler <file1> ... */
		printf("ERROR: Invalid arguments. Should be: ./assembler <file1> <file2> ...    .Exiting\n");
		exit(1);
	}

	createMappingTable(mappingArray);		/* Here we create the mapping array, which holds data for each of the 16 commands */

	for (i = 1; i<argc; i++) {
		fileName = (char*)malloc(((int)strlen(argv[i]) + 4) * sizeof(char));
		for (k = 0; (k < (int)strlen(argv[i])); k++) { fileName[k] = argv[i][k]; }
		fileName[k] = '.';
		fileName[k + 1] = 'a';
		fileName[k + 2] = 's';
		fileName[k + 3] = '\0';							/* here we add .as at the end of each input file */
		
		dataImageHead = (image*)malloc(sizeof(image));
		dataImageHead->address = -1;
		dataImageHead->binaryMachineCode = "NULL";
		dataImageHead->NumberOfWords = -1;
		dataImageHead->next = NULL;
		codeImageHead = (image*)malloc(sizeof(image));
		codeImageHead->address = -1;
		codeImageHead->binaryMachineCode = "NULL";
		codeImageHead->NumberOfWords = -1;
		codeImageHead->next = NULL;
		symbolTableHead = (symbolTable*)malloc(sizeof(symbolTable));
		symbolTableHead->symbol = "NULL";
		symbolTableHead->codeData = "NULL";
		symbolTableHead->externalEntry = "NULL";
		symbolTableHead->value = -1;
		symbolTableHead->next = NULL;
		
		firstRun(fileName, mappingArray, dataImageHead, codeImageHead, symbolTableHead);	/* That line launches the first run (review) of the source code */
		
		free(fileName);
		freeMemory(dataImageHead, codeImageHead, symbolTableHead, (externalList*)NULL);		/* we free the memory after each run  */
	}  
	return 1;
}

/* That function runs the first run on the assembly code  */
void firstRun(char* fileName, mapping* mappingArray, image* dataImageHead, image* codeImageHead, symbolTable* symbolTableHead) {
	FILE* fptr;
	char line[83];
	char* readResult;
	char* lineType;			
	char* label;
	char* labelAddingResult;
	char* externParsingResult;
	int IC, DC, ICF, DCF, hasLabel, i, hasErrors, lineNumberAtSourceCode, L, DL, k;
	symbolTable* symbolIterator;
	image* dataIterator;

	fptr = fopen(fileName, "r");
	if (fptr == NULL) {
		printf("ERROR: Cannot open file. Exiting\n");
		freeMemory(dataImageHead, codeImageHead, symbolTableHead, (externalList*)NULL);
		exit(1);
	}
	readResult = fgets(line,255,fptr);
	if (!readResult) {
		printf("ERROR: file is empty. Exiting\n");
		freeMemory(dataImageHead, codeImageHead, symbolTableHead, (externalList*)NULL);
		exit(1);
	}

	lineNumberAtSourceCode = 1;
	IC = 100;
	DC = 0;
	hasErrors = 0;			/* Indicates if there are errors. 0 = no errors.  */
	while (readResult) {
		hasLabel = 0;		/* hasLabel = 0 means that there is no Label in the current line.  */
		for (i = 0; ((line[i] == ' ') || (line[i] == '\t')); i++);		/* check if it is an empty/comment line. find the first char which is not ' ' or '\t'  */
		if ((line[i] == '\n') || (line[0] == ';') || (line[i] == '\0')) {	
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}										/* at this point we know that the line is not empty and not a comment line.  */
		
		i = getLabel(line);			/* check if there is a label in the line.  */
		if (i == -2) {
			printf("ERROR: label exceeds maximum length of 31 chars. Line number %d\n",lineNumberAtSourceCode);
			hasErrors = 1;
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}
		if (i > 0) { hasLabel = 1; }   /* if there is a label: turn on the flag "hasLabel"  */
		lineType = getLineType(line, mappingArray, i);		/* get the type of that line. only possibilities at this point are instruction or command  */
		if (lineType[0] == 'E') {
			printf("%s Line number %d\n", lineType, lineNumberAtSourceCode);
			hasErrors = 1;
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}
		if ((hasLabel == 1)&&((strcmp(lineType,".entry") == 0) || (strcmp(lineType,".extern") == 0))) {		/* entry/extern lines should not have a label. it is a warning, not an error  */
			printf("WARNING: line number %d is %s line and has a label. label ignored\n", lineNumberAtSourceCode, lineType);
			hasLabel = 0;					/* if there is a label in such a line, ignore the label.  */
		}
		if ((strcmp(lineType,".data") == 0) || (strcmp(lineType,".string") == 0)) {
			if (hasLabel) {
				label = (char*)malloc((i + 1) * sizeof(char));
				for (k = 0; k < i; k++) { label[k] = line[k]; }
				label[i] = '\0';
				labelAddingResult = addSymbolLine(symbolTableHead, label, DC, "data","0");
				free(label);
				if (labelAddingResult[0] == 'E') {
					printf("%s Line number %d\n", labelAddingResult, lineNumberAtSourceCode);
					hasErrors = 1;
					lineNumberAtSourceCode++;
					readResult = fgets(line, 255, fptr);
					continue;
				}	
			}
			if (strcmp(lineType,".data") == 0) {
				DL = parseDataInstruction(i, line, dataImageHead, DC);
			}
			else {
				DL = parseStringInstruction(i, line, dataImageHead, DC);
			}
				
			if (DL == -1) {
				printf("Illegal %s line. Line number %d\n", lineType, lineNumberAtSourceCode);
				hasErrors = 1;
			}
			else {
				DC = DC + DL;		/* add to DC the number of data we added to the data image	  */
			} 
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}
		if ((strcmp(lineType,".extern") == 0) || (strcmp(lineType,".entry") == 0)) {
			if (strcmp(lineType,".extern") == 0) {
				externParsingResult = parseExternInstruction(i, line);
				if (externParsingResult == NULL) {
					printf("Illegal .extern line. Line number %d\n", lineNumberAtSourceCode);
					hasErrors = 1;
				}
				else {
					labelAddingResult = addSymbolLine(symbolTableHead, externParsingResult, 0, "0", "external");
					if (labelAddingResult[0] == 'E') {
						printf("%s Line number %d\n", labelAddingResult, lineNumberAtSourceCode);
						hasErrors = 1;
					}
				}	
			}		/* in both cases: .extern and .entry, read the next line now  */
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;	
		} /* at this point we know that it is a command line  */
		if (hasLabel) {
			label = (char*)malloc((i + 1) * sizeof(char));
			for (k = 0; k < i; k++) { label[k] = line[k]; }
			label[i] = '\0';
			labelAddingResult = addSymbolLine(symbolTableHead, label, IC, "code", "0");
			free(label);
			if (labelAddingResult[0] == 'E') {
				printf("%s Line number %d\n", labelAddingResult, lineNumberAtSourceCode);
				hasErrors = 1;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			}
		}

		L = parseCommand(i, line, IC, mappingArray, lineType,codeImageHead);
		if (L == -1) {
			printf(" Line number %d\n", lineNumberAtSourceCode);
			hasErrors = 1;
		} else  
			IC = IC + L;

		lineNumberAtSourceCode++;
		readResult = fgets(line, 255, fptr);
	}
	fclose(fptr);
	if (hasErrors == 1) {
		printf("First run ended with errors. Exiting\n");
		freeMemory(dataImageHead, codeImageHead, symbolTableHead, (externalList*)NULL);
		exit(1);
	}
	ICF = IC;
	DCF = DC;
	for (symbolIterator = symbolTableHead; symbolIterator != NULL; symbolIterator = symbolIterator->next)		/* add ICF to the data symbols values  */
		if (strcmp(symbolIterator->codeData, "data") == 0)
			symbolIterator->value = symbolIterator->value + ICF;
	for (dataIterator = dataImageHead; dataIterator != NULL; dataIterator = dataIterator->next)					/* add ICF to the data image lines addresses  */
		dataIterator->address = dataIterator->address + ICF;
	
	secondRun(fileName, dataImageHead, codeImageHead, symbolTableHead, mappingArray, ICF, DCF);	/* That run launches the second run (review of the source code) */
}

/* That function runs the second run on the assembly code */
void secondRun(char* fileName, image* dataImageHead, image* codeImageHead, symbolTable* symbolTableHead, mapping* mappingArray, int ICF, int DCF) {
	FILE* fptr;
	char line[83];
	char* readResult;
	char* lineType;
	char* labelArgument;
	char* operand;
	int lineNumberAtSourceCode, hasErrors, i, k, labelBeginning, commandIndexInMappingTable, placeHolder, addressingTypeOperand;
	int fillResult, length;
	int  numberOfOperands = -1;
	image* ICP = codeImageHead;
	symbolTable* symbolIterator;
	externalList* externalListHead;

	externalListHead = (externalList*)malloc(sizeof(externalList));		/* here we initiate the first node in the list that contains the addresses of the words which use  */
	externalListHead->address = -1;										/* external labels  */
	externalListHead->label = NULL;
	externalListHead->next = NULL;

	fptr = fopen(fileName, "r");		/* open source file again  */
	readResult = fgets(line,255,fptr);
	
	lineNumberAtSourceCode = 1;
	hasErrors = 0;			/* Indicates if there are errors. 0 = no errors.  */
	while (readResult) {
		for (i = 0; ((line[i] == ' ') || (line[i] == '\t')); i++);		/* check if it is an empty/comment line. find the first char which is not ' ' or '\t'  */
		if ((line[i] == '\n') || (line[0] == ';') || (line[i] == '\0')) {
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}										/* at this point we know that the line is not empty and not a comment line.  */

		i = getLabel(line);			/* if there is a label, line[i] == ':' . otherwise i == -1  */
		lineType = getLineType(line, mappingArray, i);		/* get the type of that line. only possibilities at this point are instruction or command  */
		if ((strcmp(lineType,".data") == 0) || (strcmp(lineType,".string") == 0) || (strcmp(lineType,".extern") == 0)) {		
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;						/* if that's a ".data" or ".string" or ".extern" instruction, continue to the next line  */
		}
		if (strcmp(lineType,".entry") == 0) {
			for (; (line[i] != '.'); i++);			/* we know that there is a '.' because we know that it is a ".entry" line  */
			for (i = i + 6; ((line[i] == ' ') || (line[i] == '\t')); i++);	/* inc i such that line[i] is the first char after ".entry". we know that this char is ' ' or '\t'. then inc i more  */
			/* at this point line[i] is the first char which is NOT ' ' or '\t' after the word ".entry"  */
			if ((line[i] == '\n') || (line[i] == '\0')) {
				printf("ERROR: Found .entry instruction without an argument. Line number %d\n", lineNumberAtSourceCode);
				hasErrors = 1;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			}	/* at this point line[i] is the first character of the label argument. now we check if it is a legal label  */

			if (!(((line[i] >= 'a') && (line[i] <= 'z')) || ((line[i] >= 'A') && (line[i] <= 'Z')))) {		/* If the label argument doesn't start with a-z or A-Z that's an illegal label argument  */
				printf("ERROR: Found .entry instruction with an illegal label argument. Line number %d\n", lineNumberAtSourceCode);
				hasErrors = 1;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			}	/* at this point we know that the label argument starts with a valid char  */
			labelBeginning = i;		/* let line[labelBeginning] be the first char in the label argument  */
			for (i++; (((line[i] >= 'a') && (line[i] <= 'z')) || ((line[i] >= 'A') && (line[i] <= 'Z')) || ((line[i] >= '0') && (line[i] <= '9'))); i++);
			/* at this point line[i] is the first illegal char after the label    */
			if ((line[i] != '\n') && (line[i] != '\0') && (line[i] != ' ') && (line[i] != '\t')) {		/* we only allow '\n' or end of file or ' ' or '\t' after a label  */
				printf("ERROR: Found .entry instruction with an illegal char at label argument. Line number %d\n", lineNumberAtSourceCode);
				hasErrors = 1;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			}
			for (k = i; ((line[k] == ' ') || (line[k] == '\t')); k++);	/* now we skip ' ' and '\t' and hope to meet '\n' or '\0'  */
			/* at this point line[i] should be '\n' or '\0'. otherwise there is more than one label argument in the line  */
			if ((line[k] != '\n') && (line[k] != '\0')) {
				printf("ERROR: Found .entry instruction with more than one argument. Line number %d\n", lineNumberAtSourceCode);
				hasErrors = 1;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			}
			i--;	/* after this line, line[labelBeginning] holds the first char of the label and line[i] holds the last char of the label  */
			if ((i - labelBeginning) > 30) {
				printf("ERROR: Found .entry instruction with a too long label argument. Line number %d\n", lineNumberAtSourceCode);
				hasErrors = 1;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			} /* at this point we know that the label is legal  */

			length = (i - labelBeginning) + 2;
			labelArgument = (char*)malloc(length * sizeof(char));					/* create a char* with the label so we will be able to compare it  */
			for (k = labelBeginning; k <= i; k++) { labelArgument[k - labelBeginning] = line[k]; }
			labelArgument[k - labelBeginning] = '\0';
		
			for (symbolIterator = symbolTableHead; symbolIterator != NULL; symbolIterator = symbolIterator->next) {
				if (strcmp(symbolIterator->symbol, labelArgument) == 0) {
					symbolIterator->externalEntry = "entry";
					break;
				}
			}
			free(labelArgument);
			if (symbolIterator == NULL) {
				printf("ERROR: label argument of an .entry instruction is not defined in the source file. Line number %d\n", lineNumberAtSourceCode);
				hasErrors = 1;
			}
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}		/* at this point we know that this is a command line. line[i] is before the command  */
		
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

		if (numberOfOperands == 0) {		/* those commands does not use addressing types 1 or 2  */
			ICP = ICP->next;		/* go to the next node in the code image  */
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}		/* at this point we know that the command has 1 or 2 operands  */

		for (i = i + 1; ((line[i] == ' ') || (line[i] == '\t')); i++);		/* after this line, line[i] is the first char of the command name  */
		i = i + strlen(lineType);		/* after this line, line[i] is the first char after the command name  */
		for (; ((line[i] == ' ') || (line[i] == '\t')); i++);  /* after this line, line[i] is the first char of the first operand (and we know that it exists)  */
		operand = getOperand(i, line);
		i = i + strlen(operand);		/* note that we increase i here since "getFinalOperand" function might change operand length    */
		operand = getFinalOperand(operand, &addressingTypeOperand, &placeHolder);
		
		if ((addressingTypeOperand == 1) || (addressingTypeOperand == 2)) {
			fillResult = fillGapsInCodeImage(symbolTableHead, ICP, ICP->next, operand, addressingTypeOperand);
			if (fillResult == -1) {				/* there is an error. move to next line  */
				hasErrors = 1;
				printf(" Line number %d\n", lineNumberAtSourceCode);
				for (k = ICP->NumberOfWords; k > 0; k--)
					ICP = ICP->next;
				lineNumberAtSourceCode++;
				readResult = fgets(line, 255, fptr);
				continue;
			}	/* at this point we know that the fill of the first operand was ended successfully  */
			if (fillResult > 1)		/* if fillResult is greater than 1, it represent the address of the word which uses an external label. add it to the list  */
				addExternalListItem(externalListHead, operand, fillResult);
		} /* at this point we are done with the first operand. line[i2] is the first char after the first operand. it might be ' ' or '\t' or '\n' or ',' or end of file  */
		
		if (numberOfOperands == 1) {			/* if there is only one operand in the command line, we have finished filling. go to the next line  */
			for (k = ICP->NumberOfWords; k > 0; k--)
				ICP = ICP->next;
			lineNumberAtSourceCode++;
			readResult = fgets(line, 255, fptr);
			continue;
		}	/* at this point we know that there are 2 operands in that command line  */
		
		for (; ((line[i] == ' ') || (line[i] == '\t')); i++);	/* after this line, line[i] is the ',' between the 2 operands  */
		for (i = i + 1; ((line[i] == ' ') || (line[i] == '\t')); i++);		/* after this line, line[i] is the first char of the second operand (and we know that it exists)  */
		operand = getOperand(i, line);
		operand = getFinalOperand(operand, &addressingTypeOperand, &placeHolder);

		if ((addressingTypeOperand == 1) || (addressingTypeOperand == 2)) {
			if (ICP->NumberOfWords == 2)
				fillResult = fillGapsInCodeImage(symbolTableHead, ICP, ICP->next, operand, addressingTypeOperand);
			else
				fillResult = fillGapsInCodeImage(symbolTableHead, ICP, ICP->next->next, operand, addressingTypeOperand);
			if (fillResult == -1) {				/* there is an error. move to next line  */
				hasErrors = 1;
				printf(" Line number %d\n", lineNumberAtSourceCode);
			}
			if (fillResult > 1)		/* if fillResult is greater than 1, it represent the address of the word which uses an external label. add it to the list  */
				addExternalListItem(externalListHead, operand, fillResult);
		}
		for (k = ICP->NumberOfWords; k > 0; k--)
			ICP = ICP->next;
		lineNumberAtSourceCode++;
		readResult = fgets(line, 255, fptr);
	}
	/* source file ended  */
	fclose(fptr);
	if (hasErrors) {
		printf("Second run ended with errors. Exiting\n");
		freeMemory(dataImageHead, codeImageHead, symbolTableHead, externalListHead);
		exit(1);
	}
	/* next we will build the output files   */
	createOutputFiles(fileName, dataImageHead, codeImageHead, symbolTableHead,externalListHead, ICF, DCF);


}
