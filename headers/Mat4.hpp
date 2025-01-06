#pragma once

#include<iostream>

namespace Math{
    template<typename T>
    class Mat4{
        T items[16];
    
        public:
            Mat4(void): Mat4((T)0) {}
    
            Mat4(T val){
                for(int i=0; i<4; i++)
                    for(int j=0; j<4; j++)
                        items[i*4+j] = (i==j) ? val : (T)0;
            }
    
            Mat4(T (&arr)[16]){
                for(int i=0; i<4; i++)
                    for(int j=0; j<4; j++)
                        items[j*4+i] = arr[i*4+j];
            }
    
            ~Mat4(){}
    
            T* operator[](int i){
                return &items[i*4];
            }
    
            Mat4<T> operator*(Mat4<T> arg){
                Mat4<T> prod = Mat4<T>();
                for(int i=0; i<4; i++)
                    for(int j=0; j<4; j++)
                        for(int k=0; k<4; k++)
                            prod[j][i] += items[k*4+i] * arg[j][k];
                return prod;
            }
    
            friend std::ostream& operator<<(std::ostream& out, Mat4 mat){
                for(int i=0; i<4; i++){
                    for(int j=0; j<4; j++)
                        out << mat.items[i*4+j] << "\t";
                    out << "\n";
                }
                return out;
            }
    };
}
