
#ifndef __LVALUE_H
#define __LVALUE_H

#include "base.h"

class LocationValueVariable : public LocationValue
{
    std::string name;
    public:
        LocationValueVariable(const std::string& name) : LocationValue()
    {
        this->name = name;
    }
    ~LocationValueVariable()
    {
    }
    void Set(Types value, Context* context) override
    {
        context->values[name] = value;
    };
    Types Evaluate(Context* context) override
    {
        return context->values[name];
    };
};

#endif
