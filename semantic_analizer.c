#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic_analizer.h"
#include "uthash.h"
#include "assembly_generator.h"

Flow *flowTable = NULL;

ArrayVariable *arrayTable = NULL;
Variable *variableTable = NULL;
Variable *tempTable = NULL;
Variable *printTable = NULL;
PrivateVariable *privateVarsTable = NULL;

ForStatement *forStatementTable = NULL;
HighStatementIf *highStatementIfTable = NULL;

int tempCounter = 0;
int printCounter = 0;
int privatesCounter = 0;
int currentFlow = 0;
int floorNum = 0;
//STACK//

int currentMathQue = 0;

#include <stdio.h>
#include <string.h>

// ----- HELPERS ----- //
const char *generateForLoopName(){
	int size = snprintf(NULL, 0, "ForLoop%d", privatesCounter) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "ForLoop%d", privatesCounter);
	const char *fixedName = tempName;
	return fixedName;
}

const char *generateNormalizedPrivateName(const char *name){
	int size = snprintf(NULL, 0, "%s%d",name ,privatesCounter) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "%s%d",name ,privatesCounter);
	const char *fixedName = tempName;
	return fixedName;
}

const char *generateLoopName(){

	int size = snprintf(NULL, 0, "ifstatement%d", privatesCounter) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "ifstatement%d", privatesCounter);
	const char *fixedName = tempName;
	return fixedName;

}

const char *generateFlowName(){
	int size = snprintf(NULL, 0, "_Start%d", currentFlow) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "_Start%d", currentFlow);
	const char *fixedName = tempName;
	return fixedName;
}

void normalizeString(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src != '"') {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';
}
//-------- END ------- //

//--------- TBC ------//

int correctVariableName(const char *name) {
    Variable *v;

    HASH_FIND_STR(variableTable, name, v);
    if (v == NULL) {
        return 1;
    } else {
        return 0;
    }
}

ArrayTypeMO checkArrayType(const char *name, int place){
	ArrayVariable *v;
	ArrayTypeMO returnStruct;

    HASH_FIND_STR(arrayTable, name, v);
    if (v == NULL) {
        returnStruct.type = nonExistingg;
        returnStruct.code = 0;
    } else {
        if(v->arraySize >= place){
        	returnStruct.type = v->arrayType;
        	returnStruct.code = 1;
        }else{
        	returnStruct.type = nonExistingg;
        	returnStruct.code = 2;
        }
    }
    
	return returnStruct;
}

PrivateVarMO privateCheck(const char *name) {
    PrivateVariable *private;
    const char *normalizedName = generateNormalizedPrivateName(name);
    PrivateVarMO res;

    HASH_FIND_STR(privateVarsTable, normalizedName, private);
    if (private == NULL) {
        res.type = nonExisting;
        res.name = normalizedName;
    } else {
        res.type = private->type;
        res.name = normalizedName;
    }
    return res;
}

VarType checkType(const char *name){
    Variable *v;

    HASH_FIND_STR(variableTable, name, v);
    if (v == NULL) {
        return nonExisting;
    } else {
        return v->type;
    }
}

//-------- END ------- //


//START OF FLOWS//
void initFlow(Flow **flowTable){
    Flow *f = (Flow *)malloc(sizeof(Flow));
    f->id = currentFlow;
    f->flowName = "_Start";
    
    Stack *stack = (Stack *)malloc(sizeof(Stack));   
    initStack(stack);
    
    AssemblyInstruction instruction;
    snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "%s:", f->flowName);
    push(stack, instruction);
    
    f->stack = stack;
    HASH_ADD_INT(*flowTable, id, f);  // Use id for hashing
}

void initNewFlow(Flow **flowTable) {
    Flow *f = (Flow *)malloc(sizeof(Flow));
    f->id = currentFlow;
    f->flowName = generateFlowName();
    
    Stack *stack = (Stack *)malloc(sizeof(Stack));   
    initStack(stack);
    
    AssemblyInstruction instruction;
    snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "%s:", f->flowName);
    push(stack, instruction);
    
    f->stack = stack;
    HASH_ADD_INT(*flowTable, id, f);
}

void iterateFlows(Flow *flowTable) {
    Flow *current_flow, *tmp;

    HASH_ITER(hh, flowTable, current_flow, tmp) {
        printf("Flow ID: %d\n", current_flow->id);
        printf("Flow Name: %s\n", current_flow->flowName);
        
        StackNode *node = current_flow->stack->top;
        while (node != NULL) {
            printf("Instruction: %s\n", node->instruction.instruction);
            node = node->next;
        }
        printf("\n");
    }
}

