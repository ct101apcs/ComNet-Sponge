#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"
#include "stream_reassembler.hh"
#include <map>
#include <string>
#include <cstdint>
#include <limits>

// //! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
// //! possibly overlapping) into an in-order byte stream.
// class StreamReassembler {
//   private:
//     // Your code here -- add private members as necessary.

//     ByteStream _output;  //!< The reassembled in-order byte stream
//     size_t _capacity;    //!< The maximum number of bytes
    
//   public:
//     //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
//     //! \note This capacity limits both the bytes that have been reassembled,
//     //! and those that have not yet been reassembled.
//     StreamReassembler(const size_t capacity);

//     //! \brief Receive a substring and write any newly contiguous bytes into the stream.
//     //!
//     //! The StreamReassembler will stay within the memory limits of the `capacity`.
//     //! Bytes that would exceed the capacity are silently discarded.
//     //!
//     //! \param data the substring
//     //! \param index indicates the index (place in sequence) of the first byte in `data`
//     //! \param eof the last byte of `data` will be the last byte in the entire stream
//     void push_substring(const std::string &data, const uint64_t index, const bool eof);

//     //! \name Access the reassembled byte stream
//     //!@{
//     const ByteStream &stream_out() const { return _output; }
//     ByteStream &stream_out() { return _output; }
//     //!@}

//     //! The number of bytes in the substrings stored but not yet reassembled
//     //!
//     //! \note If the byte at a particular index has been pushed more than once, it
//     //! should only be counted once for the purpose of this function.
//     size_t unassembled_bytes() const;

//     //! \brief Is the internal state empty (other than the output stream)?
//     //! \returns `true` if no substrings are waiting to be assembled
//     bool empty() const;
// };

// ------------Reference--------------:
// The 'push_substring() function is based on the structure of CS144 course material.

class StreamReassembler {
private:
    ByteStream _output;                                     // The reassembled ByteStream
    size_t _capacity;                                       // The maximum memory usage allowed
    size_t _first_unassembled;                              // The index of the next byte to be written to the output
    std::map<size_t, std::string> _unassembled_segments;    // Map of index -> segment for unassembled data
    size_t _eof_index;                                      // The index after the last byte in the stream

    void insert_segment(const std::string &data, const size_t index);

    void try_assemble();
    
    std::string truncate_data(const std::string &data, const size_t index);
    
public:
    StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _first_unassembled(0), _unassembled_segments(), _eof_index(std::numeric_limits<size_t>::max()) {}

    void push_substring(const std::string &data, const size_t index, const bool eof);

    ByteStream &stream_out(){
        return _output;
    }

    size_t unassembled_bytes() const;

    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
