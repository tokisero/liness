#pragma once
#include "VectorIt.h"
#include <stdexcept>

template <typename T>
class Queue {
private:
    VectorIt<T> data;

public:
    void enqueue(const T& element) {
        data.push_back(element);
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty. Cannot dequeue.");
        }
        T frontElement = data[0];
        data.remove_at(0);
        return frontElement;
    }

    T& front() {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty. Cannot access front.");
        }
        return data[0];
    }

    const T& front() const {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty. Cannot access front.");
        }
        return data[0];
    }

    VectorIt<T> toVector() const {
        VectorIt<T> result;
        for (auto it = data.begin(); it != data.end(); ++it) {
            result.push_back(*it);
        }
        return result;
    }


    void removeElement(const T& element) {
        for (size_t i = 0; i < data.get_size(); ++i) {
            if (data[i] == element) {
                data.remove_at(i);
                return;
            }
        }
        throw std::invalid_argument("Element not found in queue.");
    }

    bool isEmpty() const {
        return data.is_empty();
    }

    size_t size() const {
        return data.get_size();
    }
};
