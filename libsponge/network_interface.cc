#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), 
    _ip_address(ip_address),
    _arp_cache(),
    _pending_datagrams(),
    _arp_request_timeouts(),
    _current_time(0) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    auto it = _arp_cache.find(next_hop_ip);

    if (it != _arp_cache.end()) {
        // If the IP address is in the ARP cache, send the datagram
        const auto &ethernet_address = it->second.first;
        const auto &payload = dgram.serialize();
        _frames_out.emplace(_make_frame(ethernet_address, EthernetHeader::TYPE_IPv4, payload));
    } else {
        // If the IP address is not in the ARP cache, send an ARP request
        auto it_timeout = _arp_request_timeouts.find(next_hop_ip);
        if (it_timeout == _arp_request_timeouts.end()
            || _current_time - it_timeout->second > 6000) {
            // If no pending request, create a new one
            // _arp_request_timeouts[next_hop_ip] = _current_time;
            ARPMessage arp_request;
            arp_request.opcode = ARPMessage::OPCODE_REQUEST;
            arp_request.sender_ethernet_address = _ethernet_address;
            arp_request.sender_ip_address = _ip_address.ipv4_numeric();
            arp_request.target_ip_address = next_hop_ip;

            _frames_out.emplace(_make_frame(ETHERNET_BROADCAST, EthernetHeader::TYPE_ARP, BufferList(arp_request.serialize())));
            
            if (it_timeout == _arp_request_timeouts.end()) {
                _arp_request_timeouts.emplace(next_hop_ip, _current_time);
            } else {
                it_timeout->second = _current_time;
            }
        }
        // Store the datagram in the pending queue
        _pending_datagrams.emplace_back(next_hop_ip, dgram);
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    optional<InternetDatagram> result = nullopt;

    const auto &header = frame.header();
    const auto &payload = frame.payload();
    const auto &type = header.type;
    const auto &dst = header.dst;

    if (dst == _ethernet_address || dst == ETHERNET_BROADCAST) {
        if (type == EthernetHeader::TYPE_IPv4) {
            // If the frame is an IPv4 datagram, extract and return it
            InternetDatagram dgram;
            if (dgram.parse(Buffer(payload)) == ParseResult::NoError) {
                result.emplace(dgram);
            }
        } else if (type == EthernetHeader::TYPE_ARP) {
            // If the frame is an ARP message, process it
            ARPMessage arp_message;
            if (arp_message.parse(Buffer(payload)) == ParseResult::NoError) {
                const auto &sender_ip = arp_message.sender_ip_address;
                const auto &sender_ethernet = arp_message.sender_ethernet_address;

                auto it = _arp_cache.find(sender_ip);
                if (it == _arp_cache.end()) {
                    // If the sender IP is not in the ARP cache, add it
                    it->second.first = sender_ethernet;
                    it->second.second = _current_time;
                } else {
                    // If the sender IP is already in the ARP cache, update the timestamp
                    _arp_cache.emplace(sender_ip, make_pair(sender_ethernet, _current_time));
                }

                auto it_timeout = _arp_request_timeouts.find(sender_ip);
                if (it_timeout != _arp_request_timeouts.end()) {
                    // If there is a pending request for this IP, remove it
                    _arp_request_timeouts.erase(it_timeout);
                }

                _send_pending_datagrams(sender_ip);

                if (arp_message.opcode == ARPMessage::OPCODE_REQUEST
                    && arp_message.target_ip_address == _ip_address.ipv4_numeric()) {
                    // If the message is an ARP request, send an ARP reply
                    ARPMessage arp_reply;
                    arp_reply.opcode = ARPMessage::OPCODE_REPLY;
                    arp_reply.sender_ethernet_address = _ethernet_address;
                    arp_reply.sender_ip_address = _ip_address.ipv4_numeric();
                    arp_reply.target_ethernet_address = sender_ethernet;
                    arp_reply.target_ip_address = sender_ip;

                    _frames_out.emplace(_make_frame(sender_ethernet, EthernetHeader::TYPE_ARP, BufferList(arp_reply.serialize())));
                }
            }
        }
    }
    return result;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    _current_time += ms_since_last_tick;
    _remove_expired_requests();
}

void NetworkInterface::_remove_expired_requests() {
    for (auto it = _arp_cache.begin(); it != _arp_cache.end();) {
        if (_current_time - it->second.second > 6000) {
            it = _arp_cache.erase(it);
        } else {
            ++it;
        }
    }
}

void NetworkInterface::_send_pending_datagrams(uint32_t ip_address) {
    auto it = _pending_datagrams.begin();
    while (it != _pending_datagrams.end()) {
        if (it->first == ip_address) {
            const auto &ethernet_address = _arp_cache[ip_address].first;
            const auto &payload = it->second.serialize();
            _frames_out.emplace(_make_frame(ethernet_address, EthernetHeader::TYPE_IPv4, payload));
            it = _pending_datagrams.erase(it);
        } else {
            ++it;
        }
    }
}

EthernetFrame NetworkInterface::_make_frame(const EthernetAddress &dst, uint16_t type, const BufferList &payload) {
    EthernetFrame frame;
    frame.header().dst = dst;
    frame.header().src = _ethernet_address;
    frame.header().type = type;
    frame.payload() = payload;
    return frame;
}
