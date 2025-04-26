Assignment 4 Writeup
=============

My name: NGUYEN Phan Cao Tri

My POVIS ID: npctri22sgn

My student ID (numeric): 49005169

This assignment took me about 20 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [0.00, 0.00]

Program Structure and Design of the TCPConnection:

- _cfg (TCPConfig): contains the configuration of the TCP connection, including the receiver and sender capacities, the initial sequence number, and the round-trip timeout.
- _receiver (TCPReceiver): handles the incoming data stream and manages the state of the receiver.
- _sender (TCPSender): manages the outgoing data stream and handles the sending of segments.
- _segments_out (std::queue<TCPSegment>): a queue of segments that the TCPConnection wants to send.
- _linger_after_streams_finish (bool): indicates whether the connection should remain active after both streams have finished.
- _time_last_recv (size_t): the time of the last received segment.
- _active (bool): indicates whether the connection is still active.
- _kill_connection (void): kills the connection, optionally cleaning up resources.
- _send_rst_empty_segment (void): sends a RST segment with an empty payload.
- _clear_sender_buffer (void): clears the sender's buffer.


Implementation Challenges:
- The most challenging part of this assignment was the unexpected discrepancies in the behavior of the embedded Linux (I always use) and the VM. Then I needed to switch to the VM to pass all tests.
- This assignment requires careful structuring, I had to try and fail several times before getting the design right.
- There is no specific challenges since the tutorial is clear and easy to follow.

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
