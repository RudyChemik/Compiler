%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "semantic_analizer.h"
#include "assembly_generator.h"
#include <time.h>

MathStack mathStack;
ArrayStack arrayStack;

FILE *file;

extern Flow *flowTable;

extern ArrayVariable *arrayTable;
extern Variable *variableTable;
extern Variable *tempTable;
extern Variable *printTable;
extern PrivateVariable *privateVarsTable;
extern HighStatementIf *highStatementIfTable;
extern ForStatement *forStatementTable;

extern int privatesCounter;
extern int currentFlow;
extern int floorNum;

int started = 0;
int nested = 0;
int nestedCounter = 0;
int maxed = 0;
int mother = 0;

int lineCounter = 1;
int yylex();
void yyerror(const char *s);

typedef struct {
    int line;
    const char *msg;  
} errorTable;

typedef union {
    int ival;
    double dval;
    char *sval;
    char *stringval;
} YYSTYPE;

errorTable errors[200];
int errorCount = 0;

void createError(const char *error){
    if (errorCount < 200) {
        errorTable newError;
        newError.line = lineCounter;
        newError.msg = strdup(error);
        errors[errorCount] = newError;
        errorCount++;
    } else {
        fprintf(stderr, "Error: Too many errors, cannot store more.\n");
    }
}

#define YYSTYPE_IS_DECLARED 1
%}


%token NUM DNUM ASSIGN EQ DIF BIGGER SMALLER BEQ SEQ NEG BRAL BRAR HBRAL HBRAR DIV ADD SUB MULT INC DEC SEMICOLON INT VAR DOUBLE STR VNAME OTHER LONG IF ELSE FOR STRINGV DBRAL DBRAR PRINT COMMA MATH

%union {
    int ival;
    double dval;
    char *sval;
    char *stringval;
}

%type <ival> NUM expression term factor
%type <dval> DNUM doubleexpression doubleterm doublefactor
%type <sval> VNAME mathfactor
%type <stringval> stringexpression STRINGV
%%

program:
    statement_list
;

statement_list:
    statement
    | statement_list statement
;

statement:
    highstatement SEMICOLON { }
    | forstatement SEMICOLON {  }
    | printstatement SEMICOLON
    | declaration SEMICOLON
    | assignment SEMICOLON
    | mathematics SEMICOLON
;

printstatement:
    PRINT printvars { }
;
privateprint:
    PRINT privateprintvars
;

privateprintfor:
    PRINT privateprintvarsfor
;

privateprintvarsfor:
    VNAME { 
        VarType type = checkType($1);
        if(type != nonExisting){
            printInFor($1, type, forStatementTable);  
        }else{
            PrivateVarMO privatetype = privateCheck($1);
            if(privatetype.type != nonExisting){
                printInFor(privatetype.name, privatetype.type, forStatementTable);   
            }else{
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting value bruh \n");
                createError(errorMsg);
            }
        } 
    }
    //ext
;

privateprintvars:
    VNAME{ 
        VarType type = checkType($1);
        if(type != nonExisting){
            printVariableInIf(highStatementIfTable, type, $1, 1);  
        }else{
            PrivateVarMO privatetype = privateCheck($1);
            if(privatetype.type != nonExisting){
                printVariableInIf(highStatementIfTable, privatetype.type, $1, 0);  
            }else{
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting value bruh \n");
                createError(errorMsg);
            }
        }        
    }
    //no point
;

printvars:
    VNAME { 
        VarType type = checkType($1);
        if(type != nonExisting){
            if(type == integerValue){
            	Stack *stack = getCurrentFlowStack(&flowTable);
            	Stack *higherstack = getHigherFlowStack(&flowTable);
                printVariableValue(stack, integerValue, $1);
                if(stack != higherstack){
                    printVariableValue(higherstack, integerValue, $1);
                }
                //printVariableValue(higherstack, integerValue, $1);
            }
            if(type == doubleValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherstack = getHigherFlowStack(&flowTable);
            	printVariableValue(stack, doubleValue, $1);
            	if(stack != higherstack){
            	    printVariableValue(higherstack, doubleValue, $1);
            	}
            }
            if(type == stringValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherstack = getHigherFlowStack(&flowTable);
                printVariableValue(stack, stringValue, $1);
                if(stack != higherstack){
                    printVariableValue(higherstack, stringValue, $1);
                }
            }
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting value bruh \n");
            createError(errorMsg);
        }
    }
    | VNAME DBRAL factor DBRAR{
    	VarType type = checkType($1);
    	if(type == tableValue)
    	{
    	    ArrayTypeMO res = checkArrayType($1,$3);
	    if(res.code == 1){
	        Stack *stack = getCurrentFlowStack(&flowTable);
	        Stack *higherstack = getHigherFlowStack(&flowTable);
	        printArrayValue(stack, res.type, $1, $3);
	        if(stack != higherstack){
	            printArrayValue(higherstack, res.type, $1, $3);
	        }
	    }else{
	        char errorMsg[256];
	        if(res.code == 2){
	            snprintf(errorMsg, sizeof(errorMsg), "Array place escxeeded. \n");        
	        }else{
	            snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting array bruh \n");
	        }        
	    	createError(errorMsg);
	    }
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting array bruh \n");
            createError(errorMsg);
    	}
    }
    | stringexpression {
        const char *printName = initializePrint($1);
        Stack *stack = getCurrentFlowStack(&flowTable);
        Stack *higherstack = getHigherFlowStack(&flowTable);
        printVariableValue(stack, stringValue, printName);
        if(stack != higherstack){
	     printVariableValue(higherstack, stringValue, printName);       
	}
    }
    | printvars ADD VNAME { 
        VarType type = checkType($3);
        if(type != nonExisting){
            if(type == integerValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherstack = getHigherFlowStack(&flowTable);
                printVariableValue(stack, integerValue, $3);
                if(stack != higherstack){
	            printVariableValue(higherstack, integerValue, $3);
	        }
            }
            if(type == doubleValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherstack = getHigherFlowStack(&flowTable);
            	printVariableValue(stack, doubleValue, $3);
            	if(stack != higherstack){
	            printVariableValue(higherstack, doubleValue, $3);
	        }
            }
            if(type == stringValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherstack = getHigherFlowStack(&flowTable);
                printVariableValue(stack, stringValue, $3);
                if(stack != higherstack){
	            printVariableValue(higherstack, stringValue, $3);
	        }
            }
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting value bruh \n");
            createError(errorMsg);
        }
    }
    | printvars ADD VNAME DBRAL factor DBRAR {
    	VarType type = checkType($3);
    	if(type == tableValue)
    	{
    	    ArrayTypeMO res = checkArrayType($3,$5);
	    if(res.code == 1){
	        Stack *stack = getCurrentFlowStack(&flowTable);
	        Stack *higherstack = getHigherFlowStack(&flowTable);
	        printArrayValue(stack, res.type, $3, $5);
	        if(stack != higherstack){
	            printArrayValue(higherstack, res.type, $3, $5);
	        }
	    }else{
	        char errorMsg[256];
	        if(res.code == 2){
	            snprintf(errorMsg, sizeof(errorMsg), "Array place escxeeded. \n");        
	        }else{
	            snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting array bruh \n");
	        }        
	    	createError(errorMsg);
	    }
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant print nonexisting array bruh \n");
            createError(errorMsg);
    	}
    }
    | printvars ADD stringexpression { 
    	const char *printName = initializePrint($3);
    	Stack *higherstack = getHigherFlowStack(&flowTable);
    	Stack *stack = getCurrentFlowStack(&flowTable);
        printVariableValue(stack, stringValue, printName);
        if(stack != higherstack){
	      printVariableValue(higherstack, stringValue, printName);      
	} 
    }
;

private_members:
    highstatementstatement {  }
    | private_members highstatementstatement {  }
;

for_private_members:
    forstatementstatement {  }
    | for_private_members forstatementstatement {  }
;

forstatementstatement:
    privatedeclaration SEMICOLON {  }
    | privateassignmentfor SEMICOLON
    | privateprintfor SEMICOLON
;

highstatementstatement:
    privatedeclaration SEMICOLON
    | highstatement SEMICOLON {  }
    | privateassignment SEMICOLON
    | privateprint SEMICOLON
;



forstatement:
    FOR BRAL forinit SEMICOLON forcondition SEMICOLON forinc SEMICOLON BRAR HBRAL for_private_members HBRAR {
        redeclareInitialValues(privateVarsTable, forStatementTable);
        Stack *stack = getCurrentFlowStack(&flowTable);
        Stack *higherStack = getHigherFlowStack(&flowTable);
        if(stack != higherStack){
            startForLoop(higherStack, forStatementTable);
        }
        startForLoop(stack, forStatementTable);
    }
;

highstatement:
    IF BRAL condition BRAR HBRAL private_members HBRAR {
    if(nested > 0){
            nested--;
            privatesCounter--;
        }else{
            privatesCounter += nestedCounter;
            nested = 0;
            started = 0;
            nestedCounter = 0;
        }
    }
;

forinit:
    INT VNAME ASSIGN factor { 
        int correct = correctVariableName($2);
        if(correct == 0){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable used for FOR init alrd in use. \n");
            createError(errorMsg);
        }
        privatesCounter++;
        initializeInteger($2, (int)$4);
        initNewForLoop($2, (int)$4);
    }
    | VNAME { fprintf(stderr, "INIT old (ONLY INT!)\n"); }
;

forcondition:
    VNAME BIGGER factor {
        int correct = isMainVname($1);
    	if(correct == 0){
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Diffrent var name in for loop init. \n");
            createError(errorMsg);
    	}
    	
    	//add condition.
    	ForCondition condition;
    	condition.name = $1;
    	condition.value = $3;
    	condition.conditionType = fbigger;
    	initCondition(condition);
    }
    | VNAME SMALLER factor {
    	int correct = isMainVname($1);
    	if(correct == 0){
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Diffrent var name in for loop init. \n");
            createError(errorMsg);
    	}
    	
    	ForCondition condition;
    	condition.name = $1;
    	condition.value = $3;
    	condition.conditionType = fsmaller;
    	initCondition(condition);
    }
    | VNAME BEQ factor {
        int correct = isMainVname($1);
    	if(correct == 0){
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Diffrent var name in for loop init. \n");
            createError(errorMsg);
    	}
    	
    	ForCondition condition;
    	condition.name = $1;
    	condition.value = $3;
    	condition.conditionType = fequalOrBigger;
    	initCondition(condition);
    }
    | VNAME SEQ factor {
        int correct = isMainVname($1);
    	if(correct == 0){
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Diffrent var name in for loop init. \n");
            createError(errorMsg);
    	}
    	
    	ForCondition condition;
    	condition.name = $1;
    	condition.value = $3;
    	condition.conditionType = fequalOrSmaller;
    	initCondition(condition);
    }
;

//TODO

forinc:
    VNAME INC {    	
    	int correct = isMainVname($1);
    	if(correct == 0){
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Diffrent var name in for loop init. \n");
            createError(errorMsg);
    	}
    	//add condition finc.
    	initForIncType(finc);
    }
    | VNAME DEC {
        int correct = isMainVname($1);
    	if(correct == 0){
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Diffrent var name in for loop init. \n");
            createError(errorMsg);
    	}
    	//add condition fdec.
    	initForIncType(fdec);
    }
;

