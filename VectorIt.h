#ifndef VECTORIT_H
#define VECTORIT_H

#include <vector>
#include <stdexcept>
#include <iterator>
#include <algorithm>

template <typename T>
class VectorIt {
private:
    T* data;
    size_t capacity;
    size_t size;

    void resize(size_t new_capacity);

public:
    class Iterator {
    private:
        T* ptr;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(T* p) : ptr(p) {}

        T& operator*() { return *ptr; }
        Iterator& operator++() { ++ptr; return *this; }
        Iterator operator++(int) { Iterator temp = *this; ++ptr; return temp; }
        Iterator& operator--() { --ptr; return *this; }
        Iterator operator--(int) { Iterator temp = *this; --ptr; return temp; }

        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
        bool operator==(const Iterator& other) const { return ptr == other.ptr; }
        bool operator<(const Iterator& other) const { return ptr < other.ptr; }
        bool operator>(const Iterator& other) const { return ptr > other.ptr; }
        bool operator<=(const Iterator& other) const { return ptr <= other.ptr; }
        bool operator>=(const Iterator& other) const { return ptr >= other.ptr; }

        Iterator operator+(int d) { return Iterator(ptr + d); }
        Iterator operator-(int d) { return Iterator(ptr - d); }
        int operator-(const Iterator& other) { return ptr - other.ptr; }
    };

    VectorIt();
    VectorIt(const VectorIt<T>& other);
    VectorIt(VectorIt<T>&& other) noexcept;
    VectorIt& operator=(const VectorIt<T>& other);
    VectorIt& operator=(VectorIt<T>&& other) noexcept;
    ~VectorIt();

    void push_back(const T& value);
    void pop_back();
    size_t get_size() const;
    bool is_empty() const;

    void remove_at(size_t index);
    void remove(const T& value);
    std::vector<T> toStdVector() const;
    void fromStdVector(const std::vector<T>& vec);

    template<typename Predicate>
    VectorIt<T> find(Predicate pred) const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    Iterator begin() const;
    Iterator end() const;
};

template <typename T>
VectorIt<T>::VectorIt() : data(nullptr), capacity(0), size(0) {}

template <typename T>
VectorIt<T>::VectorIt(const VectorIt<T>& other) : data(nullptr), capacity(0), size(0) {
    resize(other.size);
    for (size_t i = 0; i < other.size; ++i)
        data[i] = other.data[i];
    size = other.size;
}

template <typename T>
VectorIt<T>::VectorIt(VectorIt<T>&& other) noexcept : data(other.data), capacity(other.capacity), size(other.size) {
    other.data = nullptr;
    other.capacity = 0;
    other.size = 0;
}

template <typename T>
VectorIt<T>& VectorIt<T>::operator=(const VectorIt<T>& other) {
    if (this != &other) {
        delete[] data;
        resize(other.size);
        for (size_t i = 0; i < other.size; ++i)
            data[i] = other.data[i];
        size = other.size;
    }
    return *this;
}

template <typename T>
VectorIt<T>& VectorIt<T>::operator=(VectorIt<T>&& other) noexcept {
    if (this != &other) {
        delete[] data;
        data = other.data;
        capacity = other.capacity;
        size = other.size;

        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }
    return *this;
}

template <typename T>
VectorIt<T>::~VectorIt() {
    delete[] data;
}

template <typename T>
void VectorIt<T>::resize(size_t new_capacity) {
    T* new_data = new T[new_capacity];
    for (size_t i = 0; i < size; ++i)
        new_data[i] = data[i];
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

template <typename T>
void VectorIt<T>::push_back(const T& value) {
    if (size >= capacity) {
        size_t new_capacity = (capacity == 0) ? 1 : capacity * 2;
        resize(new_capacity);
    }
    data[size++] = value;
}

template <typename T>
void VectorIt<T>::pop_back() {
    if (size == 0)
        throw std::out_of_range("Vector is empty. Cannot pop_back.");
    --size;
}

template <typename T>
size_t VectorIt<T>::get_size() const {
    return size;
}

template <typename T>
bool VectorIt<T>::is_empty() const {
    return size == 0;
}

template <typename T>
void VectorIt<T>::remove_at(size_t index) {
    if (index >= size)
        throw std::out_of_range("Index out of range.");
    for (size_t i = index; i < size - 1; ++i)
        data[i] = data[i + 1];
    --size;
}

template <typename T>
void VectorIt<T>::remove(const T& value) {
    for (size_t i = 0; i < size; ++i) {
        if (data[i] == value) {
            remove_at(i);
            return;
        }
    }
}

template <typename T>
std::vector<T> VectorIt<T>::toStdVector() const {
    return std::vector<T>(data, data + size);
}

template <typename T>
void VectorIt<T>::fromStdVector(const std::vector<T>& vec) {
    resize(vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
        data[i] = vec[i];
    size = vec.size();
}

template <typename T>
template <typename Predicate>
VectorIt<T> VectorIt<T>::find(Predicate pred) const {
    VectorIt<T> result;
    for (size_t i = 0; i < size; ++i) {
        if (pred(data[i])) {
            result.push_back(data[i]);
        }
    }
    return result;
}

template <typename T>
T& VectorIt<T>::operator[](size_t index) {
    if (index >= size)
        throw std::out_of_range("Index out of range.");
    return data[index];
}

template <typename T>
const T& VectorIt<T>::operator[](size_t index) const {
    if (index >= size)
        throw std::out_of_range("Index out of range.");
    return data[index];
}

template <typename T>
typename VectorIt<T>::Iterator VectorIt<T>::begin() const {
    return Iterator(data);
}

template <typename T>
typename VectorIt<T>::Iterator VectorIt<T>::end() const {
    return Iterator(data + size);
}

#endif // VECTORIT_H
