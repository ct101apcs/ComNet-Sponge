#include "stream_reassembler.hh"

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.

// void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
//     // Discard data that is entirely before _first_unassembled
//     if (index + data.size() <= _first_unassembled) {
//         if (eof) {
//             _eof_received = true;
//             _eof_index = index + data.size();
//             if (_first_unassembled >= _eof_index) {
//                 _output.end_input();
//             }
//         }
//         return;
//     }

//     // Discard data that starts beyond the capacity window
//     uint64_t first_unread = _output.bytes_read();
//     uint64_t window_end = first_unread + _capacity;
//     if (index >= window_end) {
//         return;
//     }

//     // Trim the data to fit within the capacity window
//     std::string trimmed_data = data;
//     uint64_t start_index = index;
//     if (index + trimmed_data.size() > window_end) {
//         trimmed_data = trimmed_data.substr(0, window_end - index);
//     }

//     // Handle EOF
//     if (eof) {
//         _eof_received = true;
//         _eof_index = index + data.size();
//     }

//     // Trim data that starts before _first_unassembled
//     if (index < _first_unassembled) {
//         size_t offset = _first_unassembled - index;
//         if (offset >= trimmed_data.size()) {
//             trimmed_data.clear();
//         } else {
//             trimmed_data = trimmed_data.substr(offset);
//             start_index = _first_unassembled;
//         }
//     }

//     // If no data remains after trimming, check EOF and return
//     if (trimmed_data.empty()) {
//         if (_eof_received && _first_unassembled >= _eof_index) {
//             _output.end_input();
//         }
//         return;
//     }

//     // Merge overlapping segments
//     auto it = _unassembled.lower_bound(start_index);
//     while (it != _unassembled.end() && it->first < start_index + trimmed_data.size()) {
//         uint64_t existing_start = it->first;
//         std::string &existing_data = it->second;

//         // Merge overlapping segments
//         if (existing_start < start_index + trimmed_data.size()) {
//             size_t overlap_start = std::max(start_index, existing_start);
//             size_t overlap_end = std::min(start_index + trimmed_data.size(), existing_start + existing_data.size());
//             size_t overlap_length = overlap_end - overlap_start;

//             // Replace overlapping part with new data
//             if (overlap_start == start_index && overlap_length >= trimmed_data.size()) {
//                 // New data is entirely contained within existing data
//                 trimmed_data.clear();
//                 break;
//             } else {
//                 // Merge the two segments
//                 size_t new_data_start = overlap_start - start_index;
//                 size_t existing_data_start = overlap_start - existing_start;
//                 trimmed_data.replace(new_data_start, overlap_length, existing_data.substr(existing_data_start, overlap_length));
//             }
//         }

//         // Remove the existing segment
//         it = _unassembled.erase(it);
//     }

//     // Store the new or merged segment
//     if (!trimmed_data.empty()) {
//         _unassembled[start_index] = trimmed_data;
//     }

//     // Write contiguous data to the output stream
//     while (!_unassembled.empty()) {
//         auto itr = _unassembled.begin();
//         if (itr->first > _first_unassembled) {
//             break; 
//         }

//         const std::string &segment = itr->second;
//         uint64_t segment_start = itr->first;
//         uint64_t segment_end = segment_start + segment.size();

//         // Calculate bytes to write from this segment
//         size_t bytes_available = segment_end - _first_unassembled;
//         if (bytes_available == 0) {
//             _unassembled.erase(itr);
//             continue;
//         }

//         // Ensure capacity isn't exceeded
//         size_t remaining_capacity = _capacity - (_first_unassembled - _output.bytes_read());
//         size_t bytes_to_write = std::min(bytes_available, remaining_capacity);

//         // Write to output and update state
//         std::string new_data = segment.substr(_first_unassembled - segment_start, bytes_to_write);
//         _output.write(new_data);
//         _first_unassembled += bytes_to_write;

//         // Remove or trim the segment
//         if (_first_unassembled >= segment_end) {
//             _unassembled.erase(itr);
//         } else {
//             // Reinsert the remaining part with the new start index
//             uint64_t new_start = _first_unassembled;
//             std::string remaining = segment.substr(_first_unassembled - segment_start);
//             _unassembled.erase(itr); // Remove the old entry
//             _unassembled.emplace(new_start, std::move(remaining));
//             break; // Exit after updating to avoid infinite loops
//         }

//         // Check EOF
//         if (_eof_received && _first_unassembled >= _eof_index) {
//             _output.end_input();
//             break;
//         }
//     }
// }

// size_t StreamReassembler::unassembled_bytes() const {
//         size_t count = 0;
//         uint64_t last_end = _first_unassembled;