Stack *getStackBasedOnFlow(int i, Flow **flowTable){
	Flow *flow;
	HASH_FIND_INT(*flowTable, &i, flow);
    if (flow != NULL) {
        return flow->stack;
    }
    return NULL;
}

Stack *getStartingStack(Flow **flowTable){
	Flow *flow;
	int index = 0;
	HASH_FIND_INT(*flowTable, &index, flow);
    if (flow != NULL) {
        return flow->stack;
    }
    return NULL;
}

Stack *getHigherFlowStack(Flow **flowTable){
    Flow *flow;
    int higherFlowId;
    if (currentFlow == 0) {
        higherFlowId = currentFlow;
    } else {
        higherFlowId = currentFlow - 1;
    }
        
    HASH_FIND_INT(*flowTable, &higherFlowId, flow);
    if (flow != NULL) {
        return flow->stack;
    }
    return NULL;
}

Stack *getCurrentFlowStack(Flow **flowTable) {
    Flow *flow;
    HASH_FIND_INT(*flowTable, &currentFlow, flow);
    if (flow != NULL) {
        return flow->stack;
    }
    return NULL;
}

//END OF FLOWS!S//

// ------ MAIN INITS ----- //
const char *initializePrint(const char *value){
    Value storage;
	Variable *v = (Variable *)malloc(sizeof(Variable));
	
	int size = snprintf(NULL, 0, "printVar%d", printCounter + 1) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "printVar%d", printCounter + 1);
	const char *name = tempName;
	
	v->name = strdup(name);
	storage.stringValue = strdup(value);
	v->type = stringValue;
	
	v->valuestorage = storage;
    HASH_ADD_KEYPTR(hh, printTable, v->name, strlen(v->name), v);
    printCounter++;
    
    return name;
}

void initializeArray(const char *name, ArrayType arraytype, int arraySize) {
    ArrayVariable *va = (ArrayVariable *)malloc(sizeof(ArrayVariable));
    Variable *v = (Variable *)malloc(sizeof(Variable));

    v->name = strdup(name);
    v->type = tableValue;

    va->name = strdup(name);

    ArrayValue storage;

    switch(arraytype) {
        case integerArray:
            storage.integerArray = (int *)malloc(arraySize * sizeof(int));
            va->arraySize = arraySize;
            va->arrayType = integerArray;
            va->arrayValue = storage;
            break;
        case doubleArray:
            storage.doubleArray = (double *)malloc(arraySize * sizeof(double));
            va->arraySize = arraySize;
            va->arrayType = doubleArray;
            va->arrayValue = storage;
            break;
        case stringArray:
            storage.stringArray = (char **)malloc(arraySize * sizeof(char *));
            va->arraySize = arraySize;
            va->arrayType = stringArray;
            va->arrayValue = storage;
            break;
    }

    HASH_ADD_KEYPTR(hh, variableTable, v->name, strlen(v->name), v);
    HASH_ADD_KEYPTR(hh, arrayTable, va->name, strlen(va->name), va);
}

