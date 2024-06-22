#pragma once

#include <algorithm>
#include <memory>
#include <ostream>
#include <stdexcept>

template <typename T>
class Vector {
public:
    /**
     *  associated types
     */
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Vector() = default;


    Vector(size_type n, const_reference default_val) {
        _size = n;
        _capacity = n;
        _data = std::make_unique<value_type[]>(n);
        std::fill(begin(), end(), default_val);
    }


    Vector(std::initializer_list<value_type> l) {
        _size = l.size();
        _capacity = l.size();
        _data = std::make_unique<value_type[]>(l.size());
        std::copy(l.begin(), l.end(), begin());
    }

    Vector(const Vector& copy) : _size(copy._size), _capacity(copy._capacity) {
        _data = std::make_unique<value_type[]>(_capacity);
        std::copy(copy.cbegin(), copy.cend(), begin());
    }

    Vector(Vector&& move)  noexcept : _size(move._size), _capacity(move._capacity) {
        _data = std::move(move._data);
    }

    /**
     * copy assignment
     */
    Vector& operator=(const Vector& copy) {
        _capacity = copy._capacity;
        _size = copy.size();
        _data = std::make_unique<value_type[]>(copy._capacity);
        std::copy(copy.cbegin(), copy.cend(), begin());
        return *this;
    }


    /**
     * move assignment
     */
    Vector& operator=(Vector&& move) noexcept {
        _capacity = move._capacity;
        _size = move.size();
        _data = std::move(move._data);
        return *this;
    }

    size_type size() const noexcept { return _size; }

    size_type capacity() const noexcept { return _capacity; }

    void push_back(const_reference value) {
        size_type new_capacity = calculate_capacity(_size + 1);
        resize(new_capacity);
        _data[_size++] = value;
    }

    void push_back(T&& value) {
        size_type new_capacity = calculate_capacity(_size + 1);
        resize(new_capacity);
        _data[_size++] = std::move(value);
    }

    void pop_back() {
        _data[--_size] = value_type();
    }

    /**
     * with bounds check
     */
    reference at(const size_type pos) const {
        if(pos < _size){
            return _data[pos];
        }else{
            throw std::out_of_range{"Invalid position"};
        }
    }

    reference at(const size_type pos) {
        if(pos < _size){
            return _data[pos];
        }else{
            throw std::out_of_range{"Invalid position"};
        }
    }

    /**
     * without bounds check
     */
    const_reference operator[](size_type index) const {
        return _data[index];
    }

    reference operator[](const size_type index) {
        return _data[index];
    }


    iterator begin() {
        return _data.get();
    }

    const_iterator cbegin() const {
        return _data.get();
    }

    iterator end() {
        return _data.get() + _size;
    }

    const_iterator cend() const {
        return _data.get() + _size;
    }

    reverse_iterator rbegin() {
        return reverse_iterator{end()};
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator{cend()};
    }

    reverse_iterator rend() {
        return reverse_iterator{begin()};
    }

    const_reverse_iterator crend() {
        return const_reverse_iterator{cend()};
    }

    /**
     * stream a Vector to an output stream textually
     */
    friend std::ostream& operator<<(std::ostream& o, Vector<value_type> v) {
        o << "Size: " << v._size << ", Capacity: " << v._capacity << std::endl;
        for (size_type i = 0; i < v._size; ++i) {
            if (i > 0)
                o << ", ";
            o << v._data[i];
        }
        o << std::endl;
        return o;
    }

    ~Vector() = default;

private:

    size_type growth_factor = 2;
    size_type _size;
    size_type _capacity;

    std::unique_ptr<value_type[]> _data;

    size_type calculate_capacity(size_type new_size) {
        if(_capacity == 0)
            return new_size;
        return new_size > _capacity ? _capacity * growth_factor : _capacity;
    }

    void resize(size_type new_capacity) {
        if(_capacity != new_capacity){
            std::unique_ptr<value_type[]> new_data = std::make_unique<value_type[]>(new_capacity);
            std::move(begin(), end(), new_data.get());
            _data = std::move(new_data);
            _capacity = new_capacity;
        }
    }
};
