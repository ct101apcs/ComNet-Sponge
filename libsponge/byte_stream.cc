#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

// template <typename... Targs>
// void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t ByteStream::write(const string &data) {
    size_t number_bytes_written = 0;
    for (char ch : data){
        if (buffer.size() >= _capacity){
            break;
        }

        buffer.push(ch);
        number_bytes_written++;
    }
    _bytes_written+=number_bytes_written;
    return number_bytes_written;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    std::queue<char> tmp = buffer; 
    string peeked_string;
    size_t count = 0;
    while (count < len && !tmp.empty()) {
        peeked_string += tmp.front();
        tmp.pop();
        count++;
    }
    return peeked_string;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t count = 0;
    size_t actual_bytes_popped = min(len, buffer.size());
    
    while (count < actual_bytes_popped && !buffer.empty()) { 
        buffer.pop();
        count++;
    }
    _bytes_read += actual_bytes_popped;
}


//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string read_string = peek_output(len);
    size_t count = 0;
    size_t actual_bytes_popped = min(len, buffer.size());
    while (count < actual_bytes_popped)
    {
        read_string+=buffer.front();
        buffer.pop();
        count++;
    }
    _bytes_read += actual_bytes_popped;
    return read_string;
}

void ByteStream::end_input() {
    _input_ended = true;
}

bool ByteStream::input_ended() const {
    return _input_ended;
}

size_t ByteStream::buffer_size() const {
    return buffer.size();
}

bool ByteStream::buffer_empty() const {
    return buffer.empty();
}

bool ByteStream::eof() const {
    return _input_ended && buffer_empty();
}

size_t ByteStream::bytes_written() const {
    return _bytes_written;
}

size_t ByteStream::bytes_read() const {
    return _bytes_read;
}

size_t ByteStream::remaining_capacity() const { 
    return _capacity - buffer.size();
}
