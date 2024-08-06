#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic_analizer.h"
#include "assembly_generator.h"
#include "uthash.h"

extern int privatesCounter;

void createSection(FILE *file, const char* sectionName) {
    fprintf(file, "section .%s\n", sectionName);
}

void createStart(FILE *file){
	fprintf(file, "\n");
	fprintf(file, "section .bss \n");
	fprintf(file, "	digitSpace resb 100 \n");
	fprintf(file, "	digitSpacePos resb 8 \n");
    fprintf(file, "\n");
    fprintf(file, "section .%s\n", "text");
    fprintf(file, "   global _start\n");
    fprintf(file, "\n");
    fprintf(file, "_start: \n");
}

void end(FILE *file){
    fprintf(file, "\n");
    fprintf(file, "	mov eax, 60 \n");
    fprintf(file, "	xor edi, edi \n");
    fprintf(file, "	syscall");
    fprintf(file, "\n");
}

void printEnd(Stack *stack){
	AssemblyInstruction instruction;

	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "mov eax, 60 \n");
    push(stack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "xor edi, edi \n");
    push(stack, instruction);
    snprintf(instruction.instruction, sizeof(instruction.instruction), "syscall \n");
    push(stack, instruction);
}

void initializePrints(FILE *file){
	fprintf(file, "\n");
    fprintf(file, "replace_string: \n");
    fprintf(file, "	push rdi \n");
    fprintf(file, "	push rsi \n");
    fprintf(file, "	push rcx \n");
    fprintf(file, "	mov rcx, 0 \n");
    fprintf(file, "\n");
    fprintf(file, "count_new_string: \n");
    fprintf(file, "	mov al, [rsi + rcx] \n");
    fprintf(file, "	cmp al, 0 \n");
    fprintf(file, "	je done_counting \n");
    fprintf(file, "	inc rcx \n");
    fprintf(file, "	jmp count_new_string \n");
    fprintf(file, "done_counting: \n");
    fprintf(file, "	inc rcx \n");
    fprintf(file, "\n");
    fprintf(file, "replace_loop:\n");
    fprintf(file, "	cmp rcx, 0 \n");
    fprintf(file, "	je replace_done \n");
    fprintf(file, "	mov al, [rsi] \n");
    fprintf(file, "	mov [rdi], al \n");
    fprintf(file, "	inc rsi \n");
    fprintf(file, "	inc rdi \n");
    fprintf(file, "	dec rcx \n");
    fprintf(file, "	jmp replace_loop \n");
    fprintf(file, "\n");
    fprintf(file, "replace_done: \n");
    fprintf(file, "	pop rcx \n");
    fprintf(file, "	pop rsi \n");
    fprintf(file, "	pop rdi \n");
    fprintf(file, "	ret\n");
    
    fprintf(file, "\n");   
    fprintf(file, "_printInt: \n");
    fprintf(file, "	mov r15, 0 \n");
    fprintf(file, "	mov rcx, digitSpace \n");
    fprintf(file, "	mov rbx, 10 \n");
    fprintf(file, "	mov [digitSpace], rbx \n");
    fprintf(file, "	inc rcx \n");
    fprintf(file, "	mov [digitSpacePos], rcx \n");
    fprintf(file, "	cmp rax, 0 \n");
    fprintf(file, "	jge .positive \n");
    fprintf(file, "	neg rax \n");
    fprintf(file, "	mov r15, 1 \n");
    fprintf(file, "\n");
    fprintf(file, ".positive:\n");
    fprintf(file, "_printIntLoop:\n");
    fprintf(file, "	xor rdx, rdx\n");
    fprintf(file, "	div rbx\n");
    fprintf(file, "	push rax\n");
    fprintf(file, "	add dl, 48\n");
    fprintf(file, "	mov rcx, [digitSpacePos]\n");
    fprintf(file, "	mov [rcx], dl\n");
    fprintf(file, "	inc rcx\n");
    fprintf(file, "	mov [digitSpacePos], rcx\n");
    fprintf(file, "	pop rax\n");
    fprintf(file, "	cmp rax, 0\n");
    fprintf(file, "	jne _printIntLoop\n");
    fprintf(file, "	cmp r15, 1\n");
    fprintf(file, "	jne .falseNeg\n");
    fprintf(file, "	mov rax, '-'\n");
    fprintf(file, "	call _printChar\n");
    fprintf(file, ".falseNeg:\n");
    fprintf(file, "_printIntLoop2:\n");
    fprintf(file, "	mov rcx, [digitSpacePos]\n");
    fprintf(file, "	dec rcx\n");
    fprintf(file, "	mov [digitSpacePos], rcx\n");
    fprintf(file, "	cmp rcx, digitSpace\n");
    fprintf(file, "	jl _printIntDone\n");
    fprintf(file, "	mov rax, 1\n");
    fprintf(file, "	mov rdi, 1\n");
    fprintf(file, "	mov rsi, rcx\n");
    fprintf(file, "	mov rdx, 1\n");
    fprintf(file, "	syscall\n");
    fprintf(file, "	jmp _printIntLoop2\n");
    fprintf(file, "\n");
    fprintf(file, "_printIntDone:\n");
    fprintf(file, "	ret\n");
    
    fprintf(file, "\n");
    fprintf(file, "_printstring: \n");
    fprintf(file, "	push rax \n");
    fprintf(file, "	mov rbx, 0\n");
    fprintf(file, "\n");
    fprintf(file, "_printstringloop:\n");
    fprintf(file, "	mov cl, [rax + rbx]\n");
    fprintf(file, "	cmp cl, 0\n");
    fprintf(file, "	je .done\n");
    fprintf(file, "	inc rbx\n");
    fprintf(file, "	jmp _printstringloop\n");
    fprintf(file, "\n");
    fprintf(file, ".done:\n");
    fprintf(file, "	mov rax, 1\n");
    fprintf(file, "	mov rdi, 1\n");
    fprintf(file, "	pop rsi\n");
    fprintf(file, "	mov rdx, rbx\n");
    fprintf(file, "	syscall\n");
    fprintf(file, "	ret\n");
    
    fprintf(file, "\n");
    fprintf(file, "_printDouble:\n");
    fprintf(file, "	movq rax, xmm0\n");
    fprintf(file, "	shr rax, 63\n");
    fprintf(file, "	test rax, rax\n");
    fprintf(file, "	jz .positive\n");
    fprintf(file, "	mov rax, '-'\n");
    fprintf(file, "call _printChar\n");
	fprintf(file, "mov rax, 1\n");
	fprintf(file, "shl rax, 63\n");
	fprintf(file, "movq xmm1, rax\n");
	fprintf(file, "xorps xmm0, xmm1\n");
	fprintf(file, "\n");
	fprintf(file, ".positive:\n");
	fprintf(file, "mov rax, 10\n");
	fprintf(file, "cvtsi2sd xmm1, rax\n");
	fprintf(file, "movq rax, xmm0\n");
	fprintf(file, "cvttsd2si rax, xmm0\n");
	fprintf(file, "cvtsi2sd xmm2, rax\n");
	fprintf(file, "subsd xmm0, xmm2\n");
	fprintf(file, "mov rax, rax\n");
	fprintf(file, "call _printInt\n");
	fprintf(file, "mov rax, '.'\n");
	fprintf(file, "call _printChar\n");
	fprintf(file, "mov rbx, 1000000\n");
	fprintf(file, "cvtsi2sd xmm3, rbx\n");
	fprintf(file, "mulsd xmm0, xmm3\n");
	fprintf(file, "movq rax, xmm0\n");
	fprintf(file, "cvttsd2si rax, xmm0\n");
	fprintf(file, "call _printInt\n");
    fprintf(file, "	ret \n");
    
    fprintf(file, "\n");
    fprintf(file, "_printChar:\n");
    fprintf(file, "	push rax\n");
    fprintf(file, "	mov rax, 1\n");
    fprintf(file, "	mov rdi, 1\n");
    fprintf(file, "	mov rsi, rsp\n");
    fprintf(file, "	mov rdx, 1 \n");
    fprintf(file, "	syscall \n");
    fprintf(file, "	pop rax \n");
    fprintf(file, "	ret \n");
    
    fprintf(file, "\n");
    fprintf(file, "_getStringAdr: \n");
    fprintf(file, "push rdi \n");
    fprintf(file, "push rsi \n");
    fprintf(file, "mov rcx, rsi \n");
    fprintf(file, "mov rbx, rdi \n");
    fprintf(file, "\n");
    fprintf(file, ".next_string:\n");
    fprintf(file, "cmp rcx, 0\n");
    fprintf(file, "je .found_string\n");
    fprintf(file, "mov al, [rbx]\n");
    fprintf(file, "cmp al, 0\n");
    fprintf(file, "je .skip_null\n");
    fprintf(file, "inc rbx\n");
    fprintf(file, "jmp .next_string\n");
    fprintf(file, ".skip_null:\n");
    fprintf(file, "dec rcx\n");
    fprintf(file, "inc rbx \n");
    fprintf(file, "jmp .next_string\n");
    fprintf(file, ".found_string:\n");
    fprintf(file, "mov rbx, rdi \n");
    fprintf(file, "pop rsi\n");
    fprintf(file, "pop rdi\n");
    fprintf(file, "ret\n");
    
    fprintf(file, "\n");
    fprintf(file, "_StringCompare:\n");
    fprintf(file, "push rdi\n");
    fprintf(file, "push rsi\n");
    fprintf(file, "push rcx\n");
    fprintf(file, "cmp rdi, 0\n");
    fprintf(file, "je .not_equal\n");
    fprintf(file, "cmp rsi, 0\n");
    fprintf(file, "je .not_equal\n");
    fprintf(file, "\n");
    fprintf(file, ".compare_loop:\n");
    fprintf(file, "mov al, [rdi]\n");
    fprintf(file, "mov bl, [rsi]\n");
    fprintf(file, "cmp al, bl\n");
    fprintf(file, "jne .not_equal\n");
    fprintf(file, "test al, al\n");
    fprintf(file, "je .equal\n");
    fprintf(file, "inc rdi\n");
    fprintf(file, "inc rsi\n");
    fprintf(file, "jmp .compare_loop\n");
    fprintf(file, "\n");
    fprintf(file, ".equal:\n");
    fprintf(file, "mov rax, 1\n");
    fprintf(file, "jmp .done\n");
    fprintf(file, "\n");
    fprintf(file, ".not_equal:\n");
    fprintf(file, "mov rax, 0\n");
    fprintf(file, "\n");
    fprintf(file, ".done:\n");
    fprintf(file, "pop rcx\n");
    fprintf(file, "pop rsi\n");
    fprintf(file, "pop rdi \n");
    fprintf(file, "ret\n");
    
    fprintf(file, "\n");
    fprintf(file, "enqueue: \n");
    fprintf(file, "cmp rsi, 0\n");
    fprintf(file, "je .queue1 \n");
    fprintf(file, "cmp rsi, 1\n");
    fprintf(file, "je .queue2 \n");
    fprintf(file, "ret\n");
    fprintf(file, "\n");
    fprintf(file, ".queue1:\n");
    fprintf(file, "mov rbx, [tail1] \n");
    fprintf(file, "mov rcx, [max_size1]\n");
    fprintf(file, "mov rdx, [head1] \n");
    fprintf(file, "cmp rbx, rcx\n");
    fprintf(file, "je .full \n");
    fprintf(file, "mov [queue1 + rbx], rdi\n");
    fprintf(file, "add rbx, 1 \n");
    fprintf(file, "mov [tail1], rbx\n");
    fprintf(file, "jmp .end \n");
    fprintf(file, "\n");
    fprintf(file, ".queue2: \n");
    fprintf(file, "mov rbx, [tail2] \n");
    fprintf(file, "mov rcx, [max_size2]\n");
    fprintf(file, "mov rdx, [head2]\n");
    fprintf(file, "cmp rbx, rcx\n");
    fprintf(file, "je .full\n");
    fprintf(file, "mov [queue2 + rbx], rdi\n");
    fprintf(file, "add rbx, 1\n");
    fprintf(file, "mov [tail2], rbx\n");
    fprintf(file, "jmp .end\n");
    fprintf(file, "\n");
    fprintf(file, ".full:\n");
    fprintf(file, "mov rsi, msg_full\n");
    fprintf(file, "call _printstring\n");
    fprintf(file, "jmp .end\n");
    fprintf(file, "\n");
    fprintf(file, ".end:\n");
    fprintf(file, "  ret\n");
    fprintf(file, "dequeue:\n");
    fprintf(file, " cmp rsi, 0\n");
    fprintf(file, "je .dequeue_queue1\n");
    fprintf(file, "cmp rsi, 1\n");
    fprintf(file, "je .dequeue_queue2\n");
    fprintf(file, "ret\n");
    fprintf(file, "\n");
    fprintf(file, ".dequeue_queue1:\n");
    fprintf(file, "mov rbx, [head1]\n");
    fprintf(file, "mov rcx, [tail1]\n");
    fprintf(file, "cmp rbx, rcx\n");
    fprintf(file, "je .empty\n");
    fprintf(file, "mov al, [queue1 + rbx]\n");
    fprintf(file, "add rbx, 1\n");
    fprintf(file, "mov [head1], rbx\n");
    fprintf(file, "ret\n");
    fprintf(file, "\n");
    fprintf(file, ".dequeue_queue2:\n");
    fprintf(file, "mov rbx, [head2]\n");
    fprintf(file, "mov rcx, [tail2]\n");
    fprintf(file, "cmp rbx, rcx\n");
    fprintf(file, "je .empty\n");
    fprintf(file, "mov al, [queue2 + rbx]\n");
    fprintf(file, "add rbx, 1\n");
    fprintf(file, "mov [head2], rbx\n");
    fprintf(file, "ret\n");
    fprintf(file, "\n");
    fprintf(file, ".empty:\n");
    fprintf(file, "mov rsi, msg_empty\n");
    fprintf(file, "call _printstring\n");
    fprintf(file, "mov rax, -1\n");
    fprintf(file, "ret\n");
    fprintf(file, "\n");
    fprintf(file, "\n");
}

void assignMathToVariable(Stack *stack, const char *varName){
    AssemblyInstruction instruction;

    snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    snprintf(instruction.instruction, sizeof(instruction.instruction), "mov [%s], r14\n", varName);
    push(stack, instruction);
}


void assignWithTemp(Stack *stack, const char *name, const char *tempName, VarType type){
	AssemblyInstruction instruction;

    snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    switch(type){
    	case doubleValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s]", tempName);
            push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd [%s], xmm0", name);
            push(stack, instruction);
            break;
        case stringValue:
        	snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s]", name);
            push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rsi, [%s]", tempName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call replace_string");
            push(stack, instruction);
            break;
        case integerValue:
        	snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, [%s]", tempName);
            push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov [%s], rax", name);
            push(stack, instruction);
        	break;
    }
    
}

void assignArrayValueToVariable(Stack *stack, const char *varName, const char *arrayName, VarType eqType ,int place){
	AssemblyInstruction instruction;
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    int placeValue = (place - 1) * 8;
    
    switch(eqType){
    	case integerValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", arrayName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, [rdi + %d] \n", placeValue);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov [%s], rax \n", varName);
            push(stack, instruction);
            break;
    	break;
    	case doubleValue:
			snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", arrayName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [rdi + %d]", placeValue);
            push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd [%s], xmm0", varName);
            push(stack, instruction);
    	break;
    	case stringValue:
			snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, %s \n", arrayName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rsi, %d \n", place);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _getStringAdr \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rsi, rbx \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", varName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call replace_string \n");
            push(stack, instruction);
    	break;
    }   
}

