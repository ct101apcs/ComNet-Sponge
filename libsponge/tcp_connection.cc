#include "tcp_connection.hh"

#include <iostream>
#include <limits>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const {
    return _sender.stream_in().remaining_capacity();
}

size_t TCPConnection::bytes_in_flight() const {
    return _sender.bytes_in_flight();
}

size_t TCPConnection::unassembled_bytes() const {
    return _receiver.unassembled_bytes();
}

size_t TCPConnection::time_since_last_segment_received() const {
    return _time_last_recv;
}

void TCPConnection::_kill_connection(bool clean) {
    if (!clean) {
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
    }

    // Kill the connection permanently
    _active = false;
}

void TCPConnection::_send_rst_empty_segment(){
    _sender.send_empty_segment();
    _clear_sender_buffer();
}

void TCPConnection::_clear_sender_buffer() {
    auto &queue = _sender.segments_out();
    while (!queue.empty()) {
        TCPSegment &seg = queue.front();
        TCPHeader &header = seg.header();

        // size_t max_size = numeric_limits<size_t>::max();
        uint16_t max_size = numeric_limits<uint16_t>::max();

        if (_receiver.window_size() > max_size) {
            header.win = max_size;
        } else {
            header.win = _receiver.window_size();
        }

        header.seqno = _sender.next_seqno();
        header.ackno = _receiver.ackno().value_or(WrappingInt32{0});
        header.ack = true;

        _segments_out.push(seg);
        queue.pop();
    }
}

bool TCPConnection::_kill_condition_check() const {
    return _linger_after_streams_finish 
            && _receiver.stream_out().input_ended() 
            && _receiver.unassembled_bytes() == 0
            && _sender.stream_in().input_ended()
            && _sender.bytes_in_flight() == 0;
}

void TCPConnection::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();

    if (header.rst){
        _kill_connection(false);
        return;
    }

    _time_last_recv = 0;

    _receiver.segment_received(seg);

    if (header.ack) {
        _sender.ack_received(header.seqno, header.win);
    }

    // if the incoming segment occupied any sequence numbers, the TCPConnection makes sure that 
    // at least one segment is sent in reply, to reflect an update in the ackno and window size
    if (_sender.segments_out().size() == 0 && seg.length_in_sequence_space() > 0) {
        _sender.fill_window();

        if (_sender.segments_out().size() == 0) {
            _sender.send_empty_segment();
        }
    }

    if (_receiver.ackno().has_value() 
        && (seg.length_in_sequence_space() == 0)
        && (header.seqno == _receiver.ackno().value() - 1)){
        _sender.send_empty_segment();
    }

    _clear_sender_buffer();

    if (_receiver.stream_out().input_ended() && _sender.stream_in().input_ended()) {
        if (_linger_after_streams_finish) {
            _linger_after_streams_finish = false;
            _time_last_recv = 0;
        } else {
            _kill_connection(true);
        }
    }
}

bool TCPConnection::active() const {
    return _active;
}

size_t TCPConnection::write(const string &data) {
    size_t bytes_written = _sender.stream_in().write(data);
    _sender.fill_window();
    _clear_sender_buffer();
    return bytes_written;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _time_last_recv += ms_since_last_tick;

    if (_sender.consecutive_retransmissions() > _cfg.MAX_RETX_ATTEMPTS) {
        _send_rst_empty_segment();
        _kill_connection(false);
        return;
    }

    _sender.tick(ms_since_last_tick);

    if (_kill_condition_check()){
        if (_linger_after_streams_finish) {
            if (_time_last_recv >= 10 * _cfg.rt_timeout) {
                _kill_connection(true);
            }
        } else {
            _kill_connection(true);
        }
    }

    // if (_sender.segments_out().size() > 0) {
    //     _clear_sender_buffer();
    // }

    _clear_sender_buffer();
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    _clear_sender_buffer();

    if (_linger_after_streams_finish) {
        _linger_after_streams_finish = false;
        _time_last_recv = 0;
    } else {
        _kill_connection(true);
    }
}

void TCPConnection::connect() {
    _sender.fill_window();
    _clear_sender_buffer();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            _send_rst_empty_segment();
            _kill_connection(false);
            _clear_sender_buffer();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