int initializeArrayWithValues(ArrayStack *stack, const char *name, VarType arraytype, int arraySize){
	ArrayStackNode *current = stack->top;
    ArrayStack tempStack;
    initArrayStack(&tempStack);
    
    int meanWhileSizeCounter = 0;
    
    while (current != NULL) {
        pushArrayValue(&tempStack, current->variable);
        if(current->variable.type != arraytype)
        {
        	return 2;
        }
        current = current->next;
        
        meanWhileSizeCounter++;
    }
    
    if(meanWhileSizeCounter > arraySize) { return 0; }
    
    current = tempStack.top;
    
    ArrayVariable *va = (ArrayVariable *)malloc(sizeof(ArrayVariable));
    Variable *v = (Variable *)malloc(sizeof(Variable));
    
    v->name = strdup(name);
    v->type = tableValue;
    
    va->name = strdup(name);
	va->arraySize = arraySize;
	
    ArrayValue storage;
    
    switch(arraytype){
    	case integerValue:
    	int *array = (int *)malloc(arraySize * sizeof(int));
    	int counter = 0;
    		while (current != NULL) {
    			array[counter] = current->variable.value.integerValue;
    			counter++;
				current = current->next;
			}
			va->arrayType = integerArray;
			storage.integerArray = array;
            va->arrayValue = storage;
    	break;
    	case doubleValue:
    	double *arrayd = (double *)malloc(arraySize * sizeof(double));
    	int counterd = 0;
    	while (current != NULL) {
    			arrayd[counterd] = current->variable.value.doubleValue;
    			counterd++;
				current = current->next;
			}
			va->arrayType = doubleArray;
			storage.doubleArray = arrayd;
            va->arrayValue = storage;
    	break;
    	case stringValue:
    	char **arrayst = (char **)malloc(arraySize * sizeof(char*));
    	int counterst = 0;
    	while (current != NULL) {
    			arrayst[counterst] = current->variable.value.stringValue;
    			counterst++;
				current = current->next;
			}
			va->arrayType = stringArray;
			storage.stringArray = arrayst;
            va->arrayValue = storage;
    	break;
    	default: return 0; break;
    }
    
    HASH_ADD_KEYPTR(hh, variableTable, v->name, strlen(v->name), v);
    HASH_ADD_KEYPTR(hh, arrayTable, va->name, strlen(va->name), va);
    
    while (tempStack.top != NULL) {
        ArrayStackNode *tempNode = tempStack.top;
        tempStack.top = tempStack.top->next;
        free(tempNode);
    }
    
    while (stack->top != NULL) {
        ArrayStackNode *arrayNode = stack->top;
        stack->top = stack->top->next;
        free(arrayNode);
    }
    
    return 1;
}

const char* initializeTemp(VarType type, Value value){
	Value storage;
	Variable *v = (Variable *)malloc(sizeof(Variable));
	
	int size = snprintf(NULL, 0, "temporary%d", tempCounter + 1) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "temporary%d", tempCounter + 1);
	const char *name = tempName;

	
	v->name = strdup(name);
	
	if(type == doubleValue){
	    storage.doubleValue = value.doubleValue;
	    v->type = doubleValue;
	}
	
	if(type == stringValue){
		storage.stringValue = strdup(value.stringValue);
	    v->type = stringValue;
	}
	
	if(type == integerValue){
		storage.integerValue = value.integerValue;
	    v->type = integerValue;
	}
	
	v->valuestorage = storage;
    HASH_ADD_KEYPTR(hh, tempTable, v->name, strlen(v->name), v);
    tempCounter++;
    
    return name;
}

void initializePrivatesVar(const char *name, VarType type, Value value){
	PrivateVariable *v = (PrivateVariable *)malloc(sizeof(PrivateVariable));
	v->name = strdup(name);
	v->type = type;
	v->valuestorage = value;
	v->id = privatesCounter;
	
	int size = snprintf(NULL, 0, "%s%d",name ,privatesCounter) + 1;
	char *tempName = malloc(size);
	snprintf(tempName, size, "%s%d",name ,privatesCounter);
	const char *fixedName = tempName;
	v->acessName = strdup(fixedName);	
	
	HASH_ADD_KEYPTR(hh, privateVarsTable, v->acessName, strlen(v->acessName), v);
}

void initializeInteger(const char *name, int value) {
    Value storage;
    storage.integerValue = value;

    Variable *v = (Variable *)malloc(sizeof(Variable));
    v->name = strdup(name);
    v->type = integerValue;
    v->valuestorage = storage;
    HASH_ADD_KEYPTR(hh, variableTable, v->name, strlen(v->name), v);

}

void initializeString(const char *name, const char *value){
    Value storage;
    storage.stringValue = strdup(value);

    Variable *v = (Variable *)malloc(sizeof(Variable));
    v->name = strdup(name);
    v->type = stringValue;
    v->valuestorage = storage;
    HASH_ADD_KEYPTR(hh, variableTable, v->name, strlen(v->name),v);
    
}

void initializeDouble(const char *name, double value){
    Value storage;
    storage.doubleValue = value;

    Variable *v = (Variable *)malloc(sizeof(Variable));
    v->name = strdup(name);
    v->type = doubleValue;
    v->valuestorage = storage;
    HASH_ADD_KEYPTR(hh, variableTable, v->name, strlen(v->name),v);
    
}

//END OF MAIN INITS//

// ---- MATH ----//
void clearMathStack(MathStack *stack) {
    MathStackNode *current = stack->top;
    MathStackNode *nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }

    stack->top = NULL;
}



