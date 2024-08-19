//
// Created by alex on 8/5/2024.
//

#ifndef MATRIXACCELERATORMACROS_H
#define MATRIXACCELERATORMACROS_H

template <typename T>
T readData(int ptr) {
    return (T)1;
}

template <typename T>
void writeData(T data, int ptr) {
    //
}

#define LOAD_MA_REGISTER(SR, PTR, DTYPE) ({ \
        unsigned int addr = (PTR); \
        const int step = sizeof(DTYPE); \
        DTYPE* ref = (DTYPE*)SR.data; \
        for (int i = 0; i < SR.getHeight(); ++i) { \
            for ( int j = 0; j < SR.getWidth(); ++j) { \
                ref[i * SR.width + j] = readData<DTYPE>(addr); \
                addr += step; \
            } \
        } \
    })

#define STORE_MA_REGISTER(SR, PTR, DTYPE) ({ \
        unsigned int addr = (PTR); \
        const int step = sizeof(DTYPE); \
        DTYPE* ref = (DTYPE*)SR.data; \
        for (int i = 0; i < SR.getHeight(); ++i) { \
            for ( int j = 0; j < SR.getWidth(); ++j) { \
            writeData<DTYPE>(ref[i * SR.width + j], addr); \
            addr += step; \
            } \
        } \
    })

#define MA_VV_SIMPLE_LOOP(SRD, SR1, SR2, OP, DTYPE) ({ \
        DTYPE* a = (DTYPE*)SR1.data; \
        DTYPE* b = (DTYPE*)SR2.data; \
        DTYPE* c = (DTYPE*)SRD.data; \
        for ( int i = 0; i < SRD.getHeight(); ++i ) \
            for ( int j = 0; j < SRD.getWidth(); ++j ) \
                c[i * SRD.width + j] = a[i * SR1.width + j] OP b[i * SR2.width + j]; \
    })

#define MA_VS_LOOP(SRD, SR1, RS2, OP, DTYPE) ({ \
        DTYPE* a = (DTYPE*)SR1.data; \
        DTYPE* c = (DTYPE*)SRD.data; \
        for ( int i = 0; i < SRD.getHeight(); ++i ) \
            for ( int j = 0; j < SRD.getWidth(); ++j ) \
                c[i * SRD.width + j] = a[i * SR1.width + j] OP (RS2); \
    })

#define MA_VV_MULT(SRD, SR1, SR2, DTYPE) ({ \
        DTYPE* a = (DTYPE*)SR1.data; \
        DTYPE* b = (DTYPE*)SR2.data; \
        DTYPE* c = (DTYPE*)SRD.data; \
        for ( int i = 0; i < SR1.getHeight(); ++i ) \
            for ( int j = 0; j < SR2.getWidth(); ++j ) { \
                c[i * SRD.width + j] = 0; \
                for ( int k = 0; k < SR1.getWidth(); ++k ) \
                    c[i * SRD.width + j] += a[i * SR1.width + j] * b[i * SR2.width + j]; \
        } \
    })

#define MA_CNV(SRD, SR1, SR2, DTYPE) ({ \
    DTYPE* a = (DTYPE*)SR1.data; \
    DTYPE* b = (DTYPE*)SR2.data; \
    SRD.setSize(SR1.getWidth() - SR2.getWidth() + 1, \
    SR1.getHeight() - SR2.getHeight() + 1, \
    SRD.getDtype()); \
    c = (DTYPE*)r.data; \
    for ( int i = 0; i < SRD.getHeight(); ++i ) \
        for ( int j = 0; j < SRD.getWidth(); ++j ) { \
            c[i * SRD.width + j] = 0; \
            for ( int ii = 0; ii < SR2.getHeight(); ++ii ) \
                for ( int jj = 0; jj < SR2.getWidth(); ++jj ) \
                    c[i * SR1.width + j] += a[(i + ii) * SR1.width + (j + jj)] * b[ii * SR2.width + jj]; \
        } \
    })

#endif //MATRIXACCELERATORMACROS_H