condition:
    VNAME EQ VNAME {
       VarType type = checkType($1);
       VarType typesecond = checkType($3);
       
       if(type == nonExisting || typesecond == nonExisting || type == tableValue || typesecond == tableValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type. \n");
            createError(errorMsg);
       }else{
           if(type == typesecond){
               privatesCounter++;
               if(started == 1){
                   nested++;
                   floorNum++;
                   nestedCounter++;
          
                   const char *loopName = generateLoopName();
                   startIfHigher(highStatementIfTable,equal, $1, type ,$3, loopName);
                   createIfStatement(loopName);
               }else{
               started = 1;
               floorNum = 0;
               const char *loopName = generateLoopName();
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *startingStack = getStartingStack(&flowTable);               
               startIfInMain(stack, equal, $1,type,$3, loopName);
		if(mother > 0){
               	   startIfInMain(startingStack, equal, $1,type,$3, loopName);
               	   if(mother > 1){
               	       for(int i = 1; i < currentFlow; i++){
               	       	   Stack *higherStack = getStackBasedOnFlow(i, &flowTable);
               	           startIfInMain(higherStack, equal, $1,type,$3, loopName);
               	       }
               	   }
               }
               createIfStatement(loopName);
               currentFlow++;
               initNewFlow(&flowTable);
               mother++;
               }
           }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant perform check for diffrent types. \n");
            createError(errorMsg);
           }
       }
       
    }
    | VNAME DIF VNAME {
       VarType type = checkType($1);
       VarType typesecond = checkType($3);
       
       if(type == nonExisting || typesecond == nonExisting || type == tableValue || typesecond == tableValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type. \n");
            createError(errorMsg);
       }else{
           if(type == typesecond){
               privatesCounter++;
               if(started == 1){
                   nested++;
                   floorNum++;
                   nestedCounter++;
          
                   const char *loopName = generateLoopName();
                   startIfHigher(highStatementIfTable,diffrent, $1, type ,$3, loopName);
                   createIfStatement(loopName);
               }else{
               started = 1;
               floorNum = 0;
               const char *loopName = generateLoopName();
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *startingStack = getStartingStack(&flowTable);               
               startIfInMain(stack, diffrent, $1,type,$3, loopName);
		if(mother > 0){
               	   startIfInMain(startingStack, diffrent, $1,type,$3, loopName);
               	   if(mother > 1){
               	       for(int i = 1; i < currentFlow; i++){
               	           fprintf(stderr ,"ITERATOR::: %d", i);
               	       	   Stack *higherStack = getStackBasedOnFlow(i, &flowTable);
               	           startIfInMain(higherStack, diffrent, $1,type,$3, loopName);
               	       }
               	   }
               }
               createIfStatement(loopName);
               currentFlow++;
               initNewFlow(&flowTable);
               mother++;
               }
           }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant perform check for diffrent types. \n");
            createError(errorMsg);
           }
       }
    }
    | VNAME BIGGER VNAME {
       VarType type = checkType($1);
       VarType typesecond = checkType($3);
       
       if(type == nonExisting || typesecond == nonExisting || type == tableValue || typesecond == tableValue || type == stringValue || typesecond == stringValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type. \n");
            createError(errorMsg);
       }else{
           if(type == typesecond){
               privatesCounter++;
               if(started == 1){
                   nested++;
                   floorNum++;
                   nestedCounter++;
          
                   const char *loopName = generateLoopName();
                   startIfHigher(highStatementIfTable,bigger, $1, type ,$3, loopName);
                   createIfStatement(loopName);
               }else{
               started = 1;
               floorNum = 0;
               const char *loopName = generateLoopName();
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *startingStack = getStartingStack(&flowTable);               
               startIfInMain(stack, bigger, $1,type,$3, loopName);
		if(mother > 0){
               	   startIfInMain(startingStack, bigger, $1,type,$3, loopName);
               	   if(mother > 1){
               	       for(int i = 1; i < currentFlow; i++){
               	           fprintf(stderr ,"ITERATOR::: %d", i);
               	       	   Stack *higherStack = getStackBasedOnFlow(i, &flowTable);
               	           startIfInMain(higherStack, bigger, $1,type,$3, loopName);
               	       }
               	   }
               }
               createIfStatement(loopName);
               currentFlow++;
               initNewFlow(&flowTable);
               mother++;
               }
           }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant perform check for diffrent types. \n");
            createError(errorMsg);
           }
       }
    }
    | VNAME SMALLER VNAME {
       VarType type = checkType($1);
       VarType typesecond = checkType($3);
       
       if(type == nonExisting || typesecond == nonExisting || type == tableValue || typesecond == tableValue || type == stringValue || typesecond == stringValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type. \n");
            createError(errorMsg);
       }else{
           if(type == typesecond){
               privatesCounter++;
               if(started == 1){
                   nested++;
                   floorNum++;
                   nestedCounter++;
          
                   const char *loopName = generateLoopName();
                   startIfHigher(highStatementIfTable,smaller, $1, type ,$3, loopName);
                   createIfStatement(loopName);
               }else{
               started = 1;
               floorNum = 0;
               const char *loopName = generateLoopName();
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *startingStack = getStartingStack(&flowTable);               
               startIfInMain(stack, smaller, $1,type,$3, loopName);
		if(mother > 0){
               	   startIfInMain(startingStack, smaller, $1,type,$3, loopName);
               	   if(mother > 1){
               	       for(int i = 1; i < currentFlow; i++){
               	           fprintf(stderr ,"ITERATOR::: %d", i);
               	       	   Stack *higherStack = getStackBasedOnFlow(i, &flowTable);
               	           startIfInMain(higherStack, smaller, $1,type,$3, loopName);
               	       }
               	   }
               }
               createIfStatement(loopName);
               currentFlow++;
               initNewFlow(&flowTable);
               mother++;
               }
           }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant perform check for diffrent types. \n");
            createError(errorMsg);
           }
       }
    }
    | VNAME BEQ VNAME {
       VarType type = checkType($1);
       VarType typesecond = checkType($3);
       
       if(type == nonExisting || typesecond == nonExisting || type == tableValue || typesecond == tableValue || type == stringValue || typesecond == stringValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type. \n");
            createError(errorMsg);
       }else{
           if(type == typesecond){
               privatesCounter++;
               if(started == 1){
                   nested++;
                   floorNum++;
                   nestedCounter++;
          
                   const char *loopName = generateLoopName();
                   startIfHigher(highStatementIfTable,equalOrBigger, $1, type ,$3, loopName);
                   createIfStatement(loopName);
               }else{
               started = 1;
               floorNum = 0;
               const char *loopName = generateLoopName();
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *startingStack = getStartingStack(&flowTable);               
               startIfInMain(stack, equalOrBigger, $1,type,$3, loopName);
		if(mother > 0){
               	   startIfInMain(startingStack, equalOrBigger, $1,type,$3, loopName);
               	   if(mother > 1){
               	       for(int i = 1; i < currentFlow; i++){
               	           fprintf(stderr ,"ITERATOR::: %d", i);
               	       	   Stack *higherStack = getStackBasedOnFlow(i, &flowTable);
               	           startIfInMain(higherStack, equalOrBigger, $1,type,$3, loopName);
               	       }
               	   }
               }
               createIfStatement(loopName);
               currentFlow++;
               initNewFlow(&flowTable);
               mother++;
               }
           }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant perform check for diffrent types. \n");
            createError(errorMsg);
           }
       }
    }
    | VNAME SEQ VNAME {
       VarType type = checkType($1);
       VarType typesecond = checkType($3);
       
       if(type == nonExisting || typesecond == nonExisting || type == tableValue || typesecond == tableValue || type == stringValue || typesecond == stringValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type. \n");
            createError(errorMsg);
       }else{
           if(type == typesecond){
               privatesCounter++;
               if(started == 1){
                   nested++;
                   floorNum++;
                   nestedCounter++;
          
                   const char *loopName = generateLoopName();
                   startIfHigher(highStatementIfTable,equalOrSmaller, $1, type ,$3, loopName);
                   createIfStatement(loopName);
               }else{
               started = 1;
               floorNum = 0;
               const char *loopName = generateLoopName();
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *startingStack = getStartingStack(&flowTable);               
               startIfInMain(stack, equalOrSmaller, $1,type,$3, loopName);
		if(mother > 0){
               	   startIfInMain(startingStack, equalOrSmaller, $1,type,$3, loopName);
               	   if(mother > 1){
               	       for(int i = 1; i < currentFlow; i++){
               	           fprintf(stderr ,"ITERATOR::: %d", i);
               	       	   Stack *higherStack = getStackBasedOnFlow(i, &flowTable);
               	           startIfInMain(higherStack, equalOrSmaller, $1,type,$3, loopName);
               	       }
               	   }
               }
               createIfStatement(loopName);
               currentFlow++;
               initNewFlow(&flowTable);
               mother++;
               }
           }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Cant perform check for diffrent types. \n");
            createError(errorMsg);
           }
       }
    }