Stack *computeMathStack(MathStack *stack){
    MathStackNode *current = stack->top;
    MathStack *tempStack;
    Stack *asmstack = (Stack *)malloc(sizeof(Stack));

    initStack(asmstack);

    int containsHigher = checkStatementsForMathComputing(stack);
    if(containsHigher == 0){
        ExpressionType lastExp;
        while (current != NULL) {
            pushSimpleMath(asmstack, current->data, lastExp);
            lastExp = current->data.type;
            current = current->next;
        }
    }else{
        tempStack = performHigherMath(stack);
        Stack *chuj = computeMathStack(tempStack);
    }      
    
    return asmstack;
}

MathStack *performHigherMath(MathStack *stack) {
    MathStackNode *current = stack->top;
    MathStack *computedMathStack = malloc(sizeof(MathStack));
    Stack *stackOperations = malloc(sizeof(Stack));
    MathStack *revStack = malloc(sizeof(MathStack));
    
    initMathStack(revStack);
    initStack(stackOperations);
    initMathStack(computedMathStack); 

    while (current != NULL) {    
        if(current->data.type == addExp || current->data.type == subExp){
            pushMathExpression(computedMathStack, current->data);            
        } else {
            MathStackInside first = current->data;
            current = current->next;

            MathStackInside *newNode = computeHigherMathStatement(stackOperations, first, current->data);         
            pushMathExpressionP(computedMathStack, newNode);           
        }
        current = current->next;
    }   
    
    //rev
    MathStackNode *currentNew = computedMathStack->top;
    
    while (currentNew != NULL) {
        pushMathExpression(revStack, currentNew->data);
        fprintf(stderr, "\n %s \n", currentNew->data.varName);        
        currentNew = currentNew->next;
    }
    
    return revStack;
}