//         for (const auto &pair : _unassembled) {
//             // Skip if completely before first_unassembled
//             if (pair.first + pair.second.size() <= _first_unassembled) {
//                 continue;
//             }

//             // Calculate start and end of this segment
//             uint64_t start = std::max(pair.first, _first_unassembled);
//             uint64_t end = pair.first + pair.second.size();

//             if (start >= last_end) {
//                 count += end - start;
//             } else if (end > last_end) {
//                 count += end - last_end;
//             }
//             last_end = std::max(last_end, end);
//         }
//         return count;
//     }

// bool StreamReassembler::empty() const {
//         return _unassembled.empty();
// }

// Receive a substring and write any newly contiguous bytes into the stream
// void StreamReassembler::push_substring(const std::string &data, const size_t index, const bool eof) {
//     // Handle empty data case
//     if (data.empty()) {
//         if (eof) {
//             _eof_received = true;
//             _eof_index = index;
            
//             // If we're already at or past the EOF index, we're done
//             if (_next_index >= _eof_index) {
//                 _output.end_input();
//             }
//         }
//         return;
//     }

//     // Calculate the actual range of the data
//     size_t data_start = index;
//     size_t data_end = index + data.length();
    
//     // If the data is completely before our next expected byte, ignore it
//     if (data_end <= _next_index) {
//         if (eof) {
//             _eof_received = true;
//             _eof_index = data_end;
            
//             // If we're already at or past the EOF index, we're done
//             if (_next_index >= _eof_index) {
//                 _output.end_input();
//             }
//         }
//         return;
//     }
    
//     // If the data starts before our next expected byte, trim it
//     std::string trimmed_data = data;
//     size_t trimmed_start = data_start;
//     if (data_start < _next_index) {
//         size_t offset = _next_index - data_start;
//         trimmed_data = data.substr(offset);
//         trimmed_start = _next_index;
//     }
    
//     // Calculate how much unassembled data we can store (considering capacity)
//     size_t bytes_in_output = _output.buffer_size();
//     size_t available_capacity = _capacity - bytes_in_output;
    
//     // Check if this segment would exceed our capacity
//     size_t current_unassembled = unassembled_bytes();
//     if (current_unassembled + trimmed_data.length() > available_capacity) {
//         // Trim the data to fit within capacity
//         if (available_capacity > current_unassembled) {
//             size_t can_store = available_capacity - current_unassembled;
//             trimmed_data = trimmed_data.substr(0, can_store);
//         } else {
//             // No space to store any new data
//             trimmed_data = "";
//         }
//     }
    
//     // If we have anything to store after all the trimming
//     if (!trimmed_data.empty()) {
//         // Store the segment
//         _unassembled_segments[trimmed_start] = trimmed_data;
        
//         // Try to merge overlapping segments to save memory
//         merge_overlapping_segments();
        
//         // Write any contiguous data to the output
//         write_to_output();
//     }
    
//     // Handle EOF flag
//     if (eof) {
//         _eof_received = true;
//         _eof_index = data_end;
        
//         // Check if we can mark the output stream as done
//         if (_next_index >= _eof_index) {
//             _output.end_input();
//         }
//     }
// }
void StreamReassembler::push_substring(const std::string &data, const size_t index, const bool eof) {
    // Handle empty data case
    if (data.empty()) {
        if (eof) {
            _eof_received = true;
            _eof_index = index;
            
            // If we're already at or past the EOF index, we're done
            if (_next_index >= _eof_index) {
                _output.end_input();
            }
        }
        return;
    }

    // Calculate the actual range of the data
    size_t data_start = index;
    size_t data_end = index + data.length();
    
    // If the data is completely before our next expected byte, ignore it
    if (data_end <= _next_index) {
        if (eof) {
            _eof_received = true;
            _eof_index = data_end;
            
            // If we're already at or past the EOF index, we're done
            if (_next_index >= _eof_index) {
                _output.end_input();
            }
        }
        return;
    }
    
    // If the data starts before our next expected byte, trim it
    std::string trimmed_data = data;
    size_t trimmed_start = data_start;
    if (data_start < _next_index) {
        size_t offset = _next_index - data_start;
        trimmed_data = data.substr(offset);
        trimmed_start = _next_index;
    }
    
    // Calculate how much unassembled data we can store (considering capacity)
    size_t bytes_in_output = _output.buffer_size();
    size_t available_capacity = _capacity - bytes_in_output;
    
    // Check if this segment would exceed our capacity
    size_t current_unassembled = unassembled_bytes();
    if (current_unassembled + trimmed_data.length() > available_capacity) {
        // Trim the data to fit within capacity
        if (available_capacity > current_unassembled) {
            size_t can_store = available_capacity - current_unassembled;
            trimmed_data = trimmed_data.substr(0, can_store);
        } else {
            // No space to store any new data
            trimmed_data = "";
        }
    }
    
    // If we have anything to store after all the trimming
    if (!trimmed_data.empty()) {
        // Check if this data can be written directly to the output
        // instead of storing it in the unassembled segments map
        if (trimmed_start == _next_index) {
            // Write directly to output
            size_t bytes_written = _output.write(trimmed_data);
            _next_index += bytes_written;
            
            // If we couldn't write all data, store the remainder
            if (bytes_written < trimmed_data.length()) {
                std::string remaining = trimmed_data.substr(bytes_written);
                _unassembled_segments[_next_index] = remaining;
            }
        } else {
            // Store the segment
            _unassembled_segments[trimmed_start] = trimmed_data;
        }
        
        // Try to merge overlapping segments to save memory
        merge_overlapping_segments();
        
        // Write any contiguous data to the output
        write_to_output();
    }
    
    // Handle EOF flag
    if (eof) {
        _eof_received = true;
        _eof_index = data_end;
        
        // Check if we can mark the output stream as done
        if (_next_index >= _eof_index) {
            _output.end_input();
        }
    }
}

