# lwgpp - A LOOP, WHILE and maybe a GOTO interpreter

An interpreter for [LOOP, WHILE and GOTO](https://en.wikipedia.org/wiki/LOOP_(programming_language)) written in C++.

> *Please note that the project was conducted in a group. See the contributor list for more information.*

## Description

Developed for the [SWAPK (Advanced programming concepts)]([https://kursuskatalog.au.dk/en/course/134686/SWAFP-01-Applied-Functional-Programming](https://studerende.au.dk/studier/fagportaler/ece/uddannelser/diplomingenioer/valgfag/valgfag-kun-efteraar/swapk-avancerede-programmeringskoncepter/)) course at Aarhus University. 

*There are some bigger known bugs in the code, nevertheless the interpreter does its honest work*

## Concepts

* Fundamentals
* Templates
* Namespaces
* STL Containers
* STL Algorithms
* Exceptions
* C++11++
* Concepts
* Meta programming
* Boost Lib

*Please refer to a more concise list of concepts in the `docs/` folder.*

The goal of the project is to implement 75% of the course concepts into a project, like lwgpp, and present the project in an oral exam.

## How to use

Currently, the program is more thought of as a library, therefore it should not be ran as a cli. However, feel free to modify it or send a PR.

```bash
$ git clone git@github.com:gwydd12/lwgpp.git
$ cd lwgpp/
$ cmake -S . -B build
$ cmake --build build/
$ cd build/
$ ./lwgpp
```

## Support

In case you have any questions, please feel free to reach out to me via email or create an issue in this repository.

Additionally, if you are interested in taking SWAPK
feel free to reach out to me for more information about the course and the exam I might remember some parts of it or refer to the
docs folder 

Also if you have any feedback on the code, please let me know as this is my first F# project.

## Acknowledgments

* My group members
* Søren Hansen for the course at the Aarhus University
* [cppreference](https://en.cppreference.com/w/)
* [Microsoft C++ language reference](https://fsharpforfunandprofit.com)



