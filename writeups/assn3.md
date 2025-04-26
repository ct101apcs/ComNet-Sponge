Assignment 3 Writeup
=============

My name: NGUYEN Phan Cao Tri

My POVIS ID: npctri22sgn

My student ID (numeric): 49005169

This assignment took me about 20 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
TCPSender
- _isn (WrappingInt32): The initial sequence number of the TCP connection.
- _segments_out (Queue): A queue that holds the segments to be sent.
- _initial_restranmission_timeout (int): restranmission timer to track the time of the first segment.
- _stream (ByteStream): The stream of data to be sent.
- _timer (Timer): timer state
- _next_seqno (uint64_t): The next sequence number to be sent.
- _abs_acked (uint64_t): The absolute acked number.
- _window_size (uint64_t): The size of the window.
- _outstanding_segments (Queue): A queue that holds the segments that have been sent but not yet acknowledged.
- _byte_in_flight (size_t)
- _elapsed_time (int): The time elapsed since the last segment was sent.
- _retransmission_timeout (int): The time to wait before retransmitting a segment.
- _time_expired (int): The time when the segment was sent.
- _restranmission_timeout (uint16_t): The time to wait before retransmitting a segment.
- _consecutive_retransmission (int): The number of consecutive retransmissions.

Implementation Challenges:
[]

Remaining Bugs:
I am still facing the issues with the case when nothing has been acknowledged yet, my work is sending an extra segment while should not. I initially think that the timer is not waitng the full 2363s before expiring. However, I still have not fix it successfully,

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
