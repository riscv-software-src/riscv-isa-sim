//
// Created by alex on 8/5/2024.
//

#ifndef MASOFTREGISTER_H
#define MASOFTREGISTER_H
#include <cstdint>
#include <cstdlib>

class MASoftRegister {
protected:
    bool valid;
    uint8_t dtype;
public:
    int64_t width, height;
    void* data;
    MASoftRegister(): valid(false), dtype(255), width(-1), height(-1), data(nullptr) {}

    void setSize(const int64_t _width, const int64_t _height, const uint8_t _dtype) {
        width = _width;
        height = _height;
        dtype = _dtype;

        valid = true;

        if (data != nullptr)  free(data);

        switch (dtype) {
            case 0: data = malloc(width * height); break;
            case 1: data = malloc(width * height); break;
            case 2: data = malloc(width * height * 2); break;
            case 3: data = malloc(width * height * 2); break;
            case 4: data = malloc(width * height * 4); break;
            default: data = malloc(width * height * 4); break;
        }
    }

    void markValid() {
        valid = false;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    bool isValid() const {
        return valid;
    }

    uint8_t getDtype() const {
        return dtype;
    }
};

#endif //MASOFTREGISTER_H
