#ifndef UTILS_HPP
#define UTILS_HPP

#include <random>

using namespace std;

#define EPS ((double) 1e-6)

int getRandomInteger(int l, int r, mt19937& gen);
float getRandomFloat(mt19937& gen);

#endif