void assignArrayValueFromVariable(Stack *stack, const char *arrayName, const char *varName, VarType eqType, int place){
	AssemblyInstruction instruction;
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    int placeValue = (place - 1) * 8;
    
    switch(eqType){
    	case integerValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", arrayName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, [%s] \n", varName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov [rdi + %d], rax \n", placeValue);
            push(stack, instruction);
            break;
    	break;
    	case doubleValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", arrayName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, qword [%s]", varName);
            push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd qword [rdi + %d], xmm0", placeValue);
            push(stack, instruction);
    	break;
    	case stringValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, %s \n", arrayName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rsi, %d \n", place);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _getStringAdr \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rdi, rbx \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rsi, [%s] \n", varName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call replace_string \n");
            push(stack, instruction);
    	break;
    }   
}

void assignTwoVariables(Stack *stack, const char *name, const char *assigningName, VarType type){
	AssemblyInstruction instruction;	
	
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    switch(type){
    	case integerValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rax, [%s]", assigningName);
    		push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, [rax]");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s]", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov [rdi], rax");
            push(stack, instruction);
    	break;
    	case doubleValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s]", assigningName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd [%s], xmm0", name);
            push(stack, instruction);
    	break;
    	case stringValue:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s]", name);
            push(stack, instruction);
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rsi, [%s]", assigningName);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call replace_string");
            push(stack, instruction);
    	break;
    }
}

