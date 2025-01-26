/*
CS-UY 2214
Project_1
sim.cpp

Boruch Khazanovich
bk2780@nyu.edu
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <cstdlib>
using namespace std;


// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 8192;
size_t const static MEM_BIT_SIZE = 0b1111111111111;
size_t const static REG_SIZE = 0b1111111111111111;
// make separate vari for bit masking and set MEM-SIZE TO THE RIGHT NUMBER


/*      GIVEN FUNCTIONS         */
void loadMachineCode(ifstream &f, unsigned mem[]);
void printState(unsigned pc, unsigned regs[], unsigned memory[], size_t memQuantity);

/*      GENERAL FUNCTIONS       */
int signExtend(int& num);
uint16_t pcIncFunc(uint16_t& pcCtr, int additional = 1);

/*      INSTRUCTION FUNCTIONS   */
int addFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr);
int addiFunc(int src, int dst, int immVal, unsigned int regs[], uint16_t& pcCtr);
int subFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr);
int orFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr);
int andFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr);
void sltFunc(uint16_t srcA, uint16_t srcB, int dst, unsigned int regs[], uint16_t& pcCtr);
void sltiFunc(int src, int dst, int immVal, unsigned int regs[], uint16_t& pcCtr);
void lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr);
void swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr);
void jrFunc(int regVal, unsigned int regs[], uint16_t& pcCtr);
void jeqFunc(int regAVal, int regBVal, int immVal, unsigned int regs[], uint16_t& pcCtr);
void jFunc(uint16_t immVal, uint16_t& pcCtr, bool& isHalt);
void jalFunc(uint16_t immVal, unsigned int regs[], uint16_t& pcCtr);


