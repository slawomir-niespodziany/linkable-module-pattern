cmake_minimum_required(VERSION 3.22)

project (lmf 
         VERSION 0.1 
         DESCRIPTION "Linkable module framework library.")

add_library(lmf INTERFACE)

target_include_directories(lmf 
                           INTERFACE include)
