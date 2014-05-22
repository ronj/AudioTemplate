#ifndef THREE_FLOATING_POINT_HOOK_H
#define THREE_FLOATING_POINT_HOOK_H

// TODO: EA: Make more generic for use with Euler order/enums/etc.

#include <iostream>
#include <type_traits>

template<typename T, class C, class Enable = void>
class FloatingPointHook;

template<typename T, class C>
class FloatingPointHook<T, C, typename std::enable_if<std::is_floating_point<T>::value >::type> {
    
public:
    
    FloatingPointHook()
    : value( 0 ), obj( nullptr ), hook( nullptr ) {
    }
    
    FloatingPointHook(const T other, C *objIn, void (C::*hookIn)() )
    : value( other ), obj( objIn), hook( hookIn ) {
        THREE_REVIEW("EA: Initialization order")
        //((obj)->*(hook))();
    }
    
   // FloatingPointHook(const FloatingPointHook& other)
   // : value( other.value ), obj( other.obj ), hook( other.hook ) { ((obj)->*(hook))(); }
    
    T value;

    C *obj;
    
    void (C::*hook)();

    operator const T() const { return value; }
        
    inline FloatingPointHook& operator=(const T other){ value = other; ((obj)->*(hook))(); return *this; }

    inline FloatingPointHook& operator*=(const T other){ value *= other; ((obj)->*(hook))(); return *this;}
    inline FloatingPointHook& operator/=(const T other){ value /= other; ((obj)->*(hook))(); return *this; }
    inline FloatingPointHook& operator+=(const T other){ value += other; ((obj)->*(hook))(); return *this; }
    inline FloatingPointHook& operator-=(const T other){ value -= other; ((obj)->*(hook))(); return *this; }
    inline FloatingPointHook& operator%=(const T other){ value %= other; ((obj)->*(hook))(); return *this; }
    
    inline T operator-(const T other){ return value - other; }
    inline T operator+(const T other){ return value + other; }
    inline T operator*(const T other){ return value * other; }
    inline T operator/(const T other){ return value / other; }
    inline T operator%(const T other){ return value % other; }
    
    inline bool operator==(const T& other) const { return value == other; }
    inline bool operator!=(const T& other) const { return value != other; }
    inline bool operator< (const T& other) const { return value < other; }
    inline bool operator> (const T& other) const { return value < other; }
    inline bool operator<=(const T& other) const { return value <= other; }
    inline bool operator>=(const T& other) const { return value >= other; }
    
    
    inline FloatingPointHook& operator=(const FloatingPointHook& other){ value = other.value; hook = other.hook; ((obj)->*(hook))(); return *this; }
    
    inline FloatingPointHook& operator*=(const FloatingPointHook& other){ value *= other.value; ((obj)->*(hook))(); return *this;}
    inline FloatingPointHook& operator/=(const FloatingPointHook& other){ value /= other.value; ((obj)->*(hook))(); return *this; }
    inline FloatingPointHook& operator+=(const FloatingPointHook& other){ value += other.value; ((obj)->*(hook))(); return *this; }
    inline FloatingPointHook& operator-=(const FloatingPointHook& other){ value -= other.value; ((obj)->*(hook))(); return *this; }
    inline FloatingPointHook& operator%=(const FloatingPointHook& other){ value %= other.value; ((obj)->*(hook))(); return *this; }
    
    inline T operator-(const FloatingPointHook& other) const { return value - other.value; }
    inline T operator+(const FloatingPointHook& other) const { return value - other.value; }
    inline T operator*(const FloatingPointHook& other) const { return value - other.value; }
    inline T operator/(const FloatingPointHook& other) const { return value - other.value; }
    inline T operator%(const FloatingPointHook& other) const { return value - other.value; }
    
    inline bool operator==(const FloatingPointHook& other) const { return value == other.value; }
    inline bool operator!=(const FloatingPointHook& other) const { return value != other.value; }
    inline bool operator< (const FloatingPointHook& other) const { return value < other.value; }
    inline bool operator> (const FloatingPointHook& other) const { return value < other.value; }
    inline bool operator<=(const FloatingPointHook& other) const { return value <= other.value; }
    inline bool operator>=(const FloatingPointHook& other) const { return value >= other.value; }
    
    inline friend std::ostream& operator<<(std::ostream &out, const FloatingPointHook<T, C>& other) {
        out << other.value;
        return out;
    }
    
    inline friend std::istream& operator>>(std::istream &in, const FloatingPointHook<T, C>& other) {
        in >> other.value;
        return in;
    }
    
    inline friend std::istream& operator>>(std::istream &in, const T other) {
        in >> other;
        return in;
    }
};

#define FLOAT_HOOK(c, h, v) FloatingPointHook<float, c>(v, this, &c::h)
#define DOUBLE_HOOK(c, h, v) FloatingPointHook<double, c>(v, this, &c::h)

#endif