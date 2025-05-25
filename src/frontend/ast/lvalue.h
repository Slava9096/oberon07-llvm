
#ifndef __LVALUE_H
#define __LVALUE_H

#include "base.h"

template<typename T>
class LocationValueVariable : public LocationValue<T>
{
    std::string name;
    public:
        LocationValueVariable(const std::string& name) : LocationValue<T>()
    {
        this->name = name;
    }
    ~LocationValueVariable()
    {
    }
    void Set(T value, Context* context) override
    {
        context->values[name] = value;
    };
    T Evaluate(Context* context) override
    {
        return std::get<T>(context->values[name]);
    };
};

#endif