/*
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char *argv[]) {
    /*
        Parse the command-line arguments
    */
    char *fileName = nullptr;
    bool doHelp = false;
    bool argError = false;
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-",0)==0) {
            if (arg== "-h" || arg == "--help")
                doHelp = true;
            else
                argError = true;
        } else {
            if (fileName == nullptr)
                fileName = argv[i];
            else
                argError = true;
        }
    }

    /* Display error message if appropriate */
    if (argError || doHelp || fileName == nullptr) {
        cerr << "usage " << argv[0] << " [-h] fileName" << endl << endl;
        cerr << "Simulate E20 machine" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  fileName    The file containing machine code, typically with .bin suffix" << endl << endl;
        cerr << "optional arguments:"<< endl;
        cerr << "  -h, --help  show this help message and exit" << endl;
        return 1;
    }

    ifstream f(fileName);
    if (!f.is_open()) {
        cerr << "Can't open file "<< fileName <<endl;
        return 1;
    }


    // TODO: your code here. Load f and parse using loadMachineCode

    unsigned int mem[MEM_SIZE] = {0};   // array of 8,191 memory cells each initialized with 0 
                                        // each with a max size of 8191
    unsigned int regs[NUM_REGS] = {0};  // array of 8 registers each initialized with 0  
                                        // each with a max size of 65535
    loadMachineCode(f, mem);            // read from machine code file

    bool isHalt     = false;            // will become true when the end of the program is reached
    uint16_t pcCtr  = 0;
    int reg         = 0b111;
    int regDst      = 0b0000000001110000;
    int imm         = 0b0000000000001111;
    int sevenImm    = 0b0000000001111111;
    int thirteenImm = 0b0001111111111111;
    uint16_t memSizeMask = 0b1111111111111;

    while (!isHalt) {
        /*
            isHalt will remain false until changed by the jFunc
            checks the 3 msb as well as 4 lsb to decipher the instruction
            collects the needed data to perform said instruction
            calls the correct instruction function to be executed
        */
        int srcA = (mem[pcCtr]>>10) & reg;
        int srcB = (mem[pcCtr]>>7) & reg;
        int regAVal = srcA;
        int regBVal  = srcB;
        int dst  = (mem[pcCtr]>>4) & reg;
        int src = (mem[pcCtr]>>10) & reg;
        int dsti  = (mem[pcCtr]>>7) & reg;
        int immVal = mem[pcCtr] & sevenImm;
        int relImmVal = immVal;
        int regVal = (mem[pcCtr]>>10) & reg;
        uint16_t thirteenImmVal = mem[pcCtr] & thirteenImm;
        uint16_t usrcA = (mem[pcCtr]>>10) & reg;
        uint16_t usrcB = (mem[pcCtr]>>7) & reg;
        uint16_t usrc = (mem[pcCtr]>>10) & reg;
        if ((mem[pcCtr]>>13 & 0b111) == 0) {
            if ((mem[pcCtr] & imm) == 0) {
                // cout << "i am in add \n";
                addFunc(srcA, srcB, dst, regs, pcCtr);
                // printState(pcCtr, regs, mem, 128);
            }
            else if ((mem[pcCtr] & imm) == 1) {
                // cout << "i am in sub \n";
                subFunc(srcA, srcB, dst, regs, pcCtr);
                // printState(pcCtr, regs, mem, 128);
            }
            else if ((mem[pcCtr] & imm) == 2) {
                // cout << "i am in or \n";
                orFunc(srcA, srcB, dst, regs, pcCtr);
                // printState(pcCtr, regs, mem, 128);
            }
            else if ((mem[pcCtr] & imm) == 3) {
                // cout << "i am in and \n";
                andFunc(srcA, srcB, dst, regs, pcCtr);
                // printState(pcCtr, regs, mem, 128);
            }
            else if ((mem[pcCtr] & imm) == 4) {
                // cout << "i am in slt \n";
                sltFunc(usrcA, usrcB, dst, regs, pcCtr);
                // printState(pcCtr, regs, mem, 128);
            }
            else if ((mem[pcCtr] & imm) == 8) {
                // cout << "i am in jr \n";
                jrFunc(regVal, regs, pcCtr);
                // printState(pcCtr, regs, mem, 128);
            }
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 7) {
            // cout << "i am in slti \n";
            signExtend(immVal);
            sltiFunc(usrc, dsti, immVal, regs, pcCtr);
            // printState(pcCtr, regs, mem, 128);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 4) {
            // cout << "i am in lw \n";
            signExtend(immVal);
            lwFunc(regVal, dsti, immVal, regs, mem, pcCtr);
            // printState(pcCtr, regs, mem, 128);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 5) {
            // cout << "i am in sw \n";
            signExtend(immVal);
            swFunc(regVal, src, immVal, regs, mem, pcCtr);
            // printState(pcCtr, regs, mem, 128);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 6) {
            // cout << "i am in jeq \n";
            // cout << "regAVal = " << regs[regAVal] << ", regBVal = " << regBVal << ", relImmVal = " << relImmVal << ", regs = " << regs << ", pcCtr = " << pcCtr << endl;
            signExtend(relImmVal);
            jeqFunc(regAVal, regBVal, relImmVal, regs, pcCtr);
            // printState(pcCtr, regs, mem, 128);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 1) {
            // cout << "i am in addi \n";
            signExtend(immVal);
            addiFunc(src, dsti, immVal, regs, pcCtr);
            // printState(pcCtr, regs, mem, 128);
        }
        // somethign about 13 vs 16 bits for the pcctr
        else if ((mem[pcCtr]>>13 & 0b111) == 2) {
            // cout << "i am in j \n";
            jFunc(thirteenImmVal, pcCtr, isHalt);
            // printState(pcCtr, regs, mem, 128);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 3) {
            // cout << "i am in jal \n";
            jalFunc(thirteenImmVal, regs, pcCtr);
            // printState(pcCtr, regs, mem, 128);
        }
    }

    // TODO: your code here. print the final state of the simulator before ending, using printState
    printState(pcCtr, regs, mem, 128);
    return 0;
}


/*  GENERAL FUNCTIONS   */
/*
    @param num: number to be sign extended
    if msb = 1; pad num with 1's until it reaches 16 bits
    if msb = 0; pad num with 0's until it reaches 16 bits
*/
int signExtend(int& num) {
    if (num >> 6 == 1) {
        num = (num | 0b1111111110000000);
        return num;
    }
    else if (num >> 6 == 0) {
        num = (num & 0b0000000001111111);
        return num;
    }
    return num;
}

/*
    @param pcCtr:       current pc counter value
    @param additional:  value by which to increment pcCtr (INITIALLY SET TO 1)
    @param result:      sum of pcCtr + additional
    if result is larger than 16 bits, 'and' it with MEM_SIZE
    returns the new pcCtr value

    use memSizeMask for something here
*/
uint16_t pcIncFunc(uint16_t& pcCtr, int additional) {
    if (additional == 1) { pcCtr +=1; }
    else {
        uint16_t result = pcCtr + additional;
        if (result > MEM_SIZE) { pcCtr = result & MEM_SIZE; }
        // if (result > memSizeMask) { pcCtr = result & memSizeMask; }
        else { pcCtr = result; }
        return pcCtr;
    }
    return pcCtr;
}
/*  GENERAL FUNCTIONS   */



/*  INSTRUCTION FUNCTIONS:  */

