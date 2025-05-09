Assignment 5 Writeup
=============

My name: NGUYEN Phan Cao Tri

My POVIS ID: npctri22sgn

My student ID (numeric): 49005169

This assignment took me about 16 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
- _ethernet_address (EthernetAddress): Ethernet address of the interface
- _ip_address (Address): IP address of the interface
- _frames_out (std::queue<EthernetFrame>): outbound queue of Ethernet frames that the NetworkInterface wants sent
- _arp_cache (std::map<uint32_t, std::pair<EthernetAddress, size_t>>): IPAddress to EthernetAddress mapping cache
- _pending_datagrams (std::deque<std::pair<uint32_t, InternetDatagram>>): queue of datagrams waiting for ARP resolution
- _arp_request_timeouts (std::map<uint32_t, size_t>): ARP request timeouts
- _current_time (size_t): current time in milliseconds

Implementation Challenges:
[]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
