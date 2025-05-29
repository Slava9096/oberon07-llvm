#ifndef __BASE_H
#define __BASE_H

#include <map>
#include <string>
#include <variant>

using Types = std::variant<int, double, bool, std::string>;
using TypesNumeric = std::variant<int, double>;

class Context
{
    public:
        std::map<std::string, Types> values;
        Context() {}
        ~Context() {}
};

class Statement
{
    public:
        Statement();
        virtual ~Statement();
        virtual void Execute(Context* context) = 0;
};

class BooleanExpression
{
    public:
        BooleanExpression();
        virtual ~BooleanExpression();
        virtual bool Evaluate(Context* context) = 0;
};

class RelationalExpression : public BooleanExpression
{
    public:
        RelationalExpression();
        ~RelationalExpression();
};

class ArithmeticExpression
{
    public:
        ArithmeticExpression(){};
        virtual ~ArithmeticExpression(){};
        virtual Types Evaluate(Context* context) = 0;
};

class LocationValue : public ArithmeticExpression
{
    public:
        LocationValue(){};
        virtual ~LocationValue(){};
        virtual void Set(Types value, Context* context) = 0;
};

#define BINARYOP(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION) \
    class NAME : public BASETYPE { \
            CONTENTTYPE* l; \
            CONTENTTYPE* r; \
        public: \
            NAME(CONTENTTYPE* l, CONTENTTYPE* r) : BASETYPE(){ \
                this->l = l; \
                this->r = r; \
            } \
            ~NAME(){ \
                delete l; \
                delete r; \
            } \
            RETURNTYPE Evaluate(Context* context) override{ \
                return std::visit(OPERATION{}, l->Evaluate(context), r->Evaluate(context)); \
            } \
        };


#define UNARYOP(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION) \
    class NAME : public BASETYPE { \
            CONTENTTYPE* x; \
        public: \
            NAME(CONTENTTYPE* x) : BASETYPE() { \
                this->x = x; \
            }\
            ~NAME(){ \
                delete x; \
            }\
            RETURNTYPE Evaluate(Context* context) override{ \
                return std::visit(OPERATION{}, x->Evaluate(context)); \
            } \
        };

#endif