void assignVariable(Stack *stack, const char *name, VarType type, Value value) {
    AssemblyInstruction instruction;

    snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);

    snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s]", name);
    push(stack, instruction);

    switch (type) {
        case integerValue:
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, %d", value.integerValue);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov [rdi], rax");
            push(stack, instruction);
            break;
        case doubleValue: //cant do this ac
            snprintf(instruction.instruction, sizeof(instruction.instruction), "movq xmm0, %f", value.doubleValue);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "movq [rdi], xmm0");
            push(stack, instruction);
            break;
        case stringValue: //?>???
            snprintf(instruction.instruction, sizeof(instruction.instruction), "mov rax, %s", value.stringValue);
            push(stack, instruction);
            break;
    }

}

void printArrayValue(Stack *stack, ArrayType type, const char *name, int place){
	AssemblyInstruction instruction;
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    int placeValue = (place - 1) * 8;
    
    switch(type){
    	case integerArray:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, [rdi + %d] \n", placeValue);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _printInt \n");
            push(stack, instruction);
            break;
    	break;
    	case doubleArray:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, [%s] \n", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [rdi + %d] \n", placeValue);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _printDouble \n");
            push(stack, instruction);
    	break;
    	case stringArray:
    		snprintf(instruction.instruction, sizeof(instruction.instruction), "	lea rdi, %s \n", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rsi, 1 \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _getStringAdr \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, rbx \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _printstring \n");
            push(stack, instruction);
    	break;
    }   
}