MathStackInside *computeHigherMathStatement(Stack *stackOperations,MathStackInside first, MathStackInside second){
    AssemblyInstruction instruction;
    MathStackInside *newInside = malloc(sizeof(MathStackInside));
    
    int savingQue;
    int prevQue;
    
    prevQue = currentMathQue;
    
    if(currentMathQue == 1){
        savingQue = 0;
        currentMathQue = 0;        
    }else if(currentMathQue == 0){
        savingQue = 1; 
        currentMathQue = 1;
    }
    
    if(first.isAlrdComputed == 1){
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rsi, %d \n", prevQue);
        push(stackOperations, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    call deque \n");
        push(stackOperations, instruction);
    }else{
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rax, [%s]\n", first.varName);
        push(stackOperations, instruction);
    }
    
    if(second.isAlrdComputed == 1){
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rbx, rax \n");
        push(stackOperations, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rsi, %d \n", prevQue);
        push(stackOperations, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    call deque \n");
        push(stackOperations, instruction);        
    }else{
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rbx, rax \n");
        push(stackOperations, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rax, [%s]\n", second.varName);
        push(stackOperations, instruction); 
    }
    
    const char *expType;
    
    switch(first.type){
        case multExp:
            expType = "mul";
        break;
        case divExp:
            expType = "div";
        break;
    }
    
    snprintf(instruction.instruction, sizeof(instruction.instruction), "    %s rax, rbx \n", expType);
    push(stackOperations, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rdi, rax \n");
    push(stackOperations, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rsi, %d \n", savingQue);
        push(stackOperations, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "    call enqueue \n");
    push(stackOperations, instruction);
   
	Stack *currentFLow = getCurrentFlowStack(&flowTable);

	StackNode *current = stackOperations->top; 
	Stack tempStack;
	initStack(&tempStack);

	while (current != NULL) {
		push(&tempStack, current->instruction);
		current = current->next;
	}

	pushStackToFlow(&tempStack, currentFLow);

    newInside->isAlrdComputed = 1;
    newInside->varName = "none";
    newInside->type = second.type;


    return newInside;
}

void pushStackToFlow(Stack *stack, Stack *currStack) {
    AssemblyInstruction instruction;
    StackNode *current = stack->top;

    while (current != NULL) {
        const char *instr = current->instruction.instruction;
        fprintf(stderr, "%s", instr);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    %s", instr);
        push(currStack, instruction);
        current = current->next;
    }
}

void pushSimpleMath(Stack *stack, MathStackInside model, ExpressionType lastExp){
    AssemblyInstruction instruction;
    ExpressionType currentExp;
    const char *expression;
    int prevQue = currentMathQue;    
    
    if(model.type == none){
        currentExp = lastExp;
    }else{
        currentExp = model.type;
    }
    
    switch(currentExp){
        case addExp:
            expression = "add";
        break;
        case subExp:
            expression = "sub";
        break;
    }
    
    if(model.isAlrdComputed == 1){
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rsi, %d \n", prevQue);
        push(stack, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    call deque \n");
        push(stack, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    %s r14, rax\n", expression);
        push(stack, instruction);
    }else{
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    mov rax, [%s] \n", model.varName);
        push(stack, instruction);
        snprintf(instruction.instruction, sizeof(instruction.instruction), "    %s r14, rax\n", expression);
        push(stack, instruction);
    }
}

int checkStatementsForMathComputing(MathStack *stack){
    MathStackNode *current = stack->top;
    int higherStatements = 0;
    
    while (current != NULL) {
        if(current->data.type == multExp || current->data.type == divExp)
        {
            higherStatements = 1;
            break;
        }
        current = current->next;
    }
    return higherStatements;
}


void addInitialsToMathStack(MathStack *stack, const char *name, ExpressionType type){
    MathStackInside *msi = (MathStackInside *)malloc(sizeof(MathStackInside));
    
    msi->type = type;
    msi->varName = name;
    msi->isAlrdComputed = 0;
    
    pushMathExpression(stack, *msi);
}

// END OF MATH //

// ---- FOR ---- //
int isMainVname(const char *name){
	ForStatement *fors;
	HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        //fprintf(stderr, "hitted not null %s ------ %s \n", fors->initialVarName, name);
        if(strcmp(fors->initialVarName, name) == 0) {
            return 1;
        }
    }
    return 0;

}

void initForIncType(ForIncType type){
    ForStatement *fors;
    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        fors->type = type;
    }
}

void initCondition(ForCondition condition){
    ForStatement *fors;
    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        fors->condition = condition;
    }
}

void initNewForLoop(const char *initialVarName, int value){
    ForStatement *fs = (ForStatement *)malloc(sizeof(ForStatement));
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    ForCondition *fc = (ForCondition *)malloc(sizeof(ForCondition));
    ForIncType *fi = (ForIncType *)malloc(sizeof(ForIncType));
    
    initStack(stack);
    
    fs->id = privatesCounter;
    fs->forLoopName = generateForLoopName();
    fs->initialVarName = initialVarName;
    fs->condition = *fc;
    fs->stack = stack;
    fs->type = *fi;
    
    HASH_ADD_INT(forStatementTable, id, fs); 
}

// END OF FOR //
// ---- IF ---- //
void createIfStatement(const char *loopName){
    HighStatementIf *hs = (HighStatementIf *)malloc(sizeof(HighStatementIf));
    hs->id = privatesCounter;
    hs->name = strdup(loopName);
    hs->returnFlow = currentFlow;
    hs->floor = floorNum;
    
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    Stack *contstack = (Stack *)malloc(sizeof(Stack));
    initStack(stack);
    initStack(contstack);
    
    AssemblyInstruction instruction;
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "%s:", loopName);
    push(stack, instruction);
    
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(contstack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "cont%s:", loopName);
    push(contstack, instruction);
    
    hs->stack = stack;
    hs->contstack = contstack;
    HASH_ADD_KEYPTR(hh, highStatementIfTable, hs->name, strlen(hs->name),hs);
}
// END OF IF //

// ---- STACK ---- //

void initStack(Stack *stack) {
    stack->top = NULL;
}

void initMathStack(MathStack *stack){
    stack->top = NULL;
}

void initArrayStack(ArrayStack *stack){
	stack->top = NULL;
}

void pushMathExpression(MathStack *stack, MathStackInside data){
    MathStackNode *newNode = (MathStackNode *)malloc(sizeof(MathStackNode));
    newNode->data = data;
    newNode->next = stack -> top;
    stack->top = newNode;
}

void pushMathExpressionP(MathStack *stack, MathStackInside *data){
    MathStackNode *newNode = (MathStackNode *)malloc(sizeof(MathStackNode));
    newNode->data = *data;
    newNode->next = stack -> top;
    stack->top = newNode;
}

void pushArrayValue(ArrayStack *stack, ArrayVariableSt arrayVar){
	ArrayStackNode *newNode = (ArrayStackNode *)malloc(sizeof(ArrayStackNode));
	newNode->variable = arrayVar;
	newNode->next = stack -> top;
	stack->top = newNode;
}

void push(Stack *stack, AssemblyInstruction instruction) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->instruction = instruction;
    newNode->next = stack->top;
    stack->top = newNode;
}

//---- END ---- //