;

privatedeclaration:
    INT VNAME ASSIGN expression {
    	int correct = correctVariableName($2);
    	if(correct == 1){
    	    Value value;
    	    value.integerValue = $4;
    	    initializePrivatesVar($2, integerValue, value);
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. (higher) \n", $2);
            createError(errorMsg);
    	}
    }
    | STR VNAME ASSIGN stringexpression { 
        int correct = correctVariableName($2);
    	if(correct == 1){
    	    Value value;
    	    value.stringValue = $4;
    	    initializePrivatesVar($2, stringValue, value);
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. (higher) \n", $2);
            createError(errorMsg);
    	}
    }
    | DOUBLE VNAME ASSIGN expression { 
        int correct = correctVariableName($2);
    	if(correct == 1){
    	    Value value;
    	    value.doubleValue = (double)$4;
    	    initializePrivatesVar($2, doubleValue, value);
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. (higher) \n", $2);
            createError(errorMsg);
    	}
    }
    | DOUBLE VNAME ASSIGN doubleexpression { 
        int correct = correctVariableName($2);
    	if(correct == 1){
    	    Value value;
    	    value.doubleValue = (double)$4;
    	    initializePrivatesVar($2, doubleValue, value);
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. (higher) \n", $2);
            createError(errorMsg);
    	}
    }
    | INT VNAME { 
        int correct = correctVariableName($2);
    	if(correct == 1){
    	    Value value;
    	    value.integerValue = 0;
    	    initializePrivatesVar($2, integerValue, value);
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. (higher) \n", $2);
            createError(errorMsg);
    	}
    }
    | DOUBLE VNAME { 
        int correct = correctVariableName($2);
    	if(correct == 1){
    	    Value value;
    	    value.doubleValue = (double)0;
    	    initializePrivatesVar($2, doubleValue, value);
    	}else{
    	    char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. (higher) \n", $2);
            createError(errorMsg);
    	}
    }    
;

declaration:
    INT VNAME ASSIGN expression { 
        int correct = correctVariableName($2);
        if(correct == 1){
            initializeInteger($2,$4);      
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);
        }
    }  
    | STR VNAME ASSIGN stringexpression { 
        int correct = correctVariableName($2);
        if(correct == 1){
            initializeString($2,$4);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);
        }
    }
    | DOUBLE VNAME ASSIGN expression { 
        int correct = correctVariableName($2);
        if(correct == 1){
            initializeDouble($2,(double)$4);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);
        }
    }
    | DOUBLE VNAME ASSIGN doubleexpression { 
        int correct = correctVariableName($2);
        if(correct == 1){
            initializeDouble($2,$4);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);
        }
    }
    | INT VNAME { 
        int correct = correctVariableName($2);
        if(correct == 1){
            initializeInteger($2,0);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);      
        }  
    }
    | DOUBLE VNAME { 
        int correct = correctVariableName($2);
        if(correct == 1){
            initializeDouble($2,(double)0);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);      
        } 
    }
    | INT VNAME DBRAL factor DBRAR {
        int res = correctVariableName($2);
        if(res == 1){
            initializeArray($2, integerArray, $4);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);      
        }
    }
    | DOUBLE VNAME DBRAL factor DBRAR {
         int res = correctVariableName($2);
         if(res == 1){
             initializeArray($2, doubleArray, $4);
         }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);      
         }
    }
    | STR VNAME DBRAL factor DBRAR {
         int res = correctVariableName($2);
         if(res == 1){
             initializeArray($2, stringArray, $4);
         }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);      
         }       
    }
    | INT VNAME DBRAL factor DBRAR HBRAL arrayinitialization HBRAR {
    	int correct = correctVariableName($2);
    	if(correct == 1){
    	    int res = initializeArrayWithValues(&arrayStack, $2, integerValue ,$4);
    	    if(res == 0){
    	    	char errorMsg[256];
    	    	snprintf(errorMsg, sizeof(errorMsg), "Exceeded size bb; \n");
    	    	createError(errorMsg);
    	    }else if(res == 2){
    	        char errorMsg[256];
    	    	snprintf(errorMsg, sizeof(errorMsg), "WRONG TYPE!!!!1 \n");
    	    	createError(errorMsg);
    	    }
    	}else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);	
    	}
    }
    | DOUBLE VNAME DBRAL factor DBRAR HBRAL arrayinitialization HBRAR {
	int correct = correctVariableName($2);
    	if(correct == 1){
    	    int res = initializeArrayWithValues(&arrayStack, $2, doubleValue ,$4);
    	    if(res == 0){
    	    	char errorMsg[256];
    	    	snprintf(errorMsg, sizeof(errorMsg), "Exceeded size bb; \n");
    	    	createError(errorMsg);
    	    }else if(res == 2){
    	        char errorMsg[256];
    	    	snprintf(errorMsg, sizeof(errorMsg), "WRONG TYPE!!!!1 \n");
    	    	createError(errorMsg);
    	    }
    	}else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);	
    	}
    }
    | STR VNAME DBRAL factor DBRAR HBRAL arrayinitialization HBRAR {
        int correct = correctVariableName($2);
        if(correct == 1){
            int res = initializeArrayWithValues(&arrayStack, $2, stringValue ,$4);
    	    if(res == 0){
    	    	char errorMsg[256];
    	    	snprintf(errorMsg, sizeof(errorMsg), "Exceeded size bb; \n");
    	    	createError(errorMsg);
    	    }else if(res == 2){
    	        char errorMsg[256];
    	    	snprintf(errorMsg, sizeof(errorMsg), "WRONG TYPE!!!!1 \n");
    	    	createError(errorMsg);
    	    }
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s already declared. \n", $2);
            createError(errorMsg);
        }
    }
