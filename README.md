# IRC_to_analyze

# Plano para analize do codigo:
- server (o codigo esta dentro de src/)
  - entender a lógica do main.cpp e como ele chama os outros modulos
  - entender os .h e como estao implementados
  - entender os .cpp e as bibliotecas usadas

# Bibliotecas usadas:

- boost -> Boost é uma coleção de bibliotecas que estendem a funcionalidade da linguagem

 - boost/thread -> Boost.Thread enables the use of multiple threads of execution with shared data in portable C++ code. It provides classes and functions for managing the threads themselves, along with others for synchronizing data between the threads or providing separate copies of data specific to individual threads.

 - boost/bind -> Bind function with the help of placeholders, helps to manipulate the position and number of values to be used by the function and modifies the function according to the desired output.

 - sstream -> String streams são derivações das streams de arquivos, muito utilizadas em C++. 

 - shared_ptr -> The shared_ptr class template stores a pointer to a dynamically allocated object, typically with a C++ new-expression. The object pointed to is guaranteed to be deleted when the last shared_ptr pointing to it is destroyed or reset.

 - mysql -> database

 - boost/function ->  contains a family of class templates that are function object wrappers

 - cstdlib -> This header defines several general purpose functions, including dynamic memory management, random number generation, communication with the environment, integer arithmetics, searching, sorting and converting.
