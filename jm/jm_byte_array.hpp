/* 
 * File:   jmbytearray.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月15日, 下午5:51
 */

#ifndef JMBYTEARRAY_HPP
#define	JMBYTEARRAY_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_types.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace jm {

class byte_array {
public:
    // type definitions
    typedef uint8 value_type;
    typedef uint8* iterator;
    typedef const uint8* const_iterator;
    typedef uint8& reference;
    typedef const uint8& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    byte_array();
    byte_array(uint8 *data, size_type offset, size_type size);
    byte_array(const byte_array &rhs);

    //assignment with type conversion
    byte_array& operator =(const byte_array &rhs);

    void push_back(uint8 elem);
    void push_back(const uint8 *data, size_type offset, size_type length);
    void push_back(const byte_array &other);
    // iterator support
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    // operator[]
    reference operator[] (size_type i);
    const_reference operator[](size_type i) const;

    // at() with range check
    reference at(size_type i);
    const_reference at(size_type i) const;

    // front() and back()
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // direct access to data (read-only)
    const uint8* data() const;
    uint8* data();
    size_type size();
    size_type size() const;
    bool empty() const;
    void clear();
    void resize(size_type size);
private:
    // check range (may be private because it is static)
    void range_check(size_type i);
    void range_check(size_type i) const;
private:
    size_type _alloc_size;
    size_type _size;
    boost::shared_array<uint8> _elems;
    boost::recursive_mutex _mutex;
};

typedef boost::shared_ptr<byte_array> byte_array_ptr;

}

#endif	/* JMBYTEARRAY_HPP */