;

arrayinitialization:
    factor { 
    	ArrayVariableSt stackN;
    	Value valuer;
    	valuer.integerValue = $1;
    	
    	stackN.type = integerValue;
    	stackN.value = valuer;
        pushArrayValue(&arrayStack, stackN);
    }
    | doublefactor {
    	ArrayVariableSt stackN;
    	Value valuer;
    	valuer.doubleValue = $1;
    	
    	stackN.type = doubleValue;
    	stackN.value = valuer;
        pushArrayValue(&arrayStack, stackN);
    }
    | stringexpression {
    	ArrayVariableSt stackN;
    	Value valuer;
    	valuer.stringValue = $1;
    	
    	stackN.type = stringValue;
    	stackN.value = valuer;
        pushArrayValue(&arrayStack, stackN);
    }
    | arrayinitialization COMMA doublefactor {
    	ArrayVariableSt stackN;
    	Value valuer;
    	valuer.doubleValue = $3;
    	
    	stackN.type = doubleValue;
    	stackN.value = valuer;
        pushArrayValue(&arrayStack, stackN);
    }
    | arrayinitialization COMMA factor {
     	ArrayVariableSt stackN;
    	Value valuer;
    	valuer.integerValue = $3;
    	
    	stackN.type = integerValue;
    	stackN.value = valuer;
        pushArrayValue(&arrayStack, stackN);   
    }
    | arrayinitialization COMMA stringexpression{
    	ArrayVariableSt stackN;
    	Value valuer;
    	valuer.stringValue = $3;
    	
    	stackN.type = stringValue;
    	stackN.value = valuer;
        pushArrayValue(&arrayStack, stackN);
    }
;

privateassignmentfor:
    VNAME ASSIGN expression {
       VarType type = checkType($1);
       if(type == nonExisting){
           PrivateVarMO private = privateCheck($1);
           if(private.type != nonExisting){
               if(private.type == integerValue){
                   Value value;
                   value.integerValue = $3;                   
                   forAssigning(private.name, private.type, value, forStatementTable);
               }else if(private.type == doubleValue){
                   Value value;
                   value.doubleValue = (double)$3;
                   forAssigning(private.name, private.type, value, forStatementTable);
               }else{
                   char errorMsg[256];
            	   snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	   createError(errorMsg);
               }
           }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Variable %s does not exist. xddd \n", $1);
            	createError(errorMsg);
           }
       }else{
           if(type == integerValue){
               Value value;
               value.integerValue = $3;
               forAssigning($1, integerValue, value, forStatementTable);
           }else if(type == doubleValue){
               Value value;
               value.doubleValue = (double)$3;
               forAssigning($1, doubleValue, value, forStatementTable);
           }else{
           	char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	createError(errorMsg);
           }
       }
    }
    | VNAME ASSIGN doubleexpression {
    	VarType type = checkType($1);
        if(type == nonExisting){
            PrivateVarMO private = privateCheck($1);
            if(private.type != nonExisting){
                if(private.type == doubleValue){
                   Value value;
                   value.doubleValue = (double)$3;
                   forAssigning(private.name, private.type, value, forStatementTable);
                }else{
                	char errorMsg[256];
            		snprintf(errorMsg, sizeof(errorMsg), "wrong type. \n");
            		createError(errorMsg);
                }
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "VAR DOES NOT EXIST. \n");
            	createError(errorMsg);
            }            
        }else{
            if(type == doubleValue){
               Value value;
               value.doubleValue = (double)$3;
               forAssigning($1, doubleValue, value, forStatementTable);
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	createError(errorMsg);
            }
        }
    }
    | VNAME ASSIGN stringexpression {
        VarType type = checkType($1);
        if(type == nonExisting){
            PrivateVarMO private = privateCheck($1);
            if(private.type != nonExisting){
                if(private.type == stringValue){
                   Value value;
                   value.stringValue = $3;
                   forAssigning(private.name, private.type, value, forStatementTable);
                }else{
                	char errorMsg[256];
            		snprintf(errorMsg, sizeof(errorMsg), "wrong type. \n");
            		createError(errorMsg);
                }
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "VAR DOES NOT EXIST. \n");
            	createError(errorMsg);
            }            
        }else{
            if(type == stringValue){
               Value value;
               value.stringValue = $3;
               forAssigning($1, stringValue, value, forStatementTable);
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	createError(errorMsg);
            }
        }
    }
    | VNAME ASSIGN VNAME {
    	VarType type = checkType($1);
        VarType secondtype = checkType($3);
        if(type == nonExisting && secondtype == nonExisting){

        	PrivateVarMO private = privateCheck($1);
        	PrivateVarMO privatesec = privateCheck($3);
        	if(private.type == nonExisting && privatesec.type == nonExisting || private.type == nonExisting ||   privatesec.type == nonExisting){
        	    char errorMsg[256];
            	    snprintf(errorMsg, sizeof(errorMsg), "Does not exisxt. \n");
            	    createError(errorMsg);
        	}else{
        	    if(private.type == privatesec.type){        	    	
        	    	    forAssignTwoVariables(private.name,privatesec.name, private.type, forStatementTable);     	    	
        	    }else{
        	        char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "DIFF YTPES. prv \n");
            	        createError(errorMsg);
        	    }
        	}
        	}
        if(type == nonExisting && secondtype != nonExisting){
        	PrivateVarMO private = privateCheck($1);
        	if(private.type == nonExisting){
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "1st var does not exist. \n");
            	        createError(errorMsg);
        	}
        	
        	if(private.type == secondtype){
        		forAssignTwoVariables(private.name,$3, private.type, forStatementTable);  
        	}else{
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "DIFF YTPES. cant assign\n");
            	        createError(errorMsg);
        	}        	
        }        
        if(secondtype == nonExisting && type != nonExisting){
        	PrivateVarMO private = privateCheck($3);
        	if(private.type == nonExisting){
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "1st var does not exist. \n");
            	        createError(errorMsg);
        	}
        	
        	if(private.type == type){
        		forAssignTwoVariables($1,private.name, private.type, forStatementTable);  
        	}else{
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "DIFF YTPES. cant assign\n");
            	        createError(errorMsg);
        	}
        }
    }
    | VNAME DBRAL factor DBRAR ASSIGN VNAME {
    	ArrayTypeMO arrayType = checkArrayType($1, $3);
        VarType secondtype = checkType($6);
                
        if(arrayType.type == nonExisting && secondtype == nonExisting || arrayType.type == nonExisting && secondtype == 	tableValue){
		char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Check cuz at least one does not exist. \n");
                createError(errorMsg);               
        }
        
        const char *assigningVarName;
        assigningVarName = $6;
                
        if(secondtype == nonExisting && arrayType.type != nonExisting){
            PrivateVarMO private = privateCheck($6);
            secondtype = private.type;
            assigningVarName = private.name;
        }
        
        if(arrayType.code == 1){
            if(arrayType.type == integerArray && secondtype == integerValue){           
            	assignArrayFromVarFor($1, assigningVarName, integerValue, $3, forStatementTable);            	
            }else if(arrayType.type == doubleArray && secondtype == doubleValue){            
            	assignArrayFromVarFor($1, assigningVarName, doubleValue, $3, forStatementTable);        	
            }else if(arrayType.type == stringArray && secondtype == stringValue){
            	assignArrayFromVarFor($1, assigningVarName, stringValue, $3, forStatementTable);
            }
        }else{
            char errorMsg[256];
            if(arrayType.code == 0) { snprintf(errorMsg, sizeof(errorMsg), "Array does not exist. \n"); }
            else { snprintf(errorMsg, sizeof(errorMsg), "Exceeded array size. \n"); }
            createError(errorMsg);  
        }
        
    }
