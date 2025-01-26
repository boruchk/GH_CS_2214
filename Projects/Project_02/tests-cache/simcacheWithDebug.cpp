/*
CS-UY 2214
Project_2
simcache.cpp

Boruch Khazanovich
bk2780@nyu.edu
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
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


class Cache {
    private:
        string name;
        size_t rowCt;
        unsigned short associativity;
        int blockSize;
        size_t usedCells;
        // 2D vector (of tuples) to store the cache data 
        // first value is the tag and second is the lru value
        vector<vector<tuple<int, int>>> theCache;
    public:
        Cache(const string& aName, size_t rowCt, unsigned short associativity, int blockSize)
        :   rowCt(rowCt), 
            associativity(associativity),
            blockSize(blockSize),
            usedCells(0)
        {
            this->name = aName;
            // make a vector of tuples -data- the size of associativity
            // (first element stores the tag, second stores the use amt (for the LRU logic)) 
            // all elements initialized to 0
            vector<tuple<int, int>> data(associativity);
            for (size_t idx = 0; idx < associativity; ++idx) {
                data[idx] = make_tuple(9999,0);
            }
            for (size_t idx = 0; idx < rowCt; ++idx) {
                theCache.push_back(data);
            }
        }

        string putAndFind(int theAddress, int loopCt) {
            // cout << "i am in putAndFind\n";
            int blockId = theAddress / blockSize;
            int row = blockId % rowCt;
            int tag = blockId / rowCt;
            // cout << "associativity = " << associativity << ", blockSize = " << blockSize << endl;
            // cout << "theAddress = " << theAddress << ", blockID = " << blockId << ", row = " << row << ", tag = " << tag << ", loopCt = " << loopCt << endl;
            // check if any of the cells at the given row == the new tag:
            //  if yes: increment the use count of said cell and say that we found a match (HIT)
            for (size_t idx = 0; idx < theCache[row].size(); ++idx){
                if (get<0>(theCache[row][idx]) == tag) {
                    get<1>(theCache[row][idx]) = loopCt;
                    // cout << "after HIT: tag data = " << get<0>(theCache[row][idx]) << endl;
                    // cout << "after HIT: lru data = " << get<1>(theCache[row][idx]) << endl;
                    return "HIT";
                }
            }
            // OTHERWISE:
            // if all cells are full:
            //  find (an index of) a tuple cell to replace via the LRU method -findLeastUsed-
            //  store the old data in -swaped-
            //  store the new tag into the cell
            //  set the use count of said cell to 1
            //  say that we did not find a match (MISS)
            if (usedCells == theCache[row].size()) {
                size_t lruIdx = findLruIdx(theCache[row], loopCt);
                // cout << "im in ALL cells are full, oldtag data = " << get<0>(theCache[row][lruIdx]) 
                // << " oldloopCtdata = " << get<1>(theCache[row][lruIdx]) 
                // << ", lruIdx = " << lruIdx << endl;
                // cout << " NOW INSIDE FULL:\n";
                // cout << "lruIdx = " << lruIdx << endl;
                // cout << "Initially FULL: tag data = " << get<0>(theCache[row][lruIdx]) << endl;
                // cout << "Initially FULL: lru data = " << get<1>(theCache[row][lruIdx]) << endl;
                // cout << "theAddress = " << theAddress << ", blockID = " << blockId << ", row = " << row << ", tag = " << tag << ", loopCt = " << loopCt << endl;
                get<0>(theCache[row][lruIdx]) = tag;
                get<1>(theCache[row][lruIdx]) = loopCt;
                // cout << "after FULL: tag data = " << get<0>(theCache[row][lruIdx]) << endl;
                // cout << "after FULL: lru data = " << get<1>(theCache[row][lruIdx]) << endl;
                return "MISS";
            }
            // if not all cells are full:
            //  find an empty cell, store the tag there,
            //  set the use count of said cell to 1, break out of the loop
            //  say that we did not find a match (MISS)
            for (tuple<int, int>& theData : theCache[row]) {
                if (get<0>(theData) == 9999) {
                    // cout << "im in NOT all cells are full, oldtag data = " << get<0>(theData)
                    // << " oldloopCtdata = " << get<1>(theData) << endl;
                    // cout << "before NOT FULL: tag data = " << get<0>(theCache[row][0]) << endl;
                    // cout << "before NOT FULL: lru data = " << get<1>(theCache[row][0]) << endl;
                    get<0>(theData) = tag;
                    get<1>(theData) = loopCt;
                    usedCells += 1;
                    // cout << "after NOT FULL: tag data = " << get<0>(theCache[row][0]) << endl;
                    // cout << "after NOT FULL: lru data = " << get<1>(theCache[row][0]) << endl;
                    return "MISS";
                }
            }
            return "";
        }
        size_t findLruIdx(vector<tuple<int, int>>& data, int currloopCt) {
            int lruVal = 9999;
            for (size_t idx = 0; idx < data.size(); ++idx) {
                lruVal = min(lruVal, get<1>(data[idx]));
            }
            for (size_t idx = 0; idx < data.size(); ++idx) {
                if (get<1>(data[idx]) == lruVal) {
                    return idx;
                }
            }
            return data.size();
        }
        // void store(int theAddress) {
        //     cout << "i am in store\n";
        //     int blockId = theAddress / blockSize;
        //     int row = blockId % rowCt;
        //     int tag = blockId / rowCt;
        //     cout << "theAddress = " << theAddress << ", blockID = " << blockId << ", row = " << row << ", tag = " << tag << endl;
        //     if (usedCells == theCache[row].size()) {
        //         int lru = findLeastUsed(theCache[row]);
        //         tuple<int, int>& theData = theCache[row][lru];
        //         get<0>(theData) = tag;
        //         get<1>(theData) = 1;
        //         cout << "in usedCells == theCache[row].size() -- get<0>(theData) = " << get<0>(theData) << ", tag = " << tag << endl;
        //     }
        //     else {
        //         for (tuple<int, int>& theData : theCache[row]) {
        //             if (get<0>(theData) == 9999) {
        //                 cout << "im in not all cells are full, get<0>(theData) = " << get<0>(theData) << ", tag = " << tag << endl;
        //                 get<0>(theData) = tag;
        //                 get<1>(theData) = 1;
        //                 usedCells += 1;
        //             }
        //         }
        //     }
        // }
        const string& getName() const { return name; }
        int getRow(int theAddress) const { return (theAddress / blockSize) % rowCt; }
};


/*      GIVEN FUNCTIONS         */
void loadMachineCode(ifstream &f, unsigned mem[]);
// void printState(unsigned pc, unsigned regs[], unsigned memory[], size_t memQuantity);
void print_cache_config(const string &cache_name, int size, int assoc, int blocksize, int num_rows);
void print_log_entry(const string &cache_name, const string &status, int pc, int addr, int row);

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
bool lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int mem[], uint16_t& pcCtr, Cache& theCache, int loopCt);
void swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr, Cache& theCache, int loopCt);
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
    char *filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    string cache_config;
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-",0)==0) {
            if (arg== "-h" || arg == "--help")
                do_help = true;
            else if (arg=="--cache") {
                i++;
                if (i>=argc)
                    arg_error = true;
                else
                    cache_config = argv[i];
            }
            else
                arg_error = true;
        } else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr) {
        cerr << "usage " << argv[0] << " [-h] [--cache CACHE] filename" << endl << endl;
        cerr << "Simulate E20 cache" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl<<endl;
        cerr << "optional arguments:"<<endl;
        cerr << "  -h, --help  show this help message and exit"<<endl;
        cerr << "  --cache CACHE  Cache configuration: size,associativity,blocksize (for one"<<endl;
        cerr << "                 cache) or"<<endl;
        cerr << "                 size,associativity,blocksize,size,associativity,blocksize"<<endl;
        cerr << "                 (for two caches)"<<endl;
        return 1;
    }

    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file "<< filename <<endl;
        return 1;
    }
    

    unsigned int mem[MEM_SIZE] = {0};   // array of 8,191 memory cells each initialized with 0 
                                        // each with a max size of 8191
    unsigned int regs[NUM_REGS] = {0};  // array of 8 registers each initialized with 0  
                                        // each with a max size of 65535
    loadMachineCode(f, mem);            // read from machine code file

    bool isHalt     = false;            // will become true when the end of the program is reached
    // bool l2Test     = false;            // will become true when an l2 cache is made
    int loopCt     = 0;
    uint16_t pcCtr  = 0;
    int reg         = 0b111;
    // int regDst      = 0b0000000001110000;
    int imm         = 0b0000000000001111;
    int sevenImm    = 0b0000000001111111;
    int thirteenImm = 0b0001111111111111;
    // uint16_t memSizeMask = 0b1111111111111;

    /* parse cache config */
    if (cache_config.size() > 0) {
        vector<int> parts;
        size_t pos;
        size_t lastpos = 0;
        while ((pos = cache_config.find(",", lastpos)) != string::npos) {
            parts.push_back(stoi(cache_config.substr(lastpos,pos)));
            lastpos = pos + 1;
        }
        parts.push_back(stoi(cache_config.substr(lastpos)));
        if (parts.size() == 3) {
            int l1size = parts[0];
            int l1assoc = parts[1];
            int l1blocksize = parts[2];
            int l1RowCt = l1size/(l1blocksize*l1assoc);

            // TODO: execute E20 program and simulate one cache here

            Cache l1("l1", l1RowCt, l1assoc, l1blocksize);
            print_cache_config(l1.getName(), l1size, l1assoc, l1blocksize, l1RowCt);

            // if l1.getData(): print with hit
            // else: print with miss
            while (!isHalt) {
                /*
                    isHalt will remain false until changed by the jFunc
                    checks the 3 msb as well as 4 lsb to decipher the instruction
                    collects the needed data to perform said instruction
                    calls the correct instruction function to be executed
                */
                loopCt += 1;
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
                    if ((mem[pcCtr] & imm) == 0) {          // ADD
                        // cout << "i am in add \n";
                        addFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 1) {     // SUB
                        // cout << "i am in sub \n";
                        subFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 2) {     // OR
                        // cout << "i am in or \n";
                        orFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 3) {     // AND
                        // cout << "i am in and \n";
                        andFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 4) {     // SLT
                        // cout << "i am in slt \n";
                        sltFunc(usrcA, usrcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 8) {     // JR
                        // cout << "i am in jr \n";
                        jrFunc(regVal, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 7) {   // SLTI
                    // cout << "i am in slti \n";
                    signExtend(immVal);
                    sltiFunc(usrc, dsti, immVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 4) {   // LW
                    // cout << "i am in lw \n";
                    lwFunc(regVal, dsti, immVal, regs, mem, pcCtr, l1, loopCt);
                    pcIncFunc(pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 5) {   // SW
                    // cout << "i am in sw \n";
                    swFunc(regVal, dsti, immVal, regs, mem, pcCtr, l1, loopCt);
                    pcIncFunc(pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 6) {   // JEQ
                    // cout << "i am in jeq \n";
                    // cout << "regAVal = " << regs[regAVal] << ", regBVal = " << regBVal << ", relImmVal = " << relImmVal << ", regs = " << regs << ", pcCtr = " << pcCtr << endl;
                    signExtend(relImmVal);
                    jeqFunc(regAVal, regBVal, relImmVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 1) {   // ADDI
                    // cout << "i am in addi \n";
                    signExtend(immVal);
                    addiFunc(src, dsti, immVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 2) {   // J
                    // cout << "i am in j \n";
                    jFunc(thirteenImmVal, pcCtr, isHalt);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 3) {   // JAL
                    // cout << "i am in jal \n";
                    jalFunc(thirteenImmVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
            }
        }

        else if (parts.size() == 6) {  
            int l1size = parts[0];
            int l1assoc = parts[1];
            int l1blocksize = parts[2];
            int l1RowCt = l1size/(l1blocksize*l1assoc);
            int l2size = parts[3];
            int l2assoc = parts[4];
            int l2blocksize = parts[5];
            int l2RowCt = l2size/(l2blocksize*l2assoc);

            // TODO: execute E20 program and simulate two caches here
            Cache l1("l1", l1RowCt, l1assoc, l1blocksize);
            Cache l2("l2", l2RowCt, l2assoc, l2blocksize);
            print_cache_config(l1.getName(), l1size, l1assoc, l1blocksize, l1RowCt);
            print_cache_config(l2.getName(), l2size, l2assoc, l2blocksize, l2RowCt);            

            while (!isHalt) {
                loopCt += 1;
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
                    if ((mem[pcCtr] & imm) == 0) {          // ADD
                        // cout << "i am in add \n";
                        addFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 1) {     // SUB
                        // cout << "i am in sub \n";
                        subFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 2) {     // OR
                        // cout << "i am in or \n";
                        orFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 3) {     // AND
                        // cout << "i am in and \n";
                        andFunc(srcA, srcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 4) {     // SLT
                        // cout << "i am in slt \n";
                        sltFunc(usrcA, usrcB, dst, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                    else if ((mem[pcCtr] & imm) == 8) {     // JR
                        // cout << "i am in jr \n";
                        jrFunc(regVal, regs, pcCtr);
                        // printState(pcCtr, regs, mem, 128);
                    }
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 7) {   // SLTI
                    // cout << "i am in slti \n";
                    signExtend(immVal);
                    sltiFunc(usrc, dsti, immVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 4) {   // LW
                    // cout << "i am in lw \n";
                    bool hitTest = lwFunc(regVal, dsti, immVal, regs, mem, pcCtr, l1, loopCt);
                    if (!hitTest) { lwFunc(regVal, dsti, immVal, regs, mem, pcCtr, l2, loopCt); }
                    pcIncFunc(pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 5) {   // SW
                    // cout << "i am in sw \n";
                    swFunc(regVal, dsti, immVal, regs, mem, pcCtr, l1, loopCt);
                    swFunc(regVal, dsti, immVal, regs, mem, pcCtr, l2, loopCt);
                    pcIncFunc(pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 6) {   // JEQ
                    // cout << "i am in jeq \n";
                    // cout << "regAVal = " << regs[regAVal] << ", regBVal = " << regBVal << ", relImmVal = " << relImmVal << ", regs = " << regs << ", pcCtr = " << pcCtr << endl;
                    signExtend(relImmVal);
                    jeqFunc(regAVal, regBVal, relImmVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 1) {   // ADDI
                    // cout << "i am in addi \n";
                    signExtend(immVal);
                    addiFunc(src, dsti, immVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 2) {   // J
                    // cout << "i am in j \n";
                    jFunc(thirteenImmVal, pcCtr, isHalt);
                    // printState(pcCtr, regs, mem, 128);
                }
                else if ((mem[pcCtr]>>13 & 0b111) == 3) {   // JAL
                    // cout << "i am in jal \n";
                    jalFunc(thirteenImmVal, regs, pcCtr);
                    // printState(pcCtr, regs, mem, 128);
                }
            }
        } 
        
        else {
            cerr << "Invalid cache config"  << endl;
            return 1;
        }
    }

    /*  DECODE INSTRUCTION AND CALL CORRESPONDING FUNCTION  */

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
        if (result > MEM_SIZE) { pcCtr = result & MEM_BIT_SIZE; }
        else { pcCtr = result; }
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
    size_t result = regs[srcA] + regs[srcB];
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
    size_t result = regs[src] + immVal;
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
    size_t result = regs[srcA] - regs[srcB];
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
    if (dst != 0) {
        if (regs[src] < theImmVal) { regs[dst] = 1; }
        else { regs[dst] = 0; }
    }
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
bool lwFunc(int regVal, int dst, int immVal, unsigned int regs[], unsigned int mem[], uint16_t& pcCtr, Cache& theCache, int loopCt) {
    int address = regs[regVal] + immVal;
    string test = "MISS";
    // cout << "address = " << address << ", dst = " << dst << endl;
    if (dst == 0) {
        return true;
    }
    else if (address < 0) {
        // cout << "im in address < 0\n";
        address = MEM_SIZE + address;
        regs[dst] = mem[address];
        test = theCache.putAndFind(address, loopCt);
        // cout << test << endl;
        print_log_entry(theCache.getName(), test, pcCtr, address, theCache.getRow(address));
    }
    else {
        // cout << "im in else\n";
        // cout << "address before: " << address << endl;
        // Bit mask location to only use the 13 lsb's
        address = address & MEM_BIT_SIZE;
        // cout << "address after: " << address << endl;
        regs[dst] = mem[address];
        test = theCache.putAndFind(address, loopCt);
        print_log_entry(theCache.getName(), test, pcCtr, address, theCache.getRow(address));
    }
    if (test == "HIT") return true;
    else return false;
}

// similar to lw:
// instead of loading from mem into register dst, 
// a value is instead loaded from location into mem
void swFunc(int regVal, int src, int immVal, unsigned int regs[], unsigned int *mem, uint16_t& pcCtr, Cache& theCache, int loopCt) {
    size_t address = regs[regVal] + immVal;
    if (address > 0 && address < sizeof(mem)) {
        pcIncFunc(pcCtr);
    }
    else if (address < 0) {
        theCache.putAndFind(address, loopCt);
        // theCache.store(address);
        mem[MEM_SIZE + address] = regs[src];
        print_log_entry(theCache.getName(), "SW", pcCtr, address, theCache.getRow(address));
    }
    else if (address > MEM_SIZE) {
        theCache.putAndFind(address, loopCt);
        // theCache.store(address);
        mem[MEM_SIZE - address] = regs[src];
        print_log_entry(theCache.getName(), "SW", pcCtr, address, theCache.getRow(address));
    }
    else {
        theCache.putAndFind(address, loopCt);
        // theCache.store(address);
        mem[address] = regs[src];
        print_log_entry(theCache.getName(), "SW", pcCtr, address, theCache.getRow(address));
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



/*  GIVEN FUNCTIONS:  */
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
// void printState(unsigned pc, unsigned regs[], unsigned memory[], size_t memQuantity) {
//     cout << setfill(' ');
//     cout << "Final state:" << endl;
//     cout << "\tpc=" <<setw(5)<< pc << endl;

//     for (size_t reg=0; reg<NUM_REGS; reg++)
//         cout << "\t$" << dec << reg << "="<<setw(5)<<regs[reg]<<endl;

//     cout << setfill('0');
//     bool cr = false;
//     for (size_t count=0; count<memQuantity; count++) {
//         cout << hex << setw(4) << memory[count] << " ";
//         cr = true;
//         if (count % 8 == 7) {
//             cout << endl;
//             cr = false;
//         }
//     }
//     if (cr)
//         cout << endl;
// }

void print_cache_config(const string &cache_name, int size, int assoc, int blocksize, int num_rows) {
    cout << "Cache " << cache_name << " has size " << size <<
        ", associativity " << assoc << ", blocksize " << blocksize <<
        ", rows " << num_rows << endl;
}

void print_log_entry(const string &cache_name, const string &status, int pc, int addr, int row) {
    cout << left << setw(8) << cache_name + " " + status <<  right <<
        " pc:" << setw(5) << pc <<
        "\taddr:" << setw(5) << addr <<
        "\trow:" << setw(4) << row << endl;
}
/*  GIVEN FUNCTIONS:  */

//ra0Eequ6ucie6Jei0koh6phishohm9

// g++ -Wall -o simcache *.cpp