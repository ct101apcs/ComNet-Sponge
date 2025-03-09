Assignment 1 Writeup
=============

My name: NGUYEN Phan Cao Tri

My POVIS ID: npctri22sgn

My student ID (numeric): 49005169
This assignment took me about 8 hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler: 
- _output: The reassembled ByteStream
- _capacity: The maximum memory usage allowed
- _first_unassembled: The index of the next byte to be written to the output
- _unassembled_segments: Map of index -> segment for unassembled data
- _eof_index: The index of the last byte of the reassembled data

Implementation Challenges: 
- I find it really difficult to truncate, merge, and handle all logic of the reassembler, it took me a lot of time.
- My work contains reference at some points (as mentioned in the source code) from CS144 materials.

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: the assignment of previous week, which caused bugs. I had to spend a lot of time to debug the assn1, but then I realized that the bug was from the previous week.

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
