#include "tcp_receiver.hh"

// Handle an incoming TCP segment
void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();

    // Ignore segments before SYN is received
    if (!_syn && !header.syn) {
        return;
    }

    // Set the Initial Sequence Number (ISN) if this is the first SYN
    if (!_syn) {
        _isn = header.seqno;
        _syn = true;
    }

    // If SYN hasn’t been received yet, skip further processing
    if (!_syn) {
        return;
    }

    // Push the payload to the reassembler
    std::string data = seg.payload().copy();
    if (data.size() > 0) {
        if (header.syn || header.seqno != _isn) {
            size_t index = unwrap(header.seqno - (!header.syn), _isn, _reassembler.first_unassembled());
            _reassembler.push_substring(data, index, header.fin);
        }
    }

    // Set the FIN flag if this is the first FIN
    if (header.fin || _fin) {
        _fin = true;
        if (_reassembler.unassembled_bytes() == 0) {
            _reassembler.stream_out().end_input();
        }
    }
}

std::optional<WrappingInt32> TCPReceiver::ackno() const {
    std::optional<WrappingInt32> res = std::nullopt;
    if (_syn) {
        uint64_t index = _reassembler.first_unassembled() + 1;
        if (_reassembler.stream_out().input_ended())
            index++;
        res.emplace(wrap(index, _isn));
    }
    return res;
}

size_t TCPReceiver::window_size() const {
    return _capacity - _reassembler.stream_out().buffer_size();
}
