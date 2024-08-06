# Compiler

Flex and bison compiler
### 1. <a >How to run</a>
### 2. <a>Syntax</a>
### 3. <a>Flow</a>
### 4. <a>Assembly file</a>

## How to run

### Install Flex
```bash
sudo apt-get install flex

```
### Install Bison
```bash
sudo apt-get install bison
```
### Install NASM
```bash
sudo apt install nasm
```

### Running compiler on txt file
```bash
Make
./parser < fileName.txt
```

## Syntax
### Declarations
Creates integer.
```bash
int variableName = 10;
int variableName;
```
Creates floating point num.
```bash
double variableName = 2.2;
double variableName;
```
Creates string.
```bash
string variableName = "abcd";
```
Arrays can be decalred with same types as variables.
```bash
int intArray[10];
double dbArray[5];
string strArray[12];
```
It is possible to initialize array along with its contains.
```bash
int intArray[10]{1,2,3}; (where rest will be 0.)
double dbArray[5]{1.2,2.2,3.3};
string strArray[12]{"aaa","bbb","ccc"};
```
### Assigning values
Assigning diffrent types is permitted, however assigning int variable to a double will transform integer into floating point num and add .0000 part.
```bash
int intVar = 10;
int anotherIntVar = 20;
double doubleVar = 4.4;
int intArray[10];

intVar = anotherIntVar;
intVar = 30;
intVar = .m(40*20/10+20-40*2);
doubleVar = intVar;
intArray[5] = intVar;
(...)
```
### Prinintg to the console
to print to the console you should use .p prefix.
```bash
.p variableName;
.p "whatever you want to write";
.p "writting" + varName + "even more written text";
.p intArray[4];
```
### If statements
If statements are limited and else-if statement can't be created.
If statements can be nested.
Each variable created in if or for statement can be accessed only from that statement.
```bash
if(a == b){
  if(a >= b){
    if(a < c){
     }
  }

  if(a==b){

  }
}
```
### For loops
```bash
for(int i = 0; i < 10; i++){
    //stuff
}
```
# Flow
![compiler](https://github.com/user-attachments/assets/40374664-1914-4257-9e1c-54b77f07215a)
# Assembly file
All of the declared variables are known and saved in ram inside .section data int the assembly file

![Screenshot from 2024-08-06 23-10-29](https://github.com/user-attachments/assets/fc0207ab-c45c-48c6-bd02-7d53cbc5b4f1)

Asm code is divided by flows, starting from initial Flow _Start, each time the highstatement appears the new flow is created

![Screenshot from 2024-08-06 22-52-50](https://github.com/user-attachments/assets/59e678b0-ff14-4c8e-99ca-d79a0c5d6b07)

Lastly, after the flows, loops and higherstatements there are pre-written functions which are necessary for proper operations

![Screenshot from 2024-08-06 22-53-29](https://github.com/user-attachments/assets/029049c2-17a2-4ab7-8a95-0b8c416fc77e)
