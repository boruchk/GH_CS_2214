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
size_t const static MEM_SIZE = 0b1111111111111;
size_t const static REG_SIZE = 0b1111111111111111;


/*      GIVEN FUNCTIONS         */
void loadMachineCode(ifstream &f, unsigned mem[]);
void printState(unsigned pc, unsigned regs[], unsigned memory[], size_t memQuantity);

/*      GENERAL FUNCTIONS       */
int signExtend(int& num);
int pcIncFunc(int& pcCtr, int additional = 1);

/*      INSTRUCTION FUNCTIONS   */
int addFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr);
int addiFunc(int src, int dst, int immVal, unsigned int regs[], int& pcCtr);
int subFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr);
int orFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr);
int andFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr);
void sltFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr);
void sltiFunc(int src, int dst, int immVal, unsigned int regs[], int& pcCtr);
int lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int *mem, int& pcCtr);
int swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, int& pcCtr);
void jrFunc(int regVal, unsigned int regs[], int& pcCtr);
void jeqFunc(int regAVal, int regBVal, int immVal, unsigned int regs[], int& pcCtr);
void jFunc(int immVal, int& pcCtr, bool& isHalt);
void jalFunc(int immVal, unsigned int regs[], int& pcCtr);




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
    int pcCtr       = 0;
    int reg         = 0b111;
    int regDst      = 0b0000000001110000;
    int imm         = 0b0000000000001111;
    int sevenImm    = 0b0000000001111111;
    int thirteenImm = 0b0001111111111111;

    while (!isHalt) {
        /*
            isHalt will remain false until changed by the jFunc
            checks the 3 msb as well as 4 lsb to decipher the instruction
            collects the needed data to perform said instruction
            calls the correct instruction function to be executed
        */
        if ((mem[pcCtr]>>13 & 0b111) == 0) {
            if ((mem[pcCtr] & imm) == 0) {
                int srcA = (mem[pcCtr]>>10) & reg;
                int srcB = (mem[pcCtr]>>7) & reg;
                int dst  = (mem[pcCtr]>>4) & reg;
                addFunc(srcA, srcB, dst, regs, pcCtr);
            }
            else if ((mem[pcCtr] & imm) == 1) {
                int srcA = (mem[pcCtr]>>10) & reg;
                int srcB = (mem[pcCtr]>>7) & reg;
                int dst  = (mem[pcCtr]>>4) & reg;
                subFunc(srcA, srcB, dst, regs, pcCtr);
            }
            else if ((mem[pcCtr] & imm) == 2) {
                int srcA = (mem[pcCtr]>>10) & reg;
                int srcB = (mem[pcCtr]>>7) & reg;
                int dst  = (mem[pcCtr]>>4) & reg;
                orFunc(srcA, srcB, dst, regs, pcCtr);
            }
            else if ((mem[pcCtr] & imm) == 3) {
                int srcA = (mem[pcCtr]>>10) & reg;
                int srcB = (mem[pcCtr]>>7) & reg;
                int dst  = (mem[pcCtr]>>4) & reg;
                andFunc(srcA, srcB, dst, regs, pcCtr);
            }
            else if ((mem[pcCtr] & imm) == 4) {
                int srcA = (mem[pcCtr]>>10) & reg;
                int srcB = (mem[pcCtr]>>7) & reg;
                int dst  = (mem[pcCtr]>>4) & reg;
                sltFunc(srcA, srcB, dst, regs, pcCtr);
            }
            else if ((mem[pcCtr] & imm) == 8) {
                int regVal = (mem[pcCtr]>>10) & reg;
                jrFunc(regVal, regs, pcCtr);
            }
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 7) {
            int src = (mem[pcCtr]>>10) & reg;
            int dst  = (mem[pcCtr]>>7) & reg;
            int immVal = mem[pcCtr] & sevenImm;
            signExtend(immVal);
            sltiFunc(src, dst, immVal, regs, pcCtr);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 4) {
            int regVal = (mem[pcCtr]>>10) & reg;
            int dst  = (mem[pcCtr]>>7) & reg;
            int immVal = mem[pcCtr] & sevenImm;
            signExtend(immVal);
            lwFunc(regVal, dst, immVal, regs, mem, pcCtr);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 5) {
            int regVal = (mem[pcCtr]>>10) & reg;
            int src  = mem[pcCtr] & reg;
            int immVal = mem[pcCtr] & sevenImm;
            signExtend(immVal);
            swFunc(regVal, src, immVal, regs, mem, pcCtr);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 6) {
            int regAVal = (mem[pcCtr]>>10) & reg;
            int regBVal  = (mem[pcCtr]>>7) & reg;
            int relImmVal = mem[pcCtr] & sevenImm;
            signExtend(relImmVal);
            jeqFunc(regAVal, regBVal, relImmVal, regs, pcCtr);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 1) {
            int src     = (mem[pcCtr]>>10) & reg;
            int dst     = (mem[pcCtr]>>7) & reg;
            int immVal  = mem[pcCtr] & sevenImm;
            signExtend(immVal);
            addiFunc(src, dst, immVal, regs, pcCtr);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 2) {
            int immVal = mem[pcCtr] & thirteenImm;
            jFunc(immVal, pcCtr, isHalt);
        }
        else if ((mem[pcCtr]>>13 & 0b111) == 3) {
            int immVal = mem[pcCtr] & thirteenImm;
            jalFunc(immVal, regs, pcCtr);
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
*/
int pcIncFunc(int& pcCtr, int additional) {
    if (additional == 1) { pcCtr +=1; }
    else {
        int result = pcCtr + additional;
        if (result > MEM_SIZE) { pcCtr = result & MEM_SIZE; }
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
int addFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr) {
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
int addiFunc(int src, int dst, int immVal, unsigned int regs[], int& pcCtr) {
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
int subFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr) {
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
int orFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr) {
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
int andFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr) {
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
void sltFunc(int srcA, int srcB, int dst, unsigned int regs[], int& pcCtr) {
    if (dst == 0) { pcIncFunc(pcCtr); }
    if (regs[srcA] < regs[srcB]) { regs[dst] = 1; }
    pcIncFunc(pcCtr);
}

/*
    first check if we want to alter the value in register 0:
        dont do anything
        increment pcCtr by 1
        return
    otherwise:
    if regs[srcA] < immVal:
        set regs[dst] = 1
    increment pcCtr by 1
*/
void sltiFunc(int src, int dst, int immVal, unsigned int regs[], int& pcCtr)  {
    if (dst == 0) { pcIncFunc(pcCtr); }
    else if (regs[src] < immVal) { regs[dst] = 1; }
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
int lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int *mem, int& pcCtr) {
    if (dst == 0) {
        pcIncFunc(pcCtr);
        return 1;
    }
    int location = regs[regVal] + immVal;
    if (location < 0) {
        regs[dst] = mem[MEM_SIZE + location];
        pcIncFunc(pcCtr);
        return 1;
    }
    if (location > MEM_SIZE) {
        regs[dst] = mem[MEM_SIZE - location];
        pcIncFunc(pcCtr);
        return 1;
    }
    regs[dst] = mem[location];
    pcIncFunc(pcCtr);
    return 0;
}

// similar to lw:
// instead of loading from mem into register dst, 
// a value is instead loaded from location into mem
int swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, int& pcCtr) {
    int location = regs[regVal] + immVal;
    if (location > 0 && location < sizeof(mem)) {
        pcIncFunc(pcCtr);
        return 1;
    }
    if (location < 0) {
        mem[MEM_SIZE + location] = regs[src];
        pcIncFunc(pcCtr);
        return 1;
    }
    if (location > MEM_SIZE) {
        mem[MEM_SIZE - location] = regs[src];
        pcIncFunc(pcCtr);
        return 1;
    }
    mem[location] = regs[src];
    pcIncFunc(pcCtr);
    return 0;
}

// call the pcIncFunc with 0 as the first parameter
void jrFunc(int regVal, unsigned int regs[], int& pcCtr) {
    pcCtr = 0;
    pcCtr = pcIncFunc(pcCtr, regs[regVal]);
}

// check if values in register A and B are equal.
// if true: set pcCtr to immVal + 1.
void jeqFunc(int regAVal, int regBVal, int immVal, unsigned int regs[], int& pcCtr) {
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
void jFunc(int immVal, int& pcCtr, bool& isHalt) {
    if (immVal == pcCtr) { isHalt = true; }
    else {
        pcCtr = 0;
        pcIncFunc(pcCtr, immVal);
    }
}

// set register 7 to the current pcCtr value + 1
// set pcCtr to the immVal
void jalFunc(int immVal, unsigned int regs[], int& pcCtr) {
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