;

privateassignment:
    VNAME ASSIGN expression { 
       VarType type = checkType($1);
       if(type == nonExisting){
           PrivateVarMO private = privateCheck($1);
           if(private.type != nonExisting){
               if(private.type == integerValue){
                   Value value;
                   value.integerValue = $3;                   
                   assignVariableInIfPrivate(highStatementIfTable, private.type, value, $1);
               }else if(private.type == doubleValue){
                   Value value;
                   value.doubleValue = (double)$3;
                   assignVariableInIfPrivate(highStatementIfTable, private.type, value, $1);
               }else{
                   char errorMsg[256];
            	   snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	   createError(errorMsg);
               }
           }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Variable %s does not exist. xddd \n", $1);
            	createError(errorMsg);
           }
       }else{
           if(type == integerValue){
               Value value;
               value.integerValue = $3;
               assignVariableInIfPublic(highStatementIfTable, type, value, $1);
           }else if(type == doubleValue){
               Value value;
               value.doubleValue = (double)$3;
               assignVariableInIfPublic(highStatementIfTable, type, value, $1);
           }else{
           	char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	createError(errorMsg);
           }
       }
    } 
    | VNAME ASSIGN doubleexpression { 
        VarType type = checkType($1);
        if(type == nonExisting){
            PrivateVarMO private = privateCheck($1);
            if(private.type != nonExisting){
                if(private.type == doubleValue){
                   Value value;
                   value.doubleValue = (double)$3;
                   assignVariableInIfPrivate(highStatementIfTable, private.type, value, $1);
                }else{
                	char errorMsg[256];
            		snprintf(errorMsg, sizeof(errorMsg), "wrong type. \n");
            		createError(errorMsg);
                }
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "VAR DOES NOT EXIST. \n");
            	createError(errorMsg);
            }            
        }else{
            if(type == doubleValue){
               Value value;
               value.doubleValue = (double)$3;
               assignVariableInIfPublic(highStatementIfTable, type, value, $1);
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	createError(errorMsg);
            }
        }
    }
    | VNAME ASSIGN stringexpression { 
        VarType type = checkType($1);
        if(type == nonExisting){
            PrivateVarMO private = privateCheck($1);
            if(private.type != nonExisting){
                if(private.type == stringValue){
                   Value value;
                   value.stringValue = $3;
                   assignVariableInIfPrivate(highStatementIfTable, private.type, value, $1);
                }else{
                	char errorMsg[256];
            		snprintf(errorMsg, sizeof(errorMsg), "wrong type. \n");
            		createError(errorMsg);
                }
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "VAR DOES NOT EXIST. \n");
            	createError(errorMsg);
            }            
        }else{
            if(type == stringValue){
               Value value;
               value.stringValue = $3;
               assignVariableInIfPublic(highStatementIfTable, type, value, $1);
            }else{
                char errorMsg[256];
            	snprintf(errorMsg, sizeof(errorMsg), "Cant assign to this type. \n");
            	createError(errorMsg);
            }
        }
    }
    | VNAME ASSIGN VNAME { 
        VarType type = checkType($1);
        VarType secondtype = checkType($3);
        if(type == nonExisting && secondtype == nonExisting){
        	PrivateVarMO private = privateCheck($1);
        	PrivateVarMO privatesec = privateCheck($3);
        	if(private.type == nonExisting && privatesec.type == nonExisting || private.type == nonExisting ||   privatesec.type == nonExisting){
        	    char errorMsg[256];
            	    snprintf(errorMsg, sizeof(errorMsg), "Does not exisxt. \n");
            	    createError(errorMsg);
        	}else{
        	    if(private.type == privatesec.type){        	    	
        	    	    assignTwoVariablesInIf(highStatementIfTable,private.type,$1,$3, 0,0);     	    	
        	    }else{
        	        char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "DIFF YTPES. \n");
            	        createError(errorMsg);
        	    }
        	}
        	}
        if(type == nonExisting){
        	PrivateVarMO private = privateCheck($1);
        	if(private.type == nonExisting){
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "1st var does not exist. \n");
            	        createError(errorMsg);
        	}
        	
        	if(private.type == secondtype){
        		assignTwoVariablesInIf(highStatementIfTable,private.type,$1,$3, 0,1);  
        	}else{
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "DIFF YTPES. cant assign\n");
            	        createError(errorMsg);
        	}        	
        }        
        if(secondtype == nonExisting){
        	PrivateVarMO private = privateCheck($3);
        	if(private.type == nonExisting){
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "1st var does not exist. \n");
            	        createError(errorMsg);
        	}
        	
        	if(private.type == type){
        		assignTwoVariablesInIf(highStatementIfTable,private.type,$1,$3, 1,0);  
        	}else{
        		char errorMsg[256];
            	        snprintf(errorMsg, sizeof(errorMsg), "DIFF YTPES. cant assign\n");
            	        createError(errorMsg);
        	}
        }

    }      
