cmake_minimum_required(VERSION 3.14)

project("blasius"
        VERSION 0.1.0
        DESCRIPTION "BLAS dynamic dispatch wrapper"
        LANGUAGES CXX C FORTRAN
)

add_library(blasius INTERFACE)
