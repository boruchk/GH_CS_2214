
CS-UY 2214 - Project_2
simcache.cpp

Boruch Khazanovich
bk2780@nyu.edu


As far as I was able to test, the program works as per spec.

The general algorithm that I used was as follows: (by calling the putAndFind function of the cache)
Create the correct amount of cache object(s) with provided RowCt(s), assoc(s), and blocksize(s)
 -- LW --
For a processor with only one cache:
1)  Calculate the blockID, row, and tag
2)  Loop through the cells at row to find if the tag matches any of the stored tags
    If there is a match set the lru to the current program loop number return "HIT"
    If no match was found:
3)  If all cells are full:
    Find the least recently used cell,
    set the lru to the current program loop number
    store the tag in that cell
    return MISS
4)  If not all cells are full:
    Find an empty cell,
    set the lru to the current program loop number
    store the tag in that cell
    return MISS

For a processor with 2 caches:
Do all steps 1-4 for the L1 cache.
If the LW function returns false (L1 MISS):
Do all steps 1-4 for the L2 cache.
 -- LW --

 -- SW --
For a processor with only one cache, call the putAndFind function for L1.
For a processor with 2 caches, call the putAndFind function for L1 and then for L2.
 -- SW --



One thing I would change is make a vector of caches' in the begining of 'main' in order to
prevent code reuse of the large while loop.