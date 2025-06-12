#ifndef __ARITHMETICEXPRESSION_H
#define __ARITHMETICEXPRESSION_H

#include "base.h"
#include <stdexcept>
#include <type_traits>
#include <variant>

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
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
                return std::visit([&] (auto&& val) -> llvm::Value* {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, int>) {
                        return llvm::ConstantInt::get(builder.getInt32Ty(), val);
                    } else if constexpr (std::is_same_v<T, float>) {
                        return llvm::ConstantFP::get(builder.getFloatTy(), val);
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return llvm::ConstantInt::get(builder.getInt1Ty(), val);
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        return builder.CreateGlobalString(val);
                    } else {
                        return nullptr;
                    }
                }, value);
            }
    };

    // Visitors to define operators in interpreter mode only

    struct AddVisitor {
        Types operator()(int l, int r) const { return l + r; }
        Types operator()(float l, float r) const { return l + r; }
        Types operator()(int l, float r) const { return (float)l + r; }
        Types operator()(float l, int r) const { return l + (float)r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for addition");
        }
    };
    struct SubVisitor {
        Types operator()(int l, int r) const { return l - r; }
        Types operator()(float l, float r) const { return l - r; }
        Types operator()(int l, float r) const { return (float)l - r; }
        Types operator()(float l, int r) const { return l - (float)r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for substracton");
        }
    };
    struct MultVisitor {
        Types operator()(int l, int r) const { return l * r; }
        Types operator()(float l, float r) const { return l * r; }
        Types operator()(int l, float r) const { return (float)l * r; }
        Types operator()(float l, int r) const { return l * (float)r; }
        template<typename T, typename U>
        Types operator()(T l, U r) const {
            throw std::runtime_error("Invalid types for multiplication");
        }
    };
    struct DivVisitor {
        Types operator()(int l, int r) const { return l / r; }
        Types operator()(float l, float r) const { return l / r; }
        Types operator()(int l, float r) const { return (float)l / r; }
        Types operator()(float l, int r) const { return l / (float)r; }
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
        Types operator()(float x) const { return -x; }
        template<typename T>
        Types operator()(T l) const {
            throw std::runtime_error("Invalid types for negation");
        }
    };

    // Helper function to convert operands to the same type
    inline llvm::Value* convertOperands(llvm::Value* lhs, llvm::Value* rhs, llvm::IRBuilder<>& builder) {
        if (lhs->getType()->isIntegerTy() && rhs->getType()->isFloatTy()) {
            return builder.CreateSIToFP(lhs, builder.getFloatTy());
        } else if (lhs->getType()->isFloatTy() && rhs->getType()->isIntegerTy()) {
            return builder.CreateSIToFP(rhs, builder.getFloatTy());
        }
        return nullptr;
    }

    BINARYOP(ArithmeticExpressionPlus, ArithmeticExpression, ArithmeticExpression, Types, AddVisitor,
        (lhs->getType()->isFloatTy() || rhs->getType()->isFloatTy()) ?
            builder.CreateFAdd(
                // Convert all operands to float
                lhs->getType()->isIntegerTy() ? builder.CreateSIToFP(lhs, builder.getFloatTy()) : lhs,
                rhs->getType()->isIntegerTy() ? builder.CreateSIToFP(rhs, builder.getFloatTy()) : rhs
            ) : 
            builder.CreateAdd(lhs, rhs))

    BINARYOP(ArithmeticExpressionMinus, ArithmeticExpression, ArithmeticExpression, Types, SubVisitor,
        (lhs->getType()->isFloatTy() || rhs->getType()->isFloatTy()) ?
            builder.CreateFSub(
                lhs->getType()->isIntegerTy() ? builder.CreateSIToFP(lhs, builder.getFloatTy()) : lhs,
                rhs->getType()->isIntegerTy() ? builder.CreateSIToFP(rhs, builder.getFloatTy()) : rhs
            ) : 
            builder.CreateSub(lhs, rhs))

    BINARYOP(ArithmeticExpressionMult, ArithmeticExpression, ArithmeticExpression, Types, MultVisitor,
        (lhs->getType()->isFloatTy() || rhs->getType()->isFloatTy()) ?
            builder.CreateFMul(
                lhs->getType()->isIntegerTy() ? builder.CreateSIToFP(lhs, builder.getFloatTy()) : lhs,
                rhs->getType()->isIntegerTy() ? builder.CreateSIToFP(rhs, builder.getFloatTy()) : rhs
            ) : 
            builder.CreateMul(lhs, rhs))

    BINARYOP(ArithmeticExpressionDiv, ArithmeticExpression, ArithmeticExpression, Types, DivVisitor,
        (lhs->getType()->isFloatTy() || rhs->getType()->isFloatTy()) ?
            builder.CreateFDiv(
                lhs->getType()->isIntegerTy() ? builder.CreateSIToFP(lhs, builder.getFloatTy()) : lhs,
                rhs->getType()->isIntegerTy() ? builder.CreateSIToFP(rhs, builder.getFloatTy()) : rhs
            ) : 
            builder.CreateSDiv(lhs, rhs))

    BINARYOP(ArithmeticExpressionDivInt, ArithmeticExpression, ArithmeticExpression, Types, DivIntVisitor, 
        builder.CreateSDiv(lhs, rhs))

    BINARYOP(ArithmeticExpressionDivMod, ArithmeticExpression, ArithmeticExpression, Types, DivModVisitor, 
        builder.CreateSRem(lhs, rhs))

    UNARYOP(ArithmeticExpressionNeg, ArithmeticExpression, ArithmeticExpression, Types, NegVisitor,
        val->getType()->isFloatTy() ? builder.CreateFNeg(val) : builder.CreateNeg(val))

#endif
