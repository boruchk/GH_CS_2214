myROM[0]  = {movi,  RXX, Rg0, 4'b0111}; // multiplier
myROM[1]  = {movi,  RXX, Rg1, 4'b0010}; // multiplicand
myROM[2]  = {subi,  RXX, Rg0, 4'b0001}; // subtract 1 from the multiplier
myROM[3]  = {cmpi,  RXX, Rg0, 4'b0000}; // check if multiplier = 0
myROM[4]  = {jz,    RXX, RXX, 4'b1011}; // jump to address 3 + 11 = 14 if multiplier = 0 and end program
myROM[5]  = {add,   Rg1, Rg1, 4'b0000}; // add the multiplicand to itself
myROM[6]  = {jmp,   RXX, RXX, 4'b0000}; // jump to address 6 + 12 = 2 to continue the program
myROM[7]  = {jmp,   RXX, RXX, 4'b0000}; 
myROM[8]  = {jmp,   RXX, RXX, 4'b0000}; 
myROM[9]  = {jmp,   RXX, RXX, 4'b0000}; 
myROM[10] = {jmp,   RXX, RXX, 4'b1001}; 
myROM[11] = {jmp,   RXX, RXX, 4'b0000};
myROM[12] = {jmp,   RXX, RXX, 4'b0000}; 
myROM[13] = {jmp,   RXX, RXX, 4'b0000};
myROM[14] = {mov,   Rg1, Rg2, 4'b0000}; // move the final value in rg1 into rg2
myROM[15] = {jmp,   RXX, RXX, 4'b0000}; // end program
