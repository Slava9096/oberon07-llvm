
#ifndef __ARITHMETICEXPRESSION_H
#define __ARITHMETICEXPRESSION_H

#include "base.h"
#include <stdexcept>

    class ArithmeticExpressionConst : public ArithmeticExpression
    {
        Types value;
        public:
            ArithmeticExpressionConst(Types value) : ArithmeticExpression()
            {
                this->value = value;
            }
            ~ArithmeticExpressionConst()
            {
            }
            Types Evaluate(Context* context) override
            {
                return value;
            };
    };
    struct AddVisitor {
        Types operator()(int l, int r) const { return l + r; }
        Types operator()(double l, double r) const { return l + r; }
        Types operator()(int l, double r) const { return (double)l + r; }
        Types operator()(double l, int r) const { return l + (double)r; }
        Types operator()(const std::string& l, const std::string& r) const { return l + r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for addition");
        }
    };
    struct SubVisitor {
        Types operator()(int l, int r) const { return l - r; }
        Types operator()(double l, double r) const { return l - r; }
        Types operator()(int l, double r) const { return (double)l - r; }
        Types operator()(double l, int r) const { return l - (double)r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for substracton");
        }
    };
    struct MultVisitor {
        Types operator()(int l, int r) const { return l * r; }
        Types operator()(double l, double r) const { return l * r; }
        Types operator()(int l, double r) const { return (double)l * r; }
        Types operator()(double l, int r) const { return l * (double)r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for multiplication");
        }
    };
    struct DivVisitor {
        Types operator()(int l, int r) const { return l / r; }
        Types operator()(double l, double r) const { return l / r; }
        Types operator()(int l, double r) const { return (double)l / r; }
        Types operator()(double l, int r) const { return l / (double)r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for division");
        }
    };
    struct DivIntVisitor {
        Types operator()(int l, int r) const { return l / r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for integer division");
        }
    };
    struct DivModVisitor {
        Types operator()(int l, int r) const { return l % r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for mod division");
        }
    };
    struct NegVisitor {
        Types operator()(int x) const { return -x; }
        Types operator()(double x) const { return -x; }
        template<typename T>
        Types operator()(T l) const {
            throw std::runtime_error("Invalid types for negation");
        }
    };

    BINARYOP(ArithmeticExpressionPlus, ArithmeticExpression, ArithmeticExpression, Types, AddVisitor )
    BINARYOP(ArithmeticExpressionMinus, ArithmeticExpression, ArithmeticExpression, Types, SubVisitor )
    BINARYOP(ArithmeticExpressionMult, ArithmeticExpression, ArithmeticExpression, Types, MultVisitor )
    BINARYOP(ArithmeticExpressionDiv, ArithmeticExpression, ArithmeticExpression, Types, DivVisitor )
    BINARYOP(ArithmeticExpressionDivInt, ArithmeticExpression, ArithmeticExpression, Types, DivIntVisitor )
    BINARYOP(ArithmeticExpressionDivMod, ArithmeticExpression, ArithmeticExpression, Types, DivModVisitor )
    UNARYOP(ArithmeticExpressionNeg, ArithmeticExpression, ArithmeticExpression, Types, NegVisitor )

#endif
