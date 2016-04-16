/*
    Copyright (c) 2009 Zhang li

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/*
    Author zhang li
    Email zlvbvbzl@gmail.com
*/

#ifndef __VM_VALUE__
#define __VM_VALUE__

#include "PuMap.h"
#include "PuString.h"
#include "config.h"

#include <stdio.h>
#include <assert.h>
#ifdef _MSC_VER
#pragma warning(disable:4127) // 判断条件为常量，比如：while(1)
#endif
struct Pu;

#ifndef _DEBUG
#define DECVMAP_REF(userdata)                        \
if (userdata)                                        \
{                                                    \
    ((_up_value*)(userdata))->refcount--;            \
    if (((_up_value*)(userdata))->refcount == 0)    \
    {                                                \
        delete ((_up_value*)(userdata))->vmap;        \
        ((_up_value*)(userdata))->vmap = 0;            \
        userdata = 0;                                \
    }                                                \
}
#else
void DECVMAP_REF(void *&userdata);
#endif


#define VALUE_IS_TRUE(v)                                        \
(                                                                \
    ((v).type() == BOOLEANT)?                                    \
        ((v).numVal() != 0)                                        \
                                                                \
    :(((v).type() == NUM)?                                        \
        ((v).numVal() != 0)                                        \
                                                                \
    :(((v).type() == CPTR)?                                     \
        (*(PU_INT*)&(v).numVal() != 0)                            \
                                                                \
    :(((v).type() == STR)?                                        \
        ((v).strVal().length() != 0)                            \
                                                                \
    :(((v).type() == ARRAY)?                                    \
        ((v).arr().size() != 0)                                    \
                                                                \
    :(((v).type() == MAP)?                                    \
        ((v).map().size() != 0)                                    \
                                                                \
    :(((v).type() == CORO)?                                        \
        ((v).numVal() != -1)                                    \
                                                                \
    :(((v).type() == FILEHANDLE)?                                \
        ((v).userdata() != 0 )                                  \
                                                                \
    :(((v).type() == FUNCTION)?                                    \
        (true):(false)                                            \
                                                                \
    ))))))))                                                       \
)

typedef enum PUVALUECREATEDBY{
    PU_SYSTEM,
    PU_USER
}PUVALUECREATEDBY;

struct __pu_value
{
    typedef PuVector<__pu_value> ValueArr;
    typedef PuMap<__pu_value, __pu_value, 10> PuCommMap;
    typedef PuMap<PuString, __pu_value, 10> PuValMap;

    struct _up_value
    {
        PuValMap *vmap;
        int refcount;
        bool marked;
        _up_value *next;
    };

    __pu_value(Pu *_L)
        :L(_L)
        ,createby(PU_SYSTEM)
        ,_type(UNKNOWN)
        ,arr_(0)
        ,userdata_(0)
    {
    }

    __pu_value()
        :L(NULL)
        ,createby(PU_SYSTEM)
        ,_type(UNKNOWN)
        ,arr_(0)
        ,userdata_(0)
    {
    }

    __pu_value(const __pu_value &x)
        :L(x.L)
        ,createby(PU_SYSTEM)
        ,_type(UNKNOWN)
        ,arr_(0)
        ,userdata_(0)
    {
        *this = x;
    }

    ~__pu_value();

    void operator =(const __pu_value &x);
    __pu_value operator +(const __pu_value &x);
    __pu_value operator -(const __pu_value &x);
    __pu_value operator /(const __pu_value &x);
    __pu_value operator %(const __pu_value &x);
    __pu_value operator *(const __pu_value &x);
    int operator >(const __pu_value &x) const;
    int operator <(const __pu_value &x) const;
    int operator >=(const __pu_value &x) const;
    int operator <=(const __pu_value &x) const;
    int operator !=(const __pu_value &x) const;
    int operator ==(const __pu_value &x) const;
    int operator ||(const __pu_value &x) const;
    int operator &&(const __pu_value &x) const;
    const __pu_value &operator +=(const __pu_value &x);
    const __pu_value &operator -=(const __pu_value &x);
    const __pu_value &operator *=(const __pu_value &x);
    const __pu_value &operator /=(const __pu_value &x);
    
    Pu *L;
    PUVALUECREATEDBY createby;

    PuType type() const 
    { 
        return _type; 
    }

    void SetType(PuType val) 
    { 
        if (val != _type)
        {
            destroy();
            _type = val; 
            build();
        }
    }

    unsigned int hash() const;

    ValueArr& arr()
    {
#ifdef _DEBUG
        if (type() != ARRAY || !arr_)
        {
            assert(!"arr get value failed");
        }
#endif
        return *arr_;
    }

    const ValueArr& arr() const
    {
#ifdef _DEBUG
        if (type() != ARRAY || !arr_)
        {
            assert(!"arr get value failed");
        }
#endif
        return *arr_;
    }

    PuCommMap& map()
    {
#ifdef _DEBUG
        if (type() != MAP || !map_)
        {
            assert(!"map get value failed");
        }
#endif
        return *map_;
    }

    const PuCommMap& map() const
    {
#ifdef _DEBUG
        if (type() != MAP || !map_)
        {
            assert(!"map get value failed");
        }
#endif
        return *map_;
    }

    PuString& strVal()
    {
        return *strVal_;
    }

    const PuString& strVal() const
    {
#ifdef _DEBUG
        if (type() != STR || !strVal_)
        {
            assert(!"string get value failed");
        }
#endif
        return *strVal_;
    }

    PU_NUMBER& numVal()
    {
        return numVal_;
    }

    const PU_NUMBER& numVal() const
    {
#ifdef _DEBUG
        if (type() != FUN && type() != BOOLEANT && type() != NUM && type() != CORO && type() != CFUN && type() != CPTR)
        {
            assert(!"number get value failed");
        }
#endif
        return numVal_;
    }

    void *&userdata()
    {    
        return userdata_;
    }

    void *const userdata() const
    {   
        return userdata_;
    }

    void destroy()
    {
        switch (type())
        {
        case STR:
            delete strVal_;
            strVal_ = NULL;
            break;
        case ARRAY:
            delete arr_;
            arr_ = NULL;
            break;
        case FUN:
            DECVMAP_REF(userdata());
            break;
        case MAP:
            delete map_;
            map_ = NULL;
        default:
            break;
        }        
    }

private:

    void build()
    {
        switch (type())
        {
        case STR:
            strVal_ = new PuString;
            break;
        case MAP:
            map_ = new PuCommMap;
            break;
        case ARRAY:
            arr_ = new ValueArr;
            break;
        default:
            break;
        }
    }
    PuType _type;    
    union {
    PU_NUMBER numVal_;
    ValueArr *arr_;
    PuCommMap *map_;            
    PuString *strVal_;    
    };
    void *userdata_;  
};

typedef __pu_value::ValueArr ValueArr;
typedef __pu_value::PuCommMap ValueMap;
typedef __pu_value::PuValMap StrKeyMap;
typedef __pu_value::_up_value _up_value;

#endif