/*
    @param srcA:    address of first register of addend
    @param srcB:    address of second register of addend
    @param dst:     address of where the sum will be stored
    @param regs:    array of registers
    @param result:  sum of values stored in srcA and srcB
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    if result > the capacity of the register capacity:
        drop off the msb until it fits
    set the value in dst = result
    increment pcCtr by 1
*/
int addFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr) {
    if (dst == 0) {
        pcIncFunc(pcCtr);
        return 1;
    }
    int result = regs[srcA] + regs[srcB];
    if (result > REG_SIZE) {
        result = result & REG_SIZE;
    }
    regs[dst] = result;
    pcIncFunc(pcCtr);
    return 0;
}

/*
    @param result:  sum of immVal and value stored in src
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    if result > the capacity of the register capacity:
        drop off the msb until it fits
    set the value in dst = result
    increment pcCtr by 1
*/
int addiFunc(int src, int dst, int immVal, unsigned int regs[], uint16_t& pcCtr) {
    if (dst == 0) {
        pcIncFunc(pcCtr);
        return 1;
    }
    int result = regs[src] + immVal;
    if (result > REG_SIZE) {
        result = result & REG_SIZE;
    }
    regs[dst] = result;
    pcIncFunc(pcCtr);
    return 0;
}

// same as addFunc, just subtract at the end
int subFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr) {
    if (dst == 0) {
        pcIncFunc(pcCtr);
        return 1;
    }
    int result = regs[srcA] - regs[srcB];
    if (result > REG_SIZE) {
        result = result & REG_SIZE;
    }
    regs[dst] = result;
    pcIncFunc(pcCtr);
    return 0;    
}

/*
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    otherwise:
    'or' the values in regs[srcA] and regs[srcB]
    increment pcCtr by 1
*/
int orFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr) {
    if (dst == 0) {
        pcIncFunc(pcCtr);
        return 1;
    }
    regs[dst] = regs[srcA] | regs[srcB];
    pcIncFunc(pcCtr);
    return 0;    
}

/*
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    otherwise:
    'and' the values in regs[srcA] and regs[srcB]
    increment pcCtr by 1
*/
int andFunc(int srcA, int srcB, int dst, unsigned int regs[], uint16_t& pcCtr) {
    if (dst == 0) {
        pcIncFunc(pcCtr);
        return 1;
    }
    regs[dst] = regs[srcA] & regs[srcB];
    pcIncFunc(pcCtr);
    return 0;   
}

/*
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    otherwise:
    if regs[srcA] < regs[srcB]:
        set regs[dst] = 1
    increment pcCtr by 1
*/
void sltFunc(uint16_t srcA, uint16_t srcB, int dst, unsigned int regs[], uint16_t& pcCtr) {
    uint16_t theSrcB = srcB;
    if (dst == 0) { pcIncFunc(pcCtr); }
    if (regs[srcA] < theSrcB) { regs[dst] = 1; }
    pcIncFunc(pcCtr);
}

/*
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    otherwise:
    if regs[srcA] < (unsigned version of) immVal:
        set regs[dst] = 1
    increment pcCtr by 1
*/
void sltiFunc(int src, int dst, int immVal, unsigned int regs[], uint16_t& pcCtr)  {
    uint16_t theImmVal = immVal;
    if (dst == 0) { pcIncFunc(pcCtr); }
    else if (regs[src] < theImmVal) { regs[dst] = 1; }
    pcIncFunc(pcCtr);
}

/*
    @param regVal:      register address
    @param location:    sum of immVal and value stored in regVal
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    otherwise:
    find the memory location we want to load from 
    if the location is negative:
        set the destination register to MEM_SIZE + the negative value of location
        increment pcCtr by 1
        return
    if the location is greater than MEM_SIZE:
        set the destination register to MEM_SIZE - location
        increment pcCtr by 1
        return
    otherwise:
    set the destination register to location
    increment pcCtr by 1
*/
// int lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr) {
//     if (dst == 0) {
//         pcIncFunc(pcCtr);
//         return 1;
//     }
//     int location = regs[regVal] + immVal;
//     if (location < 0) {
//         regs[dst] = mem[MEM_SIZE + location];
//         pcIncFunc(pcCtr);
//         return 1;
//     }
//     if (location > MEM_SIZE) {
//         // use bit mask cuz this would only work for if dst = 2*MEMSIZE
//         regs[dst] = mem[MEM_SIZE - location];
//         pcIncFunc(pcCtr);
//         return 1;
//     }
//     regs[dst] = mem[location];
//     pcIncFunc(pcCtr);
//     return 0;
// }

