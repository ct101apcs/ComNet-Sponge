#ifndef SPONGE_LIBSPONGE_TCP_SENDER_HH
#define SPONGE_LIBSPONGE_TCP_SENDER_HH

#include "byte_stream.hh"
#include "tcp_config.hh"
#include "tcp_segment.hh"
#include "wrapping_integers.hh"

#include <functional>
#include <queue>

class Timer {
  public:
    void start(unsigned int timeout);
    void update(unsigned int time_elapsed);
    void reset();
    bool active() const { return _active; }
    bool expired() const { return _active && _expired; }

  private:
    bool _active{false};
    bool _expired{false};
    unsigned int _current_time{0};
    unsigned int _timeout{0};
};

//! \brief The "sender" part of a TCP implementation.

//! Accepts a ByteStream, divides it up into segments and sends the
//! segments, keeps track of which segments are still in-flight,
//! maintains the Retransmission Timer, and retransmits in-flight
//! segments if the retransmission timer expires.
class TCPSender {
  private:
    //! our initial sequence number, the number for our SYN.
    WrappingInt32 _isn;

    //! outbound queue of segments that the TCPSender wants sent
    std::queue<TCPSegment> _segments_out{};

    //! retransmission timer for the connection
    unsigned int _initial_retransmission_timeout;

    //! outgoing stream of bytes that have not yet been sent
    ByteStream _stream;

    // Timer state
    Timer _timer;
    
    //! the (absolute) sequence number for the next byte to be sent
    uint64_t _next_seqno{0};

    // Abs seqno of the last byte sent
    uint64_t _abs_ackno{0};

    // Advertised window size
    uint64_t _window_size{1};

    // Outstanding segments
    std::queue<std::pair<uint64_t, TCPSegment>> _outstanding_segments;

    // Total bytes in flight
    size_t _bytes_in_flight{0};

    // Total elapsed time
    size_t _elapsed_time{0};

    // Time expired
    size_t _time_expired{0};

    // Retransmission timeout
    size_t _retransmission_timeout{TCPConfig::TIMEOUT_DFLT};

    // Number of consecutive retransmissions
    unsigned int _consecutive_retransmissions{0};

  public:
  TCPSender(const size_t capacity, const uint16_t retx_timeout,
             const std::optional<WrappingInt32> fixed_isn = std::nullopt);

    //! \name "Input" interface for the writer
    //!@{
    ByteStream &stream_in() { return _stream; }
    const ByteStream &stream_in() const { return _stream; }
    //!@}

    //! \name Methods that can cause the TCPSender to send a segment
    //!@{

    //! \brief A new acknowledgment was received
    void ack_received(const WrappingInt32 ackno, const uint16_t window_size);

    //! \brief Generate an empty-payload segment (useful for creating empty ACK segments)
    void send_empty_segment();

    void send_empty_segment(bool syn, bool fin, bool rst);

    //! \brief create and send segments to fill as much of the window as possible
    void fill_window();

    //! \brief Notifies the TCPSender of the passage of time
    void tick(const size_t ms_since_last_tick);
    //!@}

    //! \name Accessors
    //!@{

    //! \brief How many sequence numbers are occupied by segments sent but not yet acknowledged?
    //! \note count is in "sequence space," i.e. SYN and FIN each count for one byte
    //! (see TCPSegment::length_in_sequence_space())
    size_t bytes_in_flight() const;

    //! \brief Number of consecutive retransmissions that have occurred in a row
    unsigned int consecutive_retransmissions() const;

    //! \brief TCPSegments that the TCPSender has enqueued for transmission.
    //! \note These must be dequeued and sent by the TCPConnection,
    //! which will need to fill in the fields that are set by the TCPReceiver
    //! (ackno and window size) before sending.
    std::queue<TCPSegment> &segments_out() { return _segments_out; }
    //!@}

    //! \name What is the next sequence number? (used for testing)
    //!@{

    //! \brief absolute seqno for the next byte to be sent
    uint64_t next_seqno_absolute() const { return _next_seqno; }

    //! \brief relative seqno for the next byte to be sent
    WrappingInt32 next_seqno() const { return wrap(_next_seqno, _isn); }
    //!@}
};

#endif  // SPONGE_LIBSPONGE_TCP_SENDER_HH
