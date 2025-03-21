#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return WrappingInt32(isn + static_cast<uint32_t>(n));
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    // Compute the offset from ISN in 32-bit space
    uint32_t offset = n.raw_value() - isn.raw_value();
    // Convert offset to 64-bit to avoid overflow
    uint64_t offset_64 = static_cast<uint64_t>(offset);

    // Find the number of wraps (2^32 cycles) that gets us closest to checkpoint
    constexpr uint64_t WRAP_MOD = 1ULL << 32;           // 2^32
    uint64_t checkpoint_wraps = checkpoint / WRAP_MOD;  // Number of full wraps in checkpoint

    // Compute possible absolute sequence numbers around the checkpoint
    uint64_t base = checkpoint_wraps * WRAP_MOD + offset_64;
    uint64_t prev = (checkpoint_wraps > 0) ? (checkpoint_wraps - 1) * WRAP_MOD + offset_64 : offset_64;
    uint64_t next = (checkpoint_wraps + 1) * WRAP_MOD + offset_64;

    // Pick the one closest to checkpoint
    uint64_t distances[3] = {
        (base > checkpoint) ? base - checkpoint : checkpoint - base,
        (prev > checkpoint) ? prev - checkpoint : checkpoint - prev,
        (next > checkpoint) ? next - checkpoint : checkpoint - next
    };

    if (distances[0] <= distances[1] && distances[0] <= distances[2]) {
        return base;
    } else if (distances[1] <= distances[2]) {
        return prev;
    } else {
        return next;
    }
}