// int swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr) {
//     int location = regs[regVal] + immVal;
//     if (location > 0 && location < sizeof(mem)) {
//         pcIncFunc(pcCtr);
//         return 1;
//     }
//     if (location < 0) {
//         mem[MEM_SIZE + location] = regs[src];
//         pcIncFunc(pcCtr);
//         return 1;
//     }
//     if (location > MEM_SIZE) {
//         mem[MEM_SIZE - location] = regs[src];
//         pcIncFunc(pcCtr);
//         return 1;
//     }
//     mem[location] = regs[src];
//     pcIncFunc(pcCtr);
//     return 0;
// }

void lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr) {
    int location = regs[regVal] + immVal;
    if (dst == 0) {
        pcIncFunc(pcCtr);
    }
    else if (location < 0) {
        regs[dst] = mem[MEM_SIZE + location];
        pcIncFunc(pcCtr);
    }
    else {
        // Bit mask location to only use the 13 lsb's
        location = location && MEM_BIT_SIZE;
        regs[dst] = mem[location];
        pcIncFunc(pcCtr);
    }
}

// similar to lw:
// instead of loading from mem into register dst, 
// a value is instead loaded from location into mem
void swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr) {
    int location = regs[regVal] + immVal;
    if (location > 0 && location < sizeof(mem)) {
        pcIncFunc(pcCtr);
    }
    else if (location < 0) {
        mem[MEM_SIZE + location] = regs[src];
        pcIncFunc(pcCtr);
    }
    else if (location > MEM_SIZE) {
        mem[MEM_SIZE - location] = regs[src];
        pcIncFunc(pcCtr);
    }
    else {
        mem[location] = regs[src];
        pcIncFunc(pcCtr);
    }
}

// call the pcIncFunc with 0 as the first parameter
void jrFunc(int regVal, unsigned int regs[], uint16_t& pcCtr) {
    pcCtr = 0;
    pcCtr = pcIncFunc(pcCtr, regs[regVal]);
}

// check if values in register A and B are equal.
// if true: set pcCtr to immVal + 1.
void jeqFunc(int regAVal, int regBVal, int immVal, unsigned int regs[], uint16_t& pcCtr) {
    if (regs[regAVal] == regs[regBVal]) { 
        immVal += 1;
        pcIncFunc(pcCtr, immVal);
    }
    else { pcIncFunc(pcCtr); }
}

/*
    check if the program is trying to halt:
        if the immVal is the same as our current pcCtr value:
        we are trying to halt, and set isHalt = true so that the main while loop will stop
    otherwise:
        set pcCtr to the immVal
*/
void jFunc(uint16_t immVal, uint16_t& pcCtr, bool& isHalt) {
    if (immVal == pcCtr) { isHalt = true; }
    else {
        pcCtr = 0;
        pcIncFunc(pcCtr, immVal);
    }
}

// set register 7 to the current pcCtr value + 1
// set pcCtr to the immVal
void jalFunc(uint16_t immVal, unsigned int regs[], uint16_t& pcCtr) {
    regs[7] = pcIncFunc(pcCtr);
    pcCtr = 0;
    pcIncFunc(pcCtr, immVal);
}


/*  INSTRUCTION FUNCTIONS:  */




/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
void loadMachineCode(ifstream &f, unsigned mem[]) {
    regex machineCodeRe("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedAddr = 0;
    string line;
    while (getline(f, line)) {
        smatch sm;
        if (!regex_match(line, sm, machineCodeRe)) {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedAddr) {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= MEM_SIZE) {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
        expectedAddr ++;
        mem[addr] = instr;
    }
}

/*
    Prints the current state of the simulator, including
    the current program counter, the current register values,
    and the first memQuantity elements of memory.

    @param pc The final value of the program counter
    @param regs Final value of all registers
    @param memory Final value of memory
    @param memQuantity How many words of memory to dump
*/
void printState(unsigned pc, unsigned regs[], unsigned memory[], size_t memQuantity) {
    cout << setfill(' ');
    cout << "Final state:" << endl;
    cout << "\tpc=" <<setw(5)<< pc << endl;

    for (size_t reg=0; reg<NUM_REGS; reg++)
        cout << "\t$" << dec << reg << "="<<setw(5)<<regs[reg]<<endl;

    cout << setfill('0');
    bool cr = false;
    for (size_t count=0; count<memQuantity; count++) {
        cout << hex << setw(4) << memory[count] << " ";
        cr = true;
        if (count % 8 == 7) {
            cout << endl;
            cr = false;
        }
    }
    if (cr)
        cout << endl;
}

//ra0Eequ6ucie6Jei0koh6phishohm9