void printVariableValue(Stack *stack, VarType type, const char *name){
	AssemblyInstruction instruction;
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    switch(type){
    	case integerValue:
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, [%s] \n", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _printInt \n");
            push(stack, instruction);
            break;
        case doubleValue:
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s] \n", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _printDouble \n");
            push(stack, instruction);
            break;
        case stringValue:
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, %s \n", name);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _printstring \n");
            push(stack, instruction);
            break;
    
    }
}

//IFS
void assignTwoVariablesInIf(HighStatementIf *highTable, VarType type, const char *first, const char *second, int stState, int ndState){
	HighStatementIf *s;
	AssemblyInstruction instruction;

	const char *assigningName = first;
	const char *assignName = second;
	
	if(stState == 0){
	     assigningName = generateNormalizedPrivateName(first);
	}
	
	if(ndState == 0){
	    assignName = generateNormalizedPrivateName(second);
	}
	
	for (s = highTable; s != NULL; s = s->hh.next) {
        if (s->id == privatesCounter) {
        	if(s->nested > 0){
        	    assignTwoVariables(s->contstack, assigningName, assignName, type);
        	}else{
        	    assignTwoVariables(s->stack, assigningName, assignName, type);
        	}
        }
	}
}

void assignVariableInIfPublic(HighStatementIf *highTable, VarType type, Value value, const char *name){
	HighStatementIf *s;
	AssemblyInstruction instruction;
	
	Stack propStack;
	for (s = highTable; s != NULL; s = s->hh.next) {
        if (s->id == privatesCounter) {
        	if(s->nested > 0){
        	    if(type == integerValue){
        	    	assignVariable(s->contstack, name, type, value);
        	    }else if(type == doubleValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->contstack, name, tempName, type);
        	    }else if(type == stringValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->contstack, name, tempName, type);
        	    }
        	}else{
        	    if(type == integerValue){
        	    	assignVariable(s->stack, name, type, value);
        	    }else if(type == doubleValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->stack, name, tempName, type);
        	    }else if(type == stringValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->stack, name, tempName, type);
        	    }
        	}
        }
	}
}


void printVariableInIf(HighStatementIf *highTable, VarType type, const char *name, int pub){
    HighStatementIf *s;
    const char *varName;
   
    if(pub == 1){
        varName = name;
    }else{
        varName = generateNormalizedPrivateName(name);
    }
    
    for (s = highTable; s != NULL; s = s->hh.next) {
        if (s->id == privatesCounter) {
            if(s->nested > 0){
                printVariableValue(s->contstack, type, varName);
            }else{
                printVariableValue(s->stack, type, varName);
            }
        }
    }
}

void assignVariableInIfPrivate(HighStatementIf *highTable, VarType type, Value value, const char *name){
	HighStatementIf *s;
	AssemblyInstruction instruction;
	const char *varName = generateNormalizedPrivateName(name);
			
	Stack propStack;
	for (s = highTable; s != NULL; s = s->hh.next) {
        if (s->id == privatesCounter) {
        	if(s->nested > 0){
        	    if(type == integerValue){
        	    	assignVariable(s->contstack, varName, type, value);
        	    }else if(type == doubleValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->contstack, varName, tempName, type);
        	    }else if(type == stringValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->contstack, varName, tempName, type);
        	    }
        	}else{
        	    if(type == integerValue){
        	    	assignVariable(s->stack, varName, type, value);
        	    }else if(type == doubleValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->stack, varName, tempName, type);
        	    }else if(type == stringValue){
        	        const char *tempName = initializeTemp(type, value);
        	        assignWithTemp(s->stack, varName, tempName, type);
        	    }
        	}
        }
	}
}


