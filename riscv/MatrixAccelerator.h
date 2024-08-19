//
// Created by alex on 6/12/2024.
//

#ifndef MATRIXACCELERATOR_H
#define MATRIXACCELERATOR_H
#include <cstdint>
#include <cstdlib>

enum class MADType
{
    UNDEFINED,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32
};

enum class MAOperation
{
    ADD = 0b0000000,
    SUB = 0b0000010,
    CNV = 0b0001000,
    DIV = 0b0010000,
    MULT = 0b0100000,
    MULTEL = 0b1100000
};

class SoftRegister
{
protected:
    bool valid;
    MADType dtype;

public:
    int width, height;
    void *data;
    SoftRegister() : width(-1), height(-1), valid(false), dtype(MADType::UNDEFINED), data(nullptr) {}

    void setSize(int _width, int _height, MADType _dtype)
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

    void markValid()
    {
        valid = false;
    }

    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

    bool isValid() const
    {
        return valid;
    }

    MADType getDtype() const
    {
        return dtype;
    }
};

template <typename T>
class MAOperationSolverBase
{
public:
    static T readData(unsigned int addr)
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
};

template <typename T>
class MAVVOperationSolver
{
public:
    static void solver(const SoftRegister &op1, const SoftRegister &op2, SoftRegister &r, const MAOperation &op)
    {
        T *a = (T *)op1.data;
        T *b = (T *)op2.data;
        T *c = (T *)r.data;

        switch (op)
        {
        case MAOperation::ADD:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] + b[i * op2.width + j];
            break;
        case MAOperation::SUB:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] - b[i * op2.width + j];
            break;
        case MAOperation::DIV:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] / b[i * op2.width + j];
            break;
        case MAOperation::MULTEL:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] * b[i * op2.width + j];
            break;
        case MAOperation::MULT:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op2.getWidth(); ++j)
                {
                    c[i * r.width + j] = 0;
                    for (int k = 0; k < op1.getWidth(); ++k)
                        c[i * r.width + j] = a[i * op1.width + j] * b[i * op2.width + j];
                }
            break;
        case MAOperation::CNV:
            r.setSize(op1.getWidth() - op2.getWidth() + 1,
                      op1.getHeight() - op2.getHeight() + 1,
                      r.getDtype());
            c = (T *)r.data;
            for (int i = 0; i < r.getHeight(); ++i)
                for (int j = 0; j < r.getWidth(); ++j)
                {
                    c[i * r.width + j] = 0;
                    for (int ii = 0; ii < op2.getHeight(); ++ii)
                        for (int jj = 0; jj < op2.getWidth(); ++jj)
                            c[i * r.width + j] += a[(i + ii) * op1.width + (j + jj)] * b[ii * op2.width + jj];
                }
            break;
        }
    }
};

template <typename T>
class MAVSOperationSolver
{
public:
    static void solver(const SoftRegister &op1, int op2, SoftRegister &r, const MAOperation &op)
    {
        T *a = (T *)op1.data;
        T *c = (T *)r.data;

        switch (op)
        {
        case MAOperation::ADD:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] + op2;
            break;
        case MAOperation::SUB:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] - op2;
            break;
        case MAOperation::DIV:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] / op2;
            break;
        case MAOperation::MULTEL:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] * op2;
            break;
        case MAOperation::MULT:
            for (int i = 0; i < op1.getHeight(); ++i)
                for (int j = 0; j < op1.getWidth(); ++j)
                    c[i * r.width + j] = a[i * op1.width + j] * op2;
            break;
        }
    }
};

class MatrixAccelerator
{
private:
    MatrixAccelerator() {}

    SoftRegister regfile[32];

public:
    static MatrixAccelerator *getInstance()
    {
        static MatrixAccelerator *instancePtr = nullptr;

        if (instancePtr == nullptr)
        {
            instancePtr = new MatrixAccelerator();
        }

        return instancePtr;
    }

    static MADType decodeDtype(int funct7)
    {
        switch (funct7)
        {
        case 0b0000000:
            return MADType::INT8;
        case 0b1000000:
            return MADType::UINT8;
        case 0b0000010:
            return MADType::INT16;
        case 0b1000010:
            return MADType::UINT16;
        case 0b0000011:
            return MADType::INT32;
        case 0b1000011:
            return MADType::UINT32;
        default:
            return MADType::UNDEFINED;
        }
    }