// Helper method to merge overlapping segments
void StreamReassembler::merge_overlapping_segments() {
    bool merged;
    
    do {
        merged = false;
        
        auto it1 = _unassembled_segments.begin();
        while (it1 != _unassembled_segments.end()) {
            auto it2 = std::next(it1);
            
            while (it2 != _unassembled_segments.end()) {
                size_t start1 = it1->first;
                size_t end1 = start1 + it1->second.length();
                size_t start2 = it2->first;
                size_t end2 = start2 + it2->second.length();
                
                // Check for overlap or adjacency
                if (end1 >= start2 && start1 <= end2) {
                    size_t new_start = std::min(start1, start2);
                    // size_t new_end = std::max(end1, end2);

                    // Create the merged string
                    std::string merged_str;

                    if (start1 <= start2) {
                        // First segment starts first (or at the same position)
                        merged_str = it1->second;

                        // Add non-overlapping part of second segment if it extends beyond first
                        if (end2 > end1) {
                            size_t overlap = (end1 > start2) ? end1 - start2 : 0;
                            merged_str += it2->second.substr(overlap);
                        }
                    } else {
                        // Second segment starts first
                        merged_str = it2->second.substr(0, start1 - start2);

                        // Add first segment
                        merged_str += it1->second;

                        // Add remainder of second segment if it extends beyond first
                        if (end2 > end1) {
                            size_t extension = end2 - end1;
                            merged_str += it2->second.substr(it2->second.length() - extension);
                        }
                    }

                    // Remove the original segments
                    _unassembled_segments.erase(it2);
                    _unassembled_segments.erase(it1);
                    
                    // Add the merged segment
                    _unassembled_segments[new_start] = merged_str;
                    
                    merged = true;
                    break;
                }
                
                ++it2;
            }
            
            if (merged) {
                break;  // Start over since we modified the map
            }
            
            ++it1;
        }
    } while (merged && _unassembled_segments.size() > 1);
}

// Helper method to write contiguous data to the output
void StreamReassembler::write_to_output() {
    // Loop until we can't write any more contiguous data
    while (!_unassembled_segments.empty()) {
        auto first_segment = _unassembled_segments.begin();
        
        // If this segment starts at our next expected index
        if (first_segment->first == _next_index) {
            // Write it to the output
            const std::string &data = first_segment->second;
            size_t bytes_written = _output.write(data);
            
            // Update our next expected index
            _next_index += bytes_written;
            
            // Remove the segment (it's fully processed if bytes_written == data.length())
            if (bytes_written == data.length()) {
                _unassembled_segments.erase(first_segment);
            } else {
                // Couldn't write the whole segment, store the remainder
                std::string remaining = data.substr(bytes_written);
                _unassembled_segments.erase(first_segment);
                _unassembled_segments[_next_index] = remaining;
                break;  // Output buffer is full, can't write any more
            }
        } else {
            // The first segment doesn't start at the next expected index
            break;
        }
    }
    
    // Check if we've reached the EOF
    if (_eof_received && _next_index >= _eof_index) {
        _output.end_input();
    }
}

// Access the reassembled ByteStream
ByteStream &StreamReassembler::stream_out() {
    return _output;
}

// The number of bytes in the substrings stored but not yet reassembled
size_t StreamReassembler::unassembled_bytes() const {
    size_t total = 0;
    for (const auto &segment : _unassembled_segments) {
        total += segment.second.length();
    }
    return total;
}

// Is the internal state empty (other than the output stream)?
bool StreamReassembler::empty() const {
    return _unassembled_segments.empty();
}
