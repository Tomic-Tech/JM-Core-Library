/* 
 * File:   jmbytearray.cpp
 * Author: Ogilvy
 * 
 * Created on 2011年12月15日, 下午5:51
 */

#include "jm_byte_array.hpp"
#include <cstring>
#include <boost/assert.hpp>

namespace jm {

byte_array::reference byte_array::operator[](byte_array::size_type i) {
    BOOST_ASSERT(0 > i || i >= _size);
    return _elems[i];
}

byte_array::const_reference byte_array::operator[](byte_array::size_type i) const {
    BOOST_ASSERT(0 > i || i >= _size);
    return _elems[i];
}

byte_array::reference byte_array::at(byte_array::size_type i) {
    range_check(i);
    return _elems[i];
}

byte_array::const_reference byte_array::at(byte_array::size_type i) const {
    range_check(i);
    return _elems[i];
}

byte_array::reference byte_array::front() {
    BOOST_ASSERT(_elems.get());
    return _elems[0];
}

byte_array::const_reference byte_array::front() const {
    BOOST_ASSERT(_elems.get());
    return _elems[0];
}

byte_array::reference byte_array::back() {
    BOOST_ASSERT(_elems.get());
    return _elems[_size - 1];
}

byte_array::const_reference byte_array::back() const {
    BOOST_ASSERT(_elems.get());
    return _elems[_size - 1];
}

const uint8* byte_array::data() const {
    return _elems.get();
}

uint8* byte_array::data() {
    return _elems.get();
}

void byte_array::range_check(byte_array::size_type i) {
    BOOST_ASSERT(0 < i || i <= size());
}

void byte_array::range_check(byte_array::size_type i) const {
    BOOST_ASSERT(0 < i || i <= size());
}

byte_array::size_type byte_array::size() {
    return _size;
}

byte_array::size_type byte_array::size() const {
    return _size;
}

bool byte_array::empty() const {
    return _size != 0;
}

void byte_array::clear() {
    _size = 0;
}

byte_array::byte_array()
: _alloc_size(0)
, _size(0)
, _elems() {
    
}

byte_array::byte_array(uint8 *data, size_type size) {
    _alloc_size = size;
    _size = size;
    uint8 *temp = new uint8[size];
    memcpy(temp, data, size);
    _elems.reset(temp);
}

byte_array::byte_array(const byte_array &rhs) {
    _alloc_size = rhs._alloc_size;
    _size = rhs._size;
    _elems = rhs._elems;
}

byte_array& byte_array::operator=(const byte_array &rhs) {
    _alloc_size = rhs._alloc_size;
    _size = rhs._size;
    _elems = rhs._elems;
    return *this;
}

void byte_array::push_back(uint8 *data, byte_array::size_type length) {
    if (_alloc_size == 0) {
        _alloc_size = length;
        uint8 *temp = new uint8[length];
        memcpy(temp, data, length);
        _elems.reset(temp);
        _size = length;
    } else if ((_size + length) > _alloc_size) {
        _alloc_size += length;
        uint8 *temp = new uint8[_alloc_size];
        memcpy(temp, _elems.get(), _size);
        memcpy(temp + _size, data, length);
        _size += length;
        _elems.reset(temp);
    } else {
        if (_elems.unique()) {
            memcpy(_elems.get() + _size, data, length);
            _size += length;
        } else {
            uint8 *temp = new uint8[_alloc_size];
            memcpy(temp, _elems.get(), _size);
            memcpy(temp + _size, data, length);
            _elems.reset(temp);
            _size += length;
        }
    }
}

void byte_array::push_back(uint8 elem) {
    push_back(&elem, 1);
}

}