    void defineRegister(int idx, int width, int height, MADType dtype)
    {
        // TODO: checi if idx is in [0; 31]
        regfile[idx].setSize(width, height, dtype);
        regfile[idx].markValid();
    }

    void readRegister(int idx, unsigned int ptr)
    {
        switch (regfile[idx].getDtype())
        {
        case MADType::INT8:
            MAOperationSolverBase<int8_t>::readMatrix(regfile[idx], ptr);
            break;
        case MADType::UINT8:
            MAOperationSolverBase<uint8_t>::readMatrix(regfile[idx], ptr);
            break;
        case MADType::INT16:
            MAOperationSolverBase<int16_t>::readMatrix(regfile[idx], ptr);
            break;
        case MADType::UINT16:
            MAOperationSolverBase<uint16_t>::readMatrix(regfile[idx], ptr);
            break;
        case MADType::INT32:
            MAOperationSolverBase<int32_t>::readMatrix(regfile[idx], ptr);
            break;
        case MADType::UINT32:
            MAOperationSolverBase<uint32_t>::readMatrix(regfile[idx], ptr);
            break;
        }
    }

    void writeRegister(int idx, unsigned int ptr)
    {
        switch (regfile[idx].getDtype())
        {
        case MADType::INT8:
            MAOperationSolverBase<int8_t>::writeMatrix(regfile[idx], ptr);
            break;
        case MADType::UINT8:
            MAOperationSolverBase<uint8_t>::writeMatrix(regfile[idx], ptr);
            break;
        case MADType::INT16:
            MAOperationSolverBase<int16_t>::writeMatrix(regfile[idx], ptr);
            break;
        case MADType::UINT16:
            MAOperationSolverBase<uint16_t>::writeMatrix(regfile[idx], ptr);
            break;
        case MADType::INT32:
            MAOperationSolverBase<int32_t>::writeMatrix(regfile[idx], ptr);
            break;
        case MADType::UINT32:
            MAOperationSolverBase<uint32_t>::writeMatrix(regfile[idx], ptr);
            break;
        }
    }

    void operation(int rIdx, int op1Idx, int op2Idx, MAOperation op)
    {
        switch (regfile[rIdx].getDtype())
        {
        case MADType::INT8:
            MAVVOperationSolver<int8_t>::solver(regfile[op1Idx], regfile[op2Idx], regfile[rIdx], op);
            break;
        case MADType::UINT8:
            MAVVOperationSolver<uint8_t>::solver(regfile[op1Idx], regfile[op2Idx], regfile[rIdx], op);
            break;
        case MADType::INT16:
            MAVVOperationSolver<int16_t>::solver(regfile[op1Idx], regfile[op2Idx], regfile[rIdx], op);
            break;
        case MADType::UINT16:
            MAVVOperationSolver<uint16_t>::solver(regfile[op1Idx], regfile[op2Idx], regfile[rIdx], op);
            break;
        case MADType::INT32:
            MAVVOperationSolver<int32_t>::solver(regfile[op1Idx], regfile[op2Idx], regfile[rIdx], op);
            break;
        case MADType::UINT32:
            MAVVOperationSolver<uint32_t>::solver(regfile[op1Idx], regfile[op2Idx], regfile[rIdx], op);
            break;
        }
    }

    void operation(MAOperation op, int rIdx, int op1Idx, int op2)
    {
        switch (regfile[rIdx].getDtype())
        {
        case MADType::INT8:
            MAVSOperationSolver<int8_t>::solver(regfile[op1Idx], op2, regfile[rIdx], op);
            break;
        case MADType::UINT8:
            MAVSOperationSolver<uint8_t>::solver(regfile[op1Idx], op2, regfile[rIdx], op);
            break;
        case MADType::INT16:
            MAVSOperationSolver<int16_t>::solver(regfile[op1Idx], op2, regfile[rIdx], op);
            break;
        case MADType::UINT16:
            MAVSOperationSolver<uint16_t>::solver(regfile[op1Idx], op2, regfile[rIdx], op);
            break;
        case MADType::INT32:
            MAVSOperationSolver<int32_t>::solver(regfile[op1Idx], op2, regfile[rIdx], op);
            break;
        case MADType::UINT32:
            MAVSOperationSolver<uint32_t>::solver(regfile[op1Idx], op2, regfile[rIdx], op);
            break;
        }
    }
};

#endif // MATRIXACCELERATOR_H
