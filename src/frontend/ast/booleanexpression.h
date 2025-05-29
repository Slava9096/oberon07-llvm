#ifndef __BOOLEANEXPRESSION_H
#define __BOOLEANEXPRESSION_H

#include "base.h"
#include <stdexcept>

    class BooleanExpressionTrue : public BooleanExpression
        {
        public:
            BooleanExpressionTrue() : BooleanExpression()
            {
            }
            ~BooleanExpressionTrue()
            {
            }
            bool Evaluate(Context* context) override
            {
                return true;
            }
        };
    class BooleanExpressionFalse : public BooleanExpression
        {
        public:
            BooleanExpressionFalse() : BooleanExpression()
            {
            }
            ~BooleanExpressionFalse()
            {
            }
            bool Evaluate(Context* context) override
            {
                return false;
            }
        };

    struct ANDVisitor {
        bool operator()(bool l, bool r) const { return l && r; }
        template<typename T, typename U>
        bool operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for and operation");
        }
    };
    struct ORVisitor {
        bool operator()(bool l, bool r) const { return l || r; }
        template<typename T, typename U>
        bool operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for or operation");
        }
    };
    struct NOTVisitor{
        bool operator()(bool x) const { return !x; }
        template<typename T, typename U>
        bool operator()(T x) const {
            throw std::runtime_error("Invalid types for not operation");
        }
    };

#define BINARYOP_DIRECT(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION) \
        class NAME : public BASETYPE { \
            CONTENTTYPE* l; \
            CONTENTTYPE* r; \
        public: \
            NAME(CONTENTTYPE* l, CONTENTTYPE* r) : l(l), r(r) {} \
            ~NAME() override { delete l; delete r; } \
            RETURNTYPE Evaluate(Context* context) override { \
                return OPERATION{}(l->Evaluate(context), r->Evaluate(context)); \
            } \
        };

#define UNARYOP_DIRECT(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION) \
        class NAME : public BASETYPE { \
            CONTENTTYPE* x; \
        public: \
            NAME(CONTENTTYPE* x) : x(x) {} \
            ~NAME() override { delete x; } \
            RETURNTYPE Evaluate(Context* context) override { \
                return OPERATION{}(x->Evaluate(context)); \
            } \
        };

    BINARYOP_DIRECT(BooleanExpressionAnd, BooleanExpression, BooleanExpression, bool, ANDVisitor )
    BINARYOP_DIRECT(BooleanExpressionOr, BooleanExpression, BooleanExpression, bool, ORVisitor )
    UNARYOP_DIRECT(BooleanExpressionNot, BooleanExpression, BooleanExpression, bool, NOTVisitor )

#endif
