#ifndef __RELATIONALEXPRESSION_H
#define __RELATIONALEXPRESSION_H

#include "base.h"
#include <stdexcept>

    struct EQVisitor {
        bool operator()(int l, int r) const { return l == r; }
        bool operator()(double l, double r) const { return l == r; }
        bool operator()(bool l, bool r) const { return l == r; }
        bool operator()(const std::string& l, const std::string& r) const { return l == r; }
        template <typename T, typename U>
        bool operator()(T, U) const {
            return false;
        }
    };
    struct NEQVisitor {
        template <typename T, typename U>
        bool operator()(const T& l, const U& r) const {
            return !EQVisitor{}(l, r);
        }
    };
    struct LTVisitor {
        bool operator()(int l, int r) const { return l < r; }
        bool operator()(double l, double r) const { return l < r; }
        bool operator()(const std::string& l, const std::string& r) const { return l < r; }
        template <typename T, typename U>
        bool operator()(T, U) const {
            return false;
        }
    };
    struct GTVisitor {
        template <typename T, typename U>
        bool operator()(const T& l, const U& r) const {
            return LTVisitor{}(r, l);
        }
    };
    struct LEQVisitor {
        template <typename T, typename U>
        bool operator()(const T& l, const U& r) const {
            return LTVisitor{}(l, r) || EQVisitor{}(l, r);
        }
    };
    struct GEQVisitor {
        template <typename T, typename U>
        bool operator()(const T& l, const U& r) const {
            return GTVisitor{}(l, r) || EQVisitor{}(l, r);
        }
    };
    BINARYOP(RelationalExpressionEQ, RelationalExpression, ArithmeticExpression, bool, EQVisitor )
    BINARYOP(RelationalExpressionNEQ, RelationalExpression, ArithmeticExpression, bool, NEQVisitor )
    BINARYOP(RelationalExpressionLT, RelationalExpression, ArithmeticExpression, bool, LTVisitor )
    BINARYOP(RelationalExpressionGT, RelationalExpression, ArithmeticExpression, bool, GTVisitor )
    BINARYOP(RelationalExpressionLEQ, RelationalExpression, ArithmeticExpression, bool, LEQVisitor )
    BINARYOP(RelationalExpressionGEQ, RelationalExpression, ArithmeticExpression, bool, GEQVisitor )

#endif
