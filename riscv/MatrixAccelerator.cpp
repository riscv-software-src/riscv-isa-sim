#include "MatrixAccelerator.h"

SoftRegister::SoftRegister() : width(-1), height(-1), valid(false), dtype(MADType::UNDEFINED), data(nullptr) {}

void SoftRegister::setSize(int _width, int _height, MADType _dtype)
{
    width = _width;
    height = _height;
    dtype = _dtype;

    valid = true;

    if (data != nullptr)
        free(data);

    switch (dtype)
    {
    case MADType::INT8:
        data = malloc(width * height);
        break;
    case MADType::UINT8:
        data = malloc(width * height);
        break;
    case MADType::INT16:
        data = malloc(width * height * 2);
        break;
    case MADType::UINT16:
        data = malloc(width * height * 2);
        break;
    case MADType::INT32:
        data = malloc(width * height * 4);
        break;
    case MADType::UINT32:
        data = malloc(width * height * 4);
        break;
    }
}

void SoftRegister::markValid()
{
    valid = false;
}

int SoftRegister::getWidth() const
{
    return width;
}

int SoftRegister::getHeight() const
{
    return height;
}

bool SoftRegister::isValid() const
{
    return valid;
}

MADType SoftRegister::getDtype() const
{
    return dtype;
}

template <typename T>
T MAOperationSolverBase<T>::readData(unsigned int addr)
{
    return MMU.load<T>(addr);
}

static void writeData(const T &data, unsigned int addr)
{
    MMU.store<T>(addr, data);
}

static void readMatrix(const SoftRegister &reg, unsigned int ptr)
{
    unsigned int addr = ptr;
    const int step = sizeof(T);
    T *ref = (T *)reg.data;

    for (int i = 0; i < reg.getHeight(); ++i)
    {
        for (int j = 0; j < reg.getWidth(); ++j)
        {
            ref[i * reg.width + j] = readData(addr);
            addr += step;
        }
    }
}

static void writeMatrix(const SoftRegister &reg, unsigned int ptr)
{
    unsigned int addr = ptr;
    const int step = sizeof(T);
    T *ref = (T *)reg.data;

    for (int i = 0; i < reg.getHeight(); ++i)
    {
        for (int j = 0; j < reg.getWidth(); ++j)
        {
            writeData(ref[i * reg.width + j], addr);
            addr += step;
        }
    }
}