void startIfHigher(HighStatementIf *highTable, ConditionType contype, const char *first,VarType type ,const char *second, const char *loopName){
	HighStatementIf *s;
	const char *condition;
    for (s = highTable; s != NULL; s = s->hh.next) {
        if (s->id == (privatesCounter - 1)) {
            s->nested = 1;
            AssemblyInstruction instruction;
			snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    		push(s->stack, instruction);
    		
    		switch(contype){
    		    case equal:
		        	condition = "je";
					break;
				case diffrent:
					condition = "jne";
					break;
				case equalOrBigger:
					condition = "jge";
					break;
				case equalOrSmaller:
					condition = "jle";
					break;
				case bigger:
		        	condition = "jg";
					break;
				case smaller:
		        	condition = "jl";
					break;
				default:
				break;
    		}
    		
			switch(type){
		    case integerValue:
		    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, %s \n", first);
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rbx, %s \n", second);
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	cmp rax, rbx \n");
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s %s \n", condition, loopName);
		        push(s->stack, instruction);
		    break;
		    case doubleValue:
		    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s] \n", first);
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s] \n", second);
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	ucomisd xmm0, xmm1 \n");
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s %s \n", condition, loopName);
		        push(s->stack, instruction);
		    break;
		    case stringValue:
		    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rdi, %s \n", first);
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rsi, %s \n", second);
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _StringCompare \n");
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	cmp rax, 1 \n");
		        push(s->stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s %s \n", condition, loopName);
		        push(s->stack, instruction);
		    break;
		    default:
		    break;
			}
        }
    }
}

void startIfInMain(Stack *stack, ConditionType contype, const char *first, VarType type, const char *second, const char *loopName){
	AssemblyInstruction instruction;
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    push(stack, instruction);
    
    const char *condition;
    
    switch(contype){
    		    case equal:
		        	condition = "je";
					break;
				case diffrent:
					condition = "jne";
					break;
				case equalOrBigger:
					condition = "jge";
					break;
				case equalOrSmaller:
					condition = "jle";
					break;
				case bigger:
		        	condition = "jg";
					break;
				case smaller:
		        	condition = "jl";
					break;
				default:
				break;
	}
    
    switch(type){
        case integerValue:
        	snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rax, %s \n", first);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rbx, %s \n", second);
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	cmp rax, rbx \n");
            push(stack, instruction);
            snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s %s \n", condition, loopName);
            push(stack, instruction);
        break;
        case doubleValue:
        		snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s] \n", first);
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	movsd xmm0, [%s] \n", second);
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	ucomisd xmm0, xmm1 \n");
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s %s \n", condition, loopName);
		        push(stack, instruction);
        break;
        case stringValue:
        		snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rdi, %s \n", first);
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rsi, %s \n", second);
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	call _StringCompare \n");
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	cmp rax, 1 \n");
		        push(stack, instruction);
		        snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s %s \n", condition, loopName);
		        push(stack, instruction);
        break;
        default:
        break;
    }
}

//TODO
//FORS
void startForLoop(Stack *stack, ForStatement *forStatementTable){
    AssemblyInstruction instruction;
    ForStatement *fors;
    
    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    	push(stack, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rcx, [%s]", fors->initialVarName);
    	push(stack, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	mov rbx, %d", fors->initialVarValue);
    	push(stack, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	call %s", fors->forLoopName);
    	push(stack, instruction);
    }
}

void redeclareInitialValues(PrivateVariable *privateVarsTable, ForStatement *forStatementTable){
    AssemblyInstruction instruction;
    PrivateVariable *prv, *tmp;
    ForStatement *fors;
    Stack *forStack;
    
    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        forStack = fors->stack;
    }
	
    HASH_ITER(hh, privateVarsTable, prv, tmp) {
	if(prv->id == privatesCounter){
	    switch(prv->type){
	        case integerValue:	        
	            assignVariable(forStack, prv->acessName, prv->type, prv->valuestorage);
	        break;
	        case doubleValue:
	            const char *tempV = initializeTemp(doubleValue, prv->valuestorage);
	            assignWithTemp(forStack, prv->acessName, tempV ,prv->type);
	        break;
	        case stringValue:
	            const char *tempVs = initializeTemp(stringValue, prv->valuestorage);
	            assignWithTemp(forStack, prv->acessName, tempVs ,prv->type);
	        break;
	    }
	}
    }
}

void forAssigning(const char *name, VarType type, Value value, ForStatement *forStatementTable){
    ForStatement *fors;
    Stack *forStack;

    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        forStack = fors->stack;
    }
    
    switch(type){
    	case integerValue:
    	    assignVariable(forStack, name, type, value);
    	break;
    	case doubleValue:
    	    const char *tempVs = initializeTemp(doubleValue, value);
	    assignWithTemp(forStack, name, tempVs ,type);
    	break;
    	case stringValue:
    	    const char *tempV = initializeTemp(stringValue, value);
	    assignWithTemp(forStack, name, tempV ,type);
    	break;
    }

}

void forAssignTwoVariables(const char *name, const char *nameas, VarType type, ForStatement *forStatementTable){
    ForStatement *fors;
    Stack *forStack;

    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        forStack = fors->stack;
    }
    
    assignTwoVariables(forStack, name, nameas, type);
}

void assignArrayFromVarFor(const char *arrayName, const char *varName, VarType eqType, int place, ForStatement *forStatementTable){
    ForStatement *fors;
    Stack *forStack;

    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        forStack = fors->stack;
    }
    assignArrayValueFromVariable(forStack, arrayName, varName, eqType, place);
}

void printInFor(const char *name, VarType type, ForStatement *forStatementTable){
    ForStatement *fors;
    Stack *forStack;

    HASH_FIND_INT(forStatementTable, &privatesCounter, fors);
    if (fors != NULL) {
        forStack = fors->stack;
    }
    
    printVariableValue(forStack, type, name);
}

//TODO



void pushMathExpressionToCurrentFlowStack(Stack *mstack, Stack *currStack){
    AssemblyInstruction instruction;
    StackNode *current = mstack->top;
    int first = 1;
    const char *last;
    const char *instr;

    while (current != NULL) {
        if(first == 1){
            last = current->instruction.instruction;
            first = 0;
        }else{
            instr = current->instruction.instruction;
            snprintf(instruction.instruction, sizeof(instruction.instruction), "    %s", instr);
            push(currStack, instruction);
        }   
        current = current->next;     
    }

    snprintf(instruction.instruction, sizeof(instruction.instruction), "    %s \n", last);
    push(currStack, instruction);
}


void dumpFlow(FILE *file, Flow *flowTable){
	Flow *current_hs, *tmp;
		
	HASH_ITER(hh, flowTable, current_hs, tmp) {
		printEnd(current_hs->stack);
		dumpStack(file, current_hs->stack);
	}
}

void dumpStack(FILE *file, Stack *stack){
    StackNode *current = stack->top;
    Stack tempStack;
    initStack(&tempStack);

    while (current != NULL) {
        push(&tempStack, current->instruction);
        current = current->next;
    }

    //rev
    current = tempStack.top;
    while (current != NULL) {
        fprintf(file, "%s\n", current->instruction.instruction);
        current = current->next;
    }

    //Clean up temp
    while (tempStack.top != NULL) {
        StackNode *tempNode = tempStack.top;
        tempStack.top = tempStack.top->next;
        free(tempNode);
    }
}

void dumpPrivateVars(FILE *file, PrivateVariable *privateVars){
	PrivateVariable *current_variable, *tmp;
	
	fprintf(file, "\n");

    HASH_ITER(hh, privateVars, current_variable, tmp) {
        switch (current_variable->type) {
            case integerValue:
                fprintf(file, "	%s dq %d\n", current_variable->acessName, current_variable->valuestorage.integerValue);
                break;
            case stringValue:
                fprintf(file, "	%s db \"%s\",10,0\n", current_variable->acessName, current_variable->valuestorage.stringValue);
                break;
            case doubleValue:
                fprintf(file, "	%s dq %f\n", current_variable->acessName, current_variable->valuestorage.doubleValue);
                break;
            default:
                break;
        }
    }
}

void dumpVariables(FILE *file, Variable *variableTable) {
    Variable *current_variable, *tmp;

	fprintf(file, "\n");

    HASH_ITER(hh, variableTable, current_variable, tmp) {
        switch (current_variable->type) {
            case integerValue:
                fprintf(file, "	%s dq %d\n", current_variable->name, current_variable->valuestorage.integerValue);
                break;
            case stringValue:
                fprintf(file, "	%s db \"%s\",10,0\n", current_variable->name, current_variable->valuestorage.stringValue);
                break;
            case doubleValue:
                fprintf(file, "	%s dq %f\n", current_variable->name, current_variable->valuestorage.doubleValue);
                break;
            default:
                break;
        }
    }
}

void dumpFifoQue(FILE *file){
    fprintf(file, "queue1 db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");
    fprintf(file, "queue2 db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");
    fprintf(file, "tail1 dq 0 \n");
    fprintf(file, "head1 dq 0 \n");
    fprintf(file, "tail2 dq 0 \n");
    fprintf(file, "head2 dq 0 \n");
    fprintf(file, "max_size1 dq 20 \n");
    fprintf(file, "max_size2 dq 20 \n");
    fprintf(file, "msg_full db \"OVER!\", 0 \n");
    fprintf(file, "msg_empty db \"EMPTY!\", 0 \n");
}

void dumpArrays(FILE *file, ArrayVariable *arrayTable) {
    ArrayVariable *current_variable, *tmp;

    fprintf(file, "\n");

    HASH_ITER(hh, arrayTable, current_variable, tmp) {
    	fprintf(file, "\n");
        fprintf(file, "	%s ", current_variable->name);
        switch (current_variable->arrayType) {
            case integerArray: {
                fprintf(file, "dq ");
                for (int i = 0; i < current_variable->arraySize; i++) {
                    fprintf(file, "%d", current_variable->arrayValue.integerArray[i]);
                    if(i < current_variable->arraySize - 1){
                    	fprintf(file, ",");
                    }
                }
                break;
            }
            case doubleArray: {
                fprintf(file, "dq ");
                for (int i = 0; i < current_variable->arraySize; i++) {
                    fprintf(file, "%f", current_variable->arrayValue.doubleArray[i]);
                    if(i < current_variable->arraySize - 1){
                    	fprintf(file, ",");
                    }
                }
                break;
            }
            case stringArray: {
                fprintf(file, "db ");
                for (int i = 0; i < current_variable->arraySize; i++) {
                    fprintf(file, "'%s', 0", current_variable->arrayValue.stringArray[i]);
                    if(i < current_variable->arraySize - 1){
                    	fprintf(file, ",");
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    
}

void createBack(Stack *contstack, int id, int returnFlow, int nested, int floorNum){
    AssemblyInstruction instruction;
	const char *name = "contifstatement";
	if(floorNum == 0){
	snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    	push(contstack, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "jmp _Start%d \n", (returnFlow+1));
    	push(contstack, instruction);
	}else{
	    snprintf(instruction.instruction, sizeof(instruction.instruction), "\n");
    	push(contstack, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "jmp %s%d", name, (id-1));
    	push(contstack, instruction);
    }
    
    
}

void dumpIfs(FILE *file, HighStatementIf *highTable){
    HighStatementIf *current_hs, *tmp;
    AssemblyInstruction instruction;
    const char *name = "contifstatement";
    fprintf(file, "\n");

    HASH_ITER(hh, highTable, current_hs, tmp) {
    	if(current_hs->nested == 0 && current_hs->floor > 0){
    	    snprintf(instruction.instruction, sizeof(instruction.instruction), "jmp %s%d", name, ((current_hs->id)-1));
    		push(current_hs->stack, instruction);
    	}
        dumpStack(file, current_hs->stack);
        createBack(current_hs->contstack, current_hs->id, current_hs->returnFlow, current_hs->nested, current_hs->floor);
        dumpStack(file, current_hs->contstack);
    }
}

void dumpFors(FILE *file, ForStatement *forStatementTable){
    ForStatement *current_for, *tmp;
    AssemblyInstruction instruction;    
    
    HASH_ITER(hh, forStatementTable, current_for, tmp) {
    	Stack *backLoop = (Stack *)malloc(sizeof(Stack));
    	Stack *frontLoop = (Stack *)malloc(sizeof(Stack)); 
        snprintf(instruction.instruction, sizeof(instruction.instruction), "%s:",current_for->forLoopName);
    	push(frontLoop, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	cmp rcx, rbx");
    	push(frontLoop, instruction);
    	const char *conditionT;
    	switch(current_for->condition.conditionType){
    	    case fbigger:
    	        conditionT = "jl";
    	    break;
    	    case fsmaller:
    	        conditionT = "jg";
    	    break;
    	    case fequalOrBigger:
    	         conditionT = "jle";
    	    break;
    	    case fequalOrSmaller:
    	         conditionT = "jge";
    	    break;
    	}
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s _end%s",conditionT, current_for->forLoopName);
    	push(frontLoop, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	push rcx");
    	push(frontLoop, instruction);
    	dumpStack(file, frontLoop);
    	//push stack here
    	dumpStack(file, current_for->stack);
    	//    	
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	pop rcx");
    	push(backLoop, instruction);
    	const char *endConditionT;
    	switch(current_for->type){
    	    case finc:
    	        endConditionT = "inc";
    	    break;
    	    case fdec:
    	        endConditionT = "dec";
    	    break;
    	}
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	%s rcx", endConditionT);
    	push(backLoop, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "	jmp %s", current_for->forLoopName);
    	push(backLoop, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "_end%s:", current_for->forLoopName);
    	push(backLoop, instruction);
    	snprintf(instruction.instruction, sizeof(instruction.instruction), "RET");
    	push(backLoop, instruction);
    	dumpStack(file, backLoop);
    }
}
