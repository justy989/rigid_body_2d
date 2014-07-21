#ifndef IB_VECTOR2_H
#define IB_VECTOR2_H

#include <cmath>
#include <limits>

template < typename T >
class vector2 {
public:

    // simple constructor
    vector2 ( T x = 0.0f,
              T y = 0.0f );

    // operators
    vector2<T> operator- ( ) const;
    vector2<T> operator+ ( const vector2<T>& v ) const;
    vector2<T>& operator+= ( const vector2<T>& v );
    vector2<T> operator- ( const vector2<T>& v ) const;
    vector2<T>& operator-= ( const vector2<T>& v );

    T operator* ( const vector2<T>& v ) const; // dot product

    vector2<T> operator* ( T scale ) const;
    vector2<T>& operator*= ( T scale );

    vector2<T> operator/ ( T scale ) const;
    vector2<T>& operator/= ( T scale );

    // helper functions
    T mag ( ) const;
    T dot ( const vector2<T>& v ) const;
    T perp_dot ( const vector2<T>& v ) const;

    float distance_to ( const vector2<T>& v ) const;

    vector2<T> project_onto ( const vector2<T>& v ) const;

    void perp ( );
    void normalize ( );
    void zero ( );
    void negate ( );

    // accessors
    T x ( ) const { return m_x; }
    T y ( ) const { return m_y; }

    void set ( T x, T y ) { m_x = x; m_y = y; }
    void set_x ( T x ) { m_x = x; }
    void set_y ( T y ) { m_y = y; }

private:
    T m_x;
    T m_y;
};

template < typename T >
inline vector2<T>::vector2 ( T x,
                            T y ) :
                            m_x{ x },
                            m_y{ y }
{}

template < typename T >
inline vector2<T> vector2<T>::operator- ( ) const
{
    return vector2<T> { -m_x,
                        -m_y };
}

template < typename T >
inline vector2<T> vector2<T>::operator+ ( const vector2<T>& v ) const
{
    return vector2<T> { m_x + v.m_x,
                        m_y + v.m_y };
}

template < typename T >
inline vector2<T>& vector2<T>::operator+= ( const vector2<T>& v )
{
    m_x += v.m_x;
    m_y += v.m_y;

    return *this;
}

template < typename T >
inline vector2<T> vector2<T>::operator- ( const vector2<T>& v ) const
{
    return vector2 <T> { m_x - v.m_x,
                         m_y - v.m_y };
}

template < typename T >
inline vector2<T>& vector2<T>::operator-= ( const vector2<T>& v )
{
    m_x -= v.m_x;
    m_y -= v.m_y;

    return *this;
}

template < typename T >
inline T vector2<T>::operator* ( const vector2<T>& v ) const // dot product
{
    return (m_x * v.m_x) + (m_y * v.m_y);
}

template < typename T >
inline vector2<T> vector2<T>::operator* ( T scale ) const
{
    return vector2<T> { m_x * scale,
                        m_y * scale };
}

template < typename T >
inline vector2<T>& vector2<T>::operator*= ( T scale )
{
    m_x *= scale;
    m_y *= scale;

    return *this;
}

template < typename T >
inline vector2<T> vector2<T>::operator/ ( T scale) const
{
    return vector2<T> { m_x / scale,
                        m_y / scale };
}

template < typename T >
inline vector2<T>& vector2<T>::operator/= ( T scale )
{
    m_x /= scale;
    m_y /= scale;

    return *this;
}

template < typename T >
inline T vector2<T>::mag ( ) const
{
    return sqrt( m_x * m_x + m_y * m_y );
}

template < typename T >
inline T vector2<T>::dot ( const vector2<T>& v ) const
{
    return ( m_x * v.m_x ) + ( m_y * v.m_y );
}

template < typename T >
inline T vector2<T>::perp_dot ( const vector2<T>& v ) const
{
    return ( m_x * v.m_y ) - ( m_y * v.m_x );
}

template < typename T >
inline float vector2<T>::distance_to ( const vector2<T>& v ) const
{
    float x_diff = m_x - v.m_x;
    float y_diff = m_y - v.m_y;

    return sqrt ( ( x_diff * x_diff ) + ( y_diff * y_diff ) );
}

template < typename T >
inline vector2<T> vector2<T>::project_onto ( const vector2<T>& v ) const
{
    float v_mag = v.mag ( );

    float num = dot ( v );
    float denum = v_mag * v_mag;

    return v * ( num / denum );
}

template < typename T >
inline void vector2<T>::perp ()
{
    T tmp = m_x;
    m_x = -m_y;
    m_y = tmp;
}

template < typename T >
inline void vector2<T>::normalize ( )
{
    float m = mag ( );

    if( static_cast<float>(fabs(m)) < std::numeric_limits<float>::epsilon() ) {
        m_x = 0.0f;
        m_y = 0.0f;
        return;
    }

    float d = 1.0f / m;

    m_x *= d;
    m_y *= d;
}

template < typename T >
inline void vector2<T>::zero ( )
{
    m_x = 0.0;
    m_y = 0.0;
}

template < typename T >
inline void vector2<T>::negate ()
{
    m_x = -m_x;
    m_y = -m_y;
}

using vec2 = vector2<float>;

#endif

