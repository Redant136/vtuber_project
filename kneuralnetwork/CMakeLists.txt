cmake_minimum_required(VERSION 3.8)
project(kneuralnetwork VERSION 1.0)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(knn knn_NEAT.c knn_DeepLearning.c knn_DeepLearning_calculate.c knn_DeepLearning_backpropagate.c)