;

assignment:
    VNAME ASSIGN expression {
        VarType type = checkType($1);
        if(type != nonExisting){
            if(type == integerValue){                
                Value value;
                value.integerValue = $3;
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
                if(stack != higherStack){
                    assignVariable(higherStack, $1, integerValue, value);  
                }
                assignVariable(stack, $1, integerValue, value);         			
            }else if(type == doubleValue){
               Value value;
               value.doubleValue = (double)$3;
               const char *tempName = initializeTemp(doubleValue, value);
               Stack *stack = getCurrentFlowStack(&flowTable);
               Stack *higherStack = getHigherFlowStack(&flowTable);
               assignWithTemp(stack, $1, tempName, doubleValue);
               if(stack != higherStack){
                   assignWithTemp(higherStack, $1, tempName, doubleValue);
               }
            }else{
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Can't assign integer to this type. \n");
                createError(errorMsg);
            }
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s does not exist. \n", $1);
            createError(errorMsg);       
        }
    }
    | VNAME ASSIGN stringexpression{
        VarType type = checkType($1);
        if(type != nonExisting){
            if(type == stringValue){
            	Value value;
            	value.stringValue = $3;
            	const char *temp = initializeTemp(stringValue, value);
            	Stack *stack = getCurrentFlowStack(&flowTable);
            	Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignWithTemp(stack, $1, temp, stringValue);
            	if(stack != higherStack){
            	    assignWithTemp(higherStack, $1, temp, stringValue);
            	}
            }else{
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Cant assign string to this... \n");
                createError(errorMsg);  
            }
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s does not exist. \n", $1);
            createError(errorMsg);          
        }
    }
    | VNAME ASSIGN doubleexpression{ 
    	VarType type = checkType($1);
        if(type != nonExisting){
            if(type == doubleValue){
            	Value value;
            	value.doubleValue = $3;
            	const char *temp = initializeTemp(doubleValue, value);
            	Stack *stack = getCurrentFlowStack(&flowTable);
            	Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignWithTemp(stack, $1, temp, doubleValue);
            	if(stack != higherStack){
            	    assignWithTemp(higherStack, $1, temp, doubleValue);
            	}
            }else{
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Cant assign doubleValue to this... \n");
                createError(errorMsg);  
            }
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Variable with name %s does not exist. \n", $1);
            createError(errorMsg);          
        }
    }
    | VNAME ASSIGN VNAME {
        VarType type = checkType($1);
        VarType secondtype = checkType($3);
        if(type == nonExisting || secondtype == nonExisting || type == tableValue || secondtype == tableValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Check cuz at least one does not exist. \n");
            createError(errorMsg);   
        }
        
        if(type == secondtype){
            if(type == integerValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignTwoVariables(stack, $1, $3, integerValue);
            	if(stack != higherStack){
            	    assignTwoVariables(stack, $1, $3, integerValue);
            	}
            }else if(type == doubleValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignTwoVariables(stack, $1, $3, doubleValue);
            	if(stack != higherStack){
            	    assignTwoVariables(stack, $1, $3, doubleValue);
            	}
            }else if(type == stringValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignTwoVariables(stack, $1, $3, stringValue);
            	if(stack != higherStack){
            	    assignTwoVariables(stack, $1, $3, stringValue);
            	}
            }
        }
    }
    | VNAME DBRAL factor DBRAR ASSIGN VNAME{
        ArrayTypeMO arrayType = checkArrayType($1, $3);
        VarType secondtype = checkType($6);        
        if(arrayType.type == nonExisting || secondtype == nonExisting || secondtype == tableValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Check cuz at least one does not exist. \n");
            createError(errorMsg);   
        }
        
	if(arrayType.code == 1){
            if(arrayType.type == integerArray && secondtype == integerValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignArrayValueFromVariable(stack, $1, $6, integerValue, $3);
            	if(stack != higherStack){
            	    assignArrayValueFromVariable(higherStack, $1, $6, integerValue, $3);
            	}
            }else if(arrayType.type == doubleArray && secondtype == doubleValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignArrayValueFromVariable(stack, $1, $6, doubleValue, $3);
            	if(stack != higherStack){
            	    assignArrayValueFromVariable(higherStack, $1, $6, doubleValue, $3);
            	}
            }else if(arrayType.type == stringArray && secondtype == stringValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignArrayValueFromVariable(stack, $1, $6, stringValue, $3);
            	if(stack != higherStack){
            	    assignArrayValueFromVariable(higherStack, $1, $6, stringValue, $3);
            	}
            }
        }else{
            char errorMsg[256];
            if(arrayType.code == 0) { snprintf(errorMsg, sizeof(errorMsg), "Array does not exist. \n"); }
            else { snprintf(errorMsg, sizeof(errorMsg), "Exceeded array size. \n"); }
            createError(errorMsg);  
        }
        
    }
    | VNAME ASSIGN VNAME DBRAL factor DBRAR{
        ArrayTypeMO arrayType = checkArrayType($3, $5);
        VarType firsttype = checkType($1);
        
        if(arrayType.type == nonExisting || firsttype == nonExisting || firsttype == tableValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Check cuz at least one does not exist. \n");
            createError(errorMsg);   
        }
        
        if(arrayType.code == 1){
            if(arrayType.type == integerArray && firsttype == integerValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignArrayValueToVariable(stack, $1, $3, integerValue, $5);
            	if(stack != higherStack){
            	    assignArrayValueToVariable(higherStack, $1, $3, integerValue, $5);
            	}
            }else if(arrayType.type == doubleArray && firsttype == doubleValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignArrayValueToVariable(stack, $1, $3, doubleValue, $5);
            	if(stack != higherStack){
            	    assignArrayValueToVariable(higherStack, $1, $3, doubleValue, $5);
            	}
            }else if(arrayType.type == stringArray && firsttype == stringValue){
                Stack *stack = getCurrentFlowStack(&flowTable);
                Stack *higherStack = getHigherFlowStack(&flowTable);
            	assignArrayValueToVariable(stack, $1, $3, stringValue, $5);
            	if(stack != higherStack){
            	    assignArrayValueToVariable(higherStack, $1, $3, stringValue, $5);
            	}
            }
        }else{
            char errorMsg[256];
            if(arrayType.code == 0) { snprintf(errorMsg, sizeof(errorMsg), "Array does not exist. \n"); }
            else { snprintf(errorMsg, sizeof(errorMsg), "Exceeded array size. \n"); }
            createError(errorMsg);  
        }
    }
    | VNAME ASSIGN mathematics {
        VarType type = checkType($1);
        if(type != nonExisting && type == integerValue){
            Stack *stack = getCurrentFlowStack(&flowTable);
            Stack *higherStack = getHigherFlowStack(&flowTable);
            assignMathToVariable(stack, $1);
            if(stack != higherStack){
            	assignMathToVariable(higherStack, $1);
            }
            //CLEAR MATH STACK.
            clearMathStack(&mathStack);
        }else{
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Wrong type or variable does not exist. \n");
            createError(errorMsg);
        }
    }
    | VNAME DBRAL factor DBRAR ASSIGN expression {  }
    | VNAME DBRAL factor DBRAR ASSIGN doubleexpression {  }
    | VNAME DBRAL factor DBRAR ASSIGN stringexpression {  }
