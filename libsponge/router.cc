#include "router.hh"

#include <iostream>

using namespace std;

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    _routes.push_back({route_prefix, prefix_length, next_hop, interface_num});
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    // If TTL is less than 1, the datagram is invalid (about to expire), should be dropped
    if (dgram.header().ttl <= 1){
        return; 
    }

    // Initialize a pointer to the end of the routes vector, used to hold the current best match
    auto best_match = _routes.end();

    for (auto it = _routes.begin(); it != _routes.end(); ++it) {
        // Check if the datagram's destination address matches the route prefix
        uint32_t mask = (it->prefix_length!= 0 ? 0xFFFFFFFF << (32 - it->prefix_length): 0);
        if ((dgram.header().dst & mask) == (it->route_prefix & mask)) {
            if (best_match == _routes.end() 
                || it->prefix_length > best_match->prefix_length) {
                best_match = it;
            }
        }
    }

    // If a matching route is found, send the datagram out on the appropriate interface
    if (best_match != _routes.end()) {
        --dgram.header().ttl;           // prenvet infinite loop
        auto next_hop = best_match->next_hop;
        size_t interface_num = best_match->interface_num; // interface number is the index of outgoing interface

        if (next_hop.has_value()){
            _interfaces[interface_num].send_datagram(dgram, *next_hop);
        } else {
            // If the route is directly attached, send the datagram out on the interface
            _interfaces[interface_num].send_datagram(dgram, Address::from_ipv4_numeric(dgram.header().dst));
        }
    }
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
