#ifndef __RELATIONALEXPRESSION_H
#define __RELATIONALEXPRESSION_H

#include "base.h"
#include <stdexcept>

    struct EQVisitor {
        bool operator()(int l, int r) const { return l == r; }
        bool operator()(float l, float r) const { return l == r; }
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
        bool operator()(float l, float r) const { return l < r; }
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
BINARYOP(RelationalExpressionEQ, RelationalExpression, ArithmeticExpression, bool, EQVisitor,
 [&]() -> llvm::Value* {
             if (lhs->getType()->isIntegerTy(32)) {
                 return builder.CreateICmpEQ(lhs, rhs);
             } else if (lhs->getType()->isFloatTy()) {
                 return builder.CreateFCmpOEQ(lhs, rhs);
             } else {
                 throw std::runtime_error("Unsupported operand types for equality comparison");
             }
         }())
BINARYOP(RelationalExpressionNEQ, RelationalExpression, ArithmeticExpression, bool, NEQVisitor,
 [&]() -> llvm::Value* {
             if (lhs->getType()->isIntegerTy(32)) {
                 return builder.CreateICmpNE(lhs, rhs);
             } else if (lhs->getType()->isFloatTy()) {
                 return builder.CreateFCmpONE(lhs, rhs);
             } else {
                 throw std::runtime_error("Unsupported operand types for inequality comparison");
             }
         }())
BINARYOP(RelationalExpressionLT, RelationalExpression, ArithmeticExpression, bool, LTVisitor,
 [&]() -> llvm::Value* {
             if (lhs->getType()->isIntegerTy(32)) {
                 return builder.CreateICmpSLT(lhs, rhs);
             } else if (lhs->getType()->isFloatTy()) {
                 return builder.CreateFCmpOLT(lhs, rhs);
             } else {
                 throw std::runtime_error("Unsupported operand types for less than comparison");
             }
         }())
BINARYOP(RelationalExpressionGT, RelationalExpression, ArithmeticExpression, bool, GTVisitor,
 [&]() -> llvm::Value* {
             if (lhs->getType()->isIntegerTy(32) && rhs->getType()->isIntegerTy(32)) {
                 return builder.CreateICmpSGT(lhs, rhs);
             } else if (lhs->getType()->isFloatTy() && rhs->getType()->isFloatTy()) {
                 return builder.CreateFCmpOGT(lhs, rhs);
             } else if (lhs->getType()->isIntegerTy(32) && rhs->getType()->isFloatTy()) {
                 llvm::Value* converted = builder.CreateSIToFP(lhs, builder.getFloatTy());
                 return builder.CreateFCmpOGT(converted, rhs);
             } else if (lhs->getType()->isFloatTy() && rhs->getType()->isIntegerTy(32)) {
                 llvm::Value* converted = builder.CreateSIToFP(rhs, builder.getFloatTy());
                 return builder.CreateFCmpOGT(lhs, converted);
             } else {
                 throw std::runtime_error("Unsupported operand types for greater than comparison");
             }
         }())
BINARYOP(RelationalExpressionLEQ, RelationalExpression, ArithmeticExpression, bool, LEQVisitor,
 [&]() -> llvm::Value* {
             if (lhs->getType()->isIntegerTy(32)) {
                 return builder.CreateICmpSLE(lhs, rhs);
             } else if (lhs->getType()->isFloatTy()) {
                 return builder.CreateFCmpOLE(lhs, rhs);
             } else {
                 throw std::runtime_error("Unsupported operand types for less equal comparison");
             }
         }())
BINARYOP(RelationalExpressionGEQ, RelationalExpression, ArithmeticExpression, bool, GEQVisitor,
 [&]() -> llvm::Value* {
             if (lhs->getType()->isIntegerTy(32)) {
                 return builder.CreateICmpSGE(lhs, rhs);
             } else if (lhs->getType()->isFloatTy()) {
                 return builder.CreateFCmpOGE(lhs, rhs);
             } else {
                 throw std::runtime_error("Unsupported operand types for greater equal comparison");
             }
         }())
#endif
