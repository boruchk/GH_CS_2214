
CS-UY 2214 - Project_1
sim.cpp

Boruch Khazanovich
bk2780@nyu.edu


As far as I was able to test, the program works as requested

The general algorithm that I used was as follows:
1) find the instruction by isolating the 3 msb (and 4 lsb) as needed
2) isolate the required registers and immediate values as needed
3) call the corresponding instruction function to complete the entire instruction

If I would have more time, I would try to shorten the code as it is quite long.
I think I would be able to shorten some of the instruction functions by creating more generalized functions
like I did with signExtend() and pcIncFunc().
