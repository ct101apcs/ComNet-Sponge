#include "tcp_sender.hh"
#include "tcp_config.hh"

#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

using namespace std;

void Timer::start(unsigned int timeout) {
    _active = true;
    _expired = false;
    _current_time = 0;
    _timeout = timeout;
}

void Timer::update(unsigned int time_elapsed) {
    if (!_active) {
        return; 
    }
    _current_time += time_elapsed;
    if (_current_time >= _timeout) {
        _expired = true;
    }
}

void Timer::reset() {
    _active = false;
    _expired = false;
    _current_time = 0;
    _timeout = 0;
}

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)

TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _timer()
    , _next_seqno(0)
    , _abs_ackno(0)
    , _window_size(1)
    , _outstanding_segments()
    , _bytes_in_flight(0)
    , _elapsed_time(0)
    , _time_expired(0)
    , _retransmission_timeout(retx_timeout)  
    , _consecutive_retransmissions(0)                   
{}

uint64_t TCPSender::bytes_in_flight() const {
    uint64_t res = 0;
    std::queue<std::pair<uint64_t, TCPSegment>> tmp_queue = _outstanding_segments;
    while (!tmp_queue.empty()) {
        res += tmp_queue.front().second.length_in_sequence_space();
        tmp_queue.pop();
    }
    return res;
}

void TCPSender::fill_window() {
    while (true)
    {
        uint64_t effective_window_size = (_window_size == 0) ? 1 : _window_size;
        // Compute available space from next_seqno to (ackno + window_size)
        uint64_t available_space = 
        (_abs_ackno + effective_window_size - _next_seqno > 0) ?
        (_abs_ackno + effective_window_size - _next_seqno) : 0;

        if (available_space == 0)
            break;

        // Conditional check to send a segment (SYN, data availabl or FIN)
        bool syn_needed = (_next_seqno == 0);
        bool data_available = !_stream.buffer_empty();
        bool fin_needed = (_stream.eof() && _stream.buffer_empty()) &&
                            (_next_seqno < _stream.bytes_written() + 2);
        
        if (!syn_needed && !data_available && !fin_needed)
            break;
        
        // Create a new segment
        TCPSegment seg;

        // Set the sequence number
        seg.header().seqno = wrap(_next_seqno, _isn);

        // Set the SYN flag if connection is not established
        if (syn_needed)
            seg.header().syn = true;

        size_t max_payload_size = min(static_cast<size_t>(TCPConfig::MAX_PAYLOAD_SIZE), 
                                    static_cast<size_t>(available_space - (seg.header().syn ? 1 : 0)));
        
        std::string payload = _stream.read(max_payload_size);
        seg.payload() = Buffer(std::move(payload));

        // If the stream is EOF and the segment is empty, set the FIN flag
        bool fin_space = (available_space - (seg.header().syn ? 1 : 0) - seg.payload().size() > 0);

        if (_stream.eof() && _stream.buffer_empty() && fin_space)
            seg.header().fin = true;
        
        size_t seg_size = seg.length_in_sequence_space();

        // Verify the segment size
        if (seg_size == 0 || seg_size > available_space)
            break;
        
        _segments_out.push(seg);
        _outstanding_segments.push({_next_seqno, seg});
        _bytes_in_flight += seg_size;
        _next_seqno += seg_size;

        if (!_timer.active()) {
            _timer.start(_retransmission_timeout);
        }
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    // Convert the ackno to an absolute sequence number
    uint64_t abs_ackno = unwrap(ackno, _isn, _abs_ackno + 1);

    // Update the window size
    _window_size = window_size;

    // Check if the ackno is valid
    if (abs_ackno > _next_seqno) {
        return;
    }

    // Update the absolute ackno
    _abs_ackno = abs_ackno;

    // Remove acknowledged segments from the outstanding segments queue
    while (!_outstanding_segments.empty() && 
            _outstanding_segments.front().first +
            _outstanding_segments.front().second.length_in_sequence_space() <= abs_ackno) {

        _bytes_in_flight -= _outstanding_segments.front().second.length_in_sequence_space();
        _outstanding_segments.pop();

        _consecutive_retransmissions = 0;
        _retransmission_timeout = _initial_retransmission_timeout;

        // Restart the timer if there are still outstanding segments
        if (!_outstanding_segments.empty()) {
            _timer.start(_retransmission_timeout);
        } else {
            // Stop the timer if there are no outstanding segments
            _timer.reset();
        }
    }

    fill_window();
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick){
    if (_timer.active()) {
        _timer.update(ms_since_last_tick);
        if (_timer.expired() && !_outstanding_segments.empty()) {
            TCPSegment seg = _outstanding_segments.front().second;
            seg.header().seqno = wrap(_outstanding_segments.front().first, _isn);
            _segments_out.push(seg);
            if (_window_size > 0) {
                _consecutive_retransmissions++;
                _retransmission_timeout *= 2;
            }
            _timer.start(_retransmission_timeout);
        }
    }
}

unsigned int TCPSender::consecutive_retransmissions() const {
    return _consecutive_retransmissions;
}

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    seg.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(seg);
}

void TCPSender::send_empty_segment(bool syn, bool fin, bool rst) {
    TCPSegment seg;
    seg.header().seqno = wrap(_next_seqno, _isn);
    seg.header().syn = syn;
    seg.header().fin = fin;
    seg.header().rst = rst;
    _segments_out.push(seg);
    _outstanding_segments.push({_next_seqno, seg});
    _next_seqno += seg.length_in_sequence_space();
}
