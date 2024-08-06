#ifndef ASSEMBLY_GENERATOR_H
#define ASSEMBLY_GENERATOR_H

#include "semantic_analizer.h"
#include "uthash.h"

//general
void createSection(FILE *file, const char* sectionName);
void createStart(FILE *file);

void end(FILE *file);
void initializePrints(FILE *file);

void printEnd(Stack *stack);
void dumpFlow(FILE *file, Flow *flowTable);

//main stack adders
void assignMathToVariable(Stack *stack, const char *varName);
void assignWithTemp(Stack *stack, const char *name, const char *tempName, VarType type);
void assignVariable(Stack *stack, const char *name, VarType type, Value value);
void assignTwoVariables(Stack *stack, const char *name, const char *assigningName, VarType type);
void assignArrayValueFromVariable(Stack *stack, const char *arrayName, const char *varName, VarType eqType, int place);
void assignArrayValueToVariable(Stack *stack, const char *varName, const char *arrayName, VarType eqType ,int place);
void printVariableValue(Stack *stack, VarType type, const char *name);
void printArrayValue(Stack *stack, ArrayType type, const char *name, int place);

//if
void startIfInMain(Stack *stack, ConditionType contype, const char *first, VarType type, const char *second, const char *loopName);
void startIfHigher(HighStatementIf *highTable, ConditionType contype, const char *first, VarType type ,const char *second, const char *loopName);
void assignVariableInIfPrivate(HighStatementIf *highTable, VarType type, Value value, const char *name);
void assignVariableInIfPublic(HighStatementIf *highTable, VarType type, Value value, const char *name);
void assignTwoVariablesInIf(HighStatementIf *highTable, VarType type, const char *first, const char *second, int stState, int ndState);
void printVariableInIf(HighStatementIf *highTable, VarType type, const char *name, int pub);

//for
void startForLoop(Stack *stack, ForStatement *forStatementTable);
void redeclareInitialValues(PrivateVariable *privateVarsTable, ForStatement *forStatementTable);
void forAssigning(const char *name, VarType type, Value value, ForStatement *forStatementTable);
void forAssignTwoVariables(const char *name, const char *nameas, VarType type, ForStatement *forStatementTable);
void assignArrayFromVarFor(const char *arrayName, const char *varName, VarType eqType, int place, ForStatement *forStatementTable);
void printInFor(const char *name, VarType type, ForStatement *forStatementTable);

//math
void pushMathExpressionToCurrentFlowStack(Stack *mstack, Stack *currStack);

//DUMPS
void dumpStack(FILE *file, Stack *stack);
void dumpVariables(FILE *file, Variable *variableTable);
void dumpPrivateVars(FILE *file, PrivateVariable *privateVars);
void dumpArrays(FILE *file, ArrayVariable *arrayTable);
void dumpFifoQue(FILE *file);

void dumpIfs(FILE *file, HighStatementIf *highTable);
void dumpFors(FILE *file, ForStatement *forStatementTable);
#endif

