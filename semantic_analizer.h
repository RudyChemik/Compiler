#ifndef SEMANTIC_ANALIZER_H
#define SEMANTIC_ANALIZER_H
#include "uthash.h"

typedef enum {
    addExp,
    subExp,
    multExp,
    divExp,
    none,
} ExpressionType;

typedef enum {
    equal,
    equalOrBigger,
    equalOrSmaller,
    bigger,
    smaller,
    diffrent,
} ConditionType;

typedef enum {
    fbigger,
    fsmaller,
    fequalOrBigger,
    fequalOrSmaller,
} ForConditionType;

typedef enum {
    finc,
    fdec,
} ForIncType;

typedef enum {
    integerValue,
    longValue,
    doubleValue,
    stringValue,
    tableValue,
    nonExisting
} VarType;

typedef enum{
    integerArray,
    doubleArray,
    stringArray,
    nonExistingg
} ArrayType;

typedef union {
    int integerValue;
    double doubleValue;
    char* stringValue;
} Value;

typedef union {
    int* integerArray;
    double* doubleArray;
    char** stringArray;
} ArrayValue;

typedef struct {
    char *name;
    VarType type;
    Value valuestorage;
    UT_hash_handle hh; 
} Variable;

typedef struct{
    char *name;
    ArrayType arrayType;
    ArrayValue arrayValue;
    int arraySize;
    UT_hash_handle hh;
} ArrayVariable;

typedef struct {
    char *name;
    char *acessName;
    int id;
    VarType type;
    Value valuestorage;
    UT_hash_handle hh; 
} PrivateVariable;


// Stack
typedef struct {
    char instruction[256];
} AssemblyInstruction;

typedef struct StackNode {
    AssemblyInstruction instruction;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} Stack;

//MATH stack
typedef struct {
    ExpressionType type;
    int isAlrdComputed;
    const char *varName;
} MathStackInside;

typedef struct MathStackNode{
    MathStackInside data;
    struct MathStackNode *next;
} MathStackNode;

typedef struct{
    MathStackNode *top;
} MathStack;



//array stack
typedef struct {
    VarType type;
    Value value;
} ArrayVariableSt;

typedef struct ArrayStackNode{
    ArrayVariableSt variable;
    struct ArrayStackNode *next;
} ArrayStackNode;

typedef struct{
    ArrayStackNode *top;
} ArrayStack;

//if.

typedef struct{
    int id;
    int nested;
    int floor;
    int returnFlow;
    const char *name;
    Stack *stack;
    Stack *contstack;
    UT_hash_handle hh;
} HighStatementIf;

typedef struct{
    const char *name;
    ForConditionType conditionType;
    int value;
} ForCondition;

typedef struct{
    int id;
    const char *initialVarName;
    int initialVarValue;
    ForCondition condition;
    const char *forLoopName;
    ForIncType type;
    Stack *stack;
    UT_hash_handle hh; 
} ForStatement;

//ret

typedef struct{
    ArrayType type;
    int code; //0 not found, //1 sucess //2 sucess but exceeded.
} ArrayTypeMO;

typedef struct{
    VarType type;
    const char *name;
} PrivateVarMO;

//FLOWS
typedef struct{
    int id;
    const char *flowName;
    Stack *stack;
    UT_hash_handle hh;
} Flow;

//helpers
void normalizeString(char *str);
const char *generateFlowName();
const char *generateLoopName();
const char *generateNormalizedPrivateName(const char *name);
const char *generateForLoopName();

int correctVariableName(const char *name);
VarType checkType(const char *name);
ArrayTypeMO checkArrayType(const char *name, int place);
PrivateVarMO privateCheck(const char *name);

//flows
Stack *getStackBasedOnFlow(int i, Flow **flowTable);
Stack *getStartingStack(Flow **flowTable);
Stack *getHigherFlowStack(Flow **flowTable);
Stack *getCurrentFlowStack(Flow **flowTable);
void initNewFlow(Flow **flowTable);
void initFlow(Flow **flowTable);
void iterateFlows(Flow *flowTable);


void initializeInteger(const char *name, int value);
void initializeString(const char *name, const char *value);
void initializeDouble(const char *name, double value);
void initializeArray(const char *name, ArrayType arraytype, int arraySize);
int initializeArrayWithValues(ArrayStack *stack, const char *name, VarType arraytype, int arraySize);//1 - suc //2 - typer //0-len
const char* initializeTemp(VarType type, Value value);
const char *initializePrint(const char *value);
void initializePrivatesVar(const char *name, VarType type, Value value);

//if
void createIfStatement(const char *loopName);

//for
void initNewForLoop(const char *initialVarName, int value);
void initCondition(ForCondition condition);
int isMainVname(const char *name);
void initForIncType(ForIncType type);

//math
void addInitialsToMathStack(MathStack *stack, const char *name, ExpressionType type);
Stack *computeMathStack(MathStack *stack);
MathStack *performHigherMath(MathStack *stack);
void pushSimpleMath(Stack *stack, MathStackInside model, ExpressionType lastExp);
int checkStatementsForMathComputing(MathStack *stack);
MathStackInside *computeHigherMathStatement(Stack *stackOperations, MathStackInside first, MathStackInside second);
void clearMathStack(MathStack *stack);

//stack
void initStack(Stack *stack);
void initArrayStack(ArrayStack *stack);
void initMathStack(MathStack *stack);

void pushStackToFlow(Stack *stack, Stack *currStack);
void push(Stack *stack, AssemblyInstruction instruction);
void pushArrayValue(ArrayStack *stack, ArrayVariableSt arrayVar);
void pushMathExpression(MathStack *stack, MathStackInside data);
void pushMathExpressionP(MathStack *stack, MathStackInside *data);

#endif

