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

class byte_array_private {
private:
	friend class byte_array;
	size_type _alloc_size;
    size_type _size;
    boost::shared_array<uint8> _elems;
    boost::recursive_mutex _mutex;
	byte_array_private()
	: _alloc_size(0)
	, _size(0) {

	}
};

byte_array::reference byte_array::operator[](size_type i) {
    BOOST_ASSERT(0 > i || i >= _pri->_size);
    return _pri->_elems[i];
}

byte_array::const_reference byte_array::operator[](size_type i) const {
    BOOST_ASSERT(0 > i || i >= _pri->_size);
    return _pri->_elems[i];
}

byte_array::reference byte_array::at(size_type i) {
    range_check(i);
    return _pri->_elems[i];
}

byte_array::const_reference byte_array::at(size_type i) const {
    range_check(i);
    return _pri->_elems[i];
}

byte_array::reference byte_array::front() {
    BOOST_ASSERT(_pri->_elems.get());
    return _pri->_elems[0];
}

byte_array::const_reference byte_array::front() const {
    BOOST_ASSERT(_pri->_elems.get());
    return _pri->_elems[0];
}

byte_array::reference byte_array::back() {
    BOOST_ASSERT(_pri->_elems.get());
    return _pri->_elems[_pri->_size - 1];
}

byte_array::const_reference byte_array::back() const {
    BOOST_ASSERT(_pri->_elems.get());
    return _pri->_elems[_pri->_size - 1];
}

const uint8* byte_array::data() const {
    return _pri->_elems.get();
}

uint8* byte_array::data() {
    return _pri->_elems.get();
}

void byte_array::range_check(size_type i) {
    BOOST_ASSERT(0 < i || i <= size());
}

void byte_array::range_check(size_type i) const {
    BOOST_ASSERT(0 < i || i <= size());
}

size_type byte_array::size() {
    return _pri->_size;
}

size_type byte_array::size() const {
    return _pri->_size;
}

bool byte_array::empty() const {
    return _pri->_size != 0;
}

void byte_array::clear() {
    _pri->_size = 0;
}

byte_array::byte_array()
	: _pri(new byte_array_private()) {
    
}

byte_array::byte_array(const uint8 *data, size_type offset, size_type size)
	: _pri(new byte_array_private()) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_alloc_size = size;
    _pri->_size = size;
    uint8 *temp = new uint8[size];
    memcpy(temp, data + offset, size);
    _pri->_elems.reset(temp);
}

byte_array::byte_array(const byte_array &rhs)
	: _pri(new byte_array_private()) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_alloc_size = rhs._pri->_alloc_size;
    _pri->_size = rhs._pri->_size;
    _pri->_elems = rhs._pri->_elems;
}

byte_array::~byte_array() {
	delete _pri;
}

byte_array& byte_array::operator=(const byte_array &rhs) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    _pri->_alloc_size = rhs._pri->_alloc_size;
    _pri->_size = rhs._pri->_size;
    _pri->_elems = rhs._pri->_elems;
    return *this;
}

void byte_array::push_back(const uint8 *data, size_type offset, size_type length) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (_pri->_alloc_size == 0) {
        _pri->_alloc_size = length;
        uint8 *temp = new uint8[length];
        memcpy(temp, data + offset, length);
        _pri->_elems.reset(temp);
        _pri->_size = length;
    } else if ((_pri->_size + length) > _pri->_alloc_size) {
        _pri->_alloc_size += length;
        _pri->_alloc_size *= 2;
        uint8 *temp = new uint8[_pri->_alloc_size];
        memcpy(temp, _pri->_elems.get(), _pri->_size);
        memcpy(temp + _pri->_size, data + offset, length);
        _pri->_size += length;
        _pri->_elems.reset(temp);
    } else {
        if (_pri->_elems.unique()) {
            memcpy(_pri->_elems.get() + _pri->_size, data, length);
            _pri->_size += length;
        } else {
            uint8 *temp = new uint8[_pri->_alloc_size];
            memcpy(temp, _pri->_elems.get(), _pri->_size);
            memcpy(temp + _pri->_size, data, length);
            _pri->_elems.reset(temp);
            _pri->_size += length;
        }
    }
}

void byte_array::push_back(uint8 elem) {
    push_back(&elem, 0, 1);
}

void byte_array::push_back(const byte_array &other) {
    push_back(other.data(), 0, other.size());
}

void byte_array::resize(size_type size) {
    boost::recursive_mutex::scoped_lock scoped_lock(_pri->_mutex);
    if (size > _pri->_alloc_size) {
        _pri->_alloc_size = size;
        uint8 *temp = new uint8[_pri->_alloc_size];
        memcpy(temp, _pri->_elems.get(), _pri->_size);
        memset(temp + _pri->_size, 0, _pri->_alloc_size - _pri->_size);
        _pri->_elems.reset(temp);
        _pri->_size = size;
    } else if (size > _pri->_size) {
        memset(_pri->_elems.get() + _pri->_size, 0, size - _pri->_size);
        _pri->_size = size;
    } else {
        _pri->_size = size;
    }
}

}

