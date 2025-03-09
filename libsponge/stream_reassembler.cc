#include "stream_reassembler.hh"

using namespace std;

void StreamReassembler::push_substring(const string& data, const size_t index, const bool eof) {
    if (eof) {
        _eof_index = index + data.size();
    }

    // Truncate front if overlapping with assembled data
    size_t start_data = index < _first_unassembled ? _first_unassembled - index : 0;
    
    if (start_data >= data.size()) {
        if (empty() && _first_unassembled >= _eof_index) {
            _output.end_input();
        }
        return;
    }

    string write_data = data.substr(start_data);
    size_t adjusted_index = index + start_data;
    size_t written;

    if (adjusted_index == _first_unassembled) {
        write_data = truncate_data(write_data, adjusted_index);
        written = _output.write(write_data);
        _first_unassembled += written;

        while (true)
        {
            try_assemble();
            auto it = _unassembled_segments.find(_first_unassembled);
            if (it == _unassembled_segments.end()) {
                break;
            }
            string& seg_data = it->second;
            written = _output.write(seg_data);
            _first_unassembled += written;
            _unassembled_segments.erase(it);
            
        }
        
    } else if (adjusted_index > _first_unassembled) {
        insert_segment(write_data, adjusted_index);
    }

    if (empty() && _first_unassembled == _eof_index) {
        _output.end_input();
    }
}

void StreamReassembler::insert_segment(const string& data, size_t index) {
    string tmp_data = data;
    size_t tmp_index = index;
    size_t tmp_end = tmp_index + tmp_data.size() - 1;

    for (auto it = _unassembled_segments.begin(); it != _unassembled_segments.end();) {
        size_t seg_start = it->first;
        string& seg_data = it->second;
        size_t seg_end = seg_start + seg_data.size() - 1;

        // No overlap, segment before
        if (seg_end < tmp_index) {
            ++it;
            continue;
        }
        // No overlap, segment after
        if (seg_start > tmp_end) {
            break;
        }

        // Overlap detected
        if (seg_start <= tmp_end && tmp_index <= seg_end) {
            // New data completely covers segment
            if (tmp_index <= seg_start && tmp_end >= seg_end) {
                it = _unassembled_segments.erase(it);
                continue;
            }
            // Segment completely covers new data
            if (seg_start <= tmp_index && seg_end >= tmp_end) {
                tmp_data.clear();
                ++it;
                continue;
            }
            // Partial overlap: new data starts before segment
            if (tmp_index < seg_start) {
                size_t extra_len = seg_end - tmp_end;
                if (extra_len > 0) {
                    tmp_data += seg_data.substr(seg_data.size() - extra_len);
                }
                it = _unassembled_segments.erase(it);
                tmp_end = tmp_index + tmp_data.size() - 1;
                continue;
            }
            // Partial overlap: segment starts before new data
            if (seg_start < tmp_index) {
                size_t prefix_len = tmp_index - seg_start;
                tmp_data.insert(0, seg_data.substr(0, prefix_len));
                tmp_index = seg_start;
                tmp_end = tmp_index + tmp_data.size() - 1;
                it = _unassembled_segments.erase(it);
                continue;
            }
        }else {
        ++it;}
    }

    if (!tmp_data.empty()) {
        string truncated = truncate_data(tmp_data, tmp_index);
        if (!truncated.empty()) {
            _unassembled_segments.insert({tmp_index, truncated});
        }
    }
}

void StreamReassembler::try_assemble() {
    for (auto it = _unassembled_segments.begin(); it != _unassembled_segments.end();) {
        size_t seg_index = it->first;
        string seg_data = it->second;
        if (seg_index < _first_unassembled) {
            it = _unassembled_segments.erase(it);
            if (seg_index + seg_data.size() > _first_unassembled) {
                seg_data = seg_data.substr(_first_unassembled - seg_index);
                seg_index = _first_unassembled;
                insert_segment(seg_data, seg_index);
            }
        }
        else {
            ++it;
        }
    }
}

string StreamReassembler::truncate_data(const string& data, size_t index) {
    size_t first_unacceptable = _output.bytes_read() + _capacity;
    size_t available_space = _capacity - _output.buffer_size() - unassembled_bytes();
    size_t max_len = min(data.size(), first_unacceptable - index);
    max_len = min(max_len, available_space);
    return data.substr(0, max_len);
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t total = 0;
    for (const auto& segment : _unassembled_segments) {
        total += segment.second.size();
    }
    return total;
}

bool StreamReassembler::empty() const {
    return _unassembled_segments.empty();
}