;

mathematics:
    MATH BRAL mathexpression BRAR {
        //compute.        
        Stack *mStack = computeMathStack(&mathStack);
        Stack *currentFlowStack = getCurrentFlowStack(&flowTable);
        pushMathExpressionToCurrentFlowStack(mStack, currentFlowStack);
    }
;

mathexpression:
    mathexpression ADD mathfactor {
        VarType type = checkType($3);
        if(type != integerValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Did that only for ints. \n");
            createError(errorMsg); 
        }
        addInitialsToMathStack(&mathStack,$3, addExp);
    }
    | mathexpression SUB mathfactor { 
        VarType type = checkType($3);
        if(type != integerValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Did that only for ints. \n");
            createError(errorMsg); 
        }
        addInitialsToMathStack(&mathStack,$3, subExp);
    }
    | mathexpression MULT mathfactor {
        VarType type = checkType($3);
        if(type != integerValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Did that only for ints. \n");
            createError(errorMsg); 
        }
        addInitialsToMathStack(&mathStack,$3, multExp);
    }
    | mathexpression DIV mathfactor {
        VarType type = checkType($3);
        if(type != integerValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Did that only for ints. \n");
            createError(errorMsg); 
        }
        addInitialsToMathStack(&mathStack,$3, divExp);
    }
    | mathfactor {
        VarType type = checkType($1);
        if(type != integerValue){
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "Did that only for ints. \n");
            createError(errorMsg); 
        }
        addInitialsToMathStack(&mathStack,$1, none);
    }
;

mathfactor:
    VNAME { $$ = $1; }
;

expression:
    expression ADD term { $$ = $1 + $3; }
    | expression SUB term { $$ = $1 - $3; }
    | term { $$ = $1; }
;

doubleexpression:
    doubleexpression ADD doubleterm { $$ = $1 + $3; }
    | doubleexpression SUB doubleterm { $$ = $1 - $3; }
    | doubleterm { $$ = $1; }
;

term:
    term MULT factor { $$ = $1 * $3; }
    | term DIV factor { $$ = $1 / $3; }
    | factor { $$ = $1; }
;

doubleterm:
    doubleterm MULT doublefactor { $$ = $1 * $3; }
    | doubleterm DIV doublefactor { $$ = $1 / $3; }
    | doublefactor { $$ = $1; }
;


factor:
    NUM { $$ = $1; }
    | SUB NUM { $$ = -$2; }
;

doublefactor:
    DNUM { $$ = $1; }
    | SUB DNUM { $$ = -$2; }
;

stringexpression:
    STRINGV { 
        normalizeString($1); 
        $$ = $1; 
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

//Main
int main(int argc, char **argv) {
    clock_t start, startAsm,startComp, end, endAsm, endComp;
    double cpu_time_used, cpu_time_usedAsm, cpu_time_usedComp;
    
    fprintf(stderr, "Compilation prcoess started...\n");
    start = clock();
    
    initFlow(&flowTable);
    initArrayStack(&arrayStack);
    initMathStack(&mathStack);
    
    yyparse();
    
    if(errorCount > 0){
        fprintf(stderr, "Compilation process failed. \n");
        fprintf(stderr, "---------------------------------- \n");
        for(int i = 0; i < errorCount; i++){
            fprintf(stderr, "Error on line %d. %s \n",errors[i].line, errors[i].msg);
        }
        return 1;
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    fprintf(stderr, "Semantic check run sucessfully. Time: %f s\n", cpu_time_used);
    fprintf(stderr, "Generating assembly... \n");
    startAsm = clock();
    
    file = fopen("assembly.asm", "w");
    if (file == NULL) {
            perror("Error creating file");
            return 1;
        }
        
    createSection(file, "data");
    dumpFifoQue(file);
    dumpVariables(file, variableTable);
    dumpArrays(file, arrayTable);
    dumpVariables(file, tempTable);
    dumpVariables(file, printTable);
    dumpPrivateVars(file, privateVarsTable);
    createStart(file);
     
    dumpFlow(file, flowTable);      
    initializePrints(file);
    dumpIfs(file, highStatementIfTable); 
    dumpFors(file, forStatementTable);
    
    endAsm = clock();
    cpu_time_usedAsm = ((double) (endAsm - startAsm)) / CLOCKS_PER_SEC;
    
    fprintf(stderr, "Assembly file generated. Time: %f s\n", cpu_time_usedAsm);
    fprintf(stderr ,"----------------------------------- \n");
    fclose(file);
    int res = system("nasm -f elf64 -o assembly.o assembly.asm");
    if (res != 0) { fprintf(stderr, "Error compiling assembly file.\n"); return 1; }
    
    res = system("ld -o assembly assembly.o");
    if (res != 0) { fprintf(stderr, "Error compiling assembly file.\n"); return 1; }
    
    res = system("./assembly");
    if(res != 0) { fprintf(stderr, "Error compiling. \n"); return 1; }
    startComp = clock();
    
    fprintf(stderr ,"----------------------------------- \n");
    endComp = clock();
    cpu_time_usedComp = ((double) (endComp - startComp)) / CLOCKS_PER_SEC;
    fprintf(stderr, "Compilation ended. Time: %f s\n", cpu_time_usedComp);
    
    return 0;
}

