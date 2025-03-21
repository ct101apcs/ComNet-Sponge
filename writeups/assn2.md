Assignment 2 Writeup
=============

My name: NGUYEN Phan Cao Tri

My POVIS ID: npctri22sgn

My student ID (numeric): 49005169

This assignment took me about 5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
TCPReceiver:
_reassembler (StreamReassembler): The data structure for re-assembling bytes.
_capacity (size_t): The maximum number of bytes we'll store.
_syn (bool): The flag to check if the SYN packet is received.
_fin (bool): The flag to check if the FIN packet is received.
_isn (WrappingInt32): The initial sequence number of the receiver.

WrappingInt32:
_raw_value (uint32_t): The value of the WrappingInt32.

Implementation Challenges:
- The low level problems can be mentioned here, some how the just the different in handling parentheses change everything, especially in casting.

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
