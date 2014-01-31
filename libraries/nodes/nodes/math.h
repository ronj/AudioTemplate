#ifndef MATH_H
#define MATH_H

#include <nodes/nodes/common.h>

#include <cmath>

struct Round : public NodeNumberUnary
{
  Round() : NodeNumberUnary("Round", "Math") { }
  float process(float aIn) { return std::round(aIn); }
};

struct Ceil : public NodeNumberUnary
{
  Ceil() : NodeNumberUnary("Ceil", "Math") { }
  float process(float aIn) { return std::ceil(aIn); }
};

struct Floor : public NodeNumberUnary
{
  Floor() : NodeNumberUnary("Floor", "Math") { }
  float process(float aIn) { return std::floor(aIn); }
};

struct Add : public NodeNumberBinary
{
  Add() : NodeNumberBinary("Add", "Math") { }
  float process(float aX, float aY) { return aX + aY; }
};

struct Subtract : public NodeNumberBinary
{
  Subtract() : NodeNumberBinary("Subtract", "Math") { }
  float process(float aX, float aY) { return aX - aY; }
};

struct Multiply : public NodeNumberBinary
{
  Multiply() : NodeNumberBinary("Multiply", "Math") { }
  float process(float aX, float aY) { return aX * aY; }
};

struct Divide : public NodeNumberBinary
{
  Divide() : NodeNumberBinary("Divide", "Math") { }
  float process(float aX, float aY) { return aX / aY; }
};

struct Mod : public NodeNumberBinary
{
  Mod() : NodeNumberBinary("Mod", "Math") { }
  float process(float aX, float aY) { return std::fmod(aX, aY); }
};

struct Min : public NodeNumberBinary
{
  Min() : NodeNumberBinary("Min", "Math") { }
  float process(float aX, float aY) { return std::min(aX, aY); }
};

struct Max : public NodeNumberBinary
{
  Max() : NodeNumberBinary("Max", "Math") { }
  float process(float aX, float aY) { return std::max(aX, aY); }
};

#endif // MATH_H
