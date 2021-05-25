#pragma once
template<class T>
struct Vec2
{
    Vec2() 
    {
        this->x = 0;
        this->y = 0;
    }
    Vec2(T x, T y)
    {
        this->x = x;
        this->y = y;
    }

    T x, y;
};

typedef Vec2<float>  vec2f;
typedef Vec2<int>    vec2i;
typedef Vec2<unsigned int> vec2u;