#ifndef __BOOLEANEXPRESSION_H
#define __BOOLEANEXPRESSION_H

#include "base.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
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
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
                return llvm::ConstantInt::get(builder.getInt1Ty(), 1);
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
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
                return llvm::ConstantInt::get(builder.getInt1Ty(), 0);
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

#define BINARYOP_DIRECT(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION, CODEGEN) \
        class NAME : public BASETYPE { \
            CONTENTTYPE* l; \
            CONTENTTYPE* r; \
        public: \
            NAME(CONTENTTYPE* l, CONTENTTYPE* r) : l(l), r(r) {} \
            ~NAME() override { delete l; delete r; } \
            RETURNTYPE Evaluate(Context* context) override { \
                return OPERATION{}(l->Evaluate(context), r->Evaluate(context)); \
            } \
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override { \
                llvm::Value* lhs = l->codegen(context, builder, symbolTable); \
                llvm::Value* rhs = r->codegen(context, builder, symbolTable); \
                return CODEGEN; }\
        };

#define UNARYOP_DIRECT(NAME, BASETYPE, CONTENTTYPE, RETURNTYPE, OPERATION, CODEGEN) \
        class NAME : public BASETYPE { \
            CONTENTTYPE* x; \
        public: \
            NAME(CONTENTTYPE* x) : x(x) {} \
            ~NAME() override { delete x; } \
            RETURNTYPE Evaluate(Context* context) override { \
                return OPERATION{}(x->Evaluate(context)); \
            } \
            llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override { \
                llvm::Value* val = x->codegen(context, builder, symbolTable); \
                return CODEGEN; }\
        };

    BINARYOP_DIRECT(BooleanExpressionAnd, BooleanExpression, BooleanExpression, bool, ANDVisitor, builder.CreateAnd(lhs,rhs))
    BINARYOP_DIRECT(BooleanExpressionOr, BooleanExpression, BooleanExpression, bool, ORVisitor, builder.CreateOr(lhs,rhs))
    UNARYOP_DIRECT(BooleanExpressionNot, BooleanExpression, BooleanExpression, bool, NOTVisitor, builder.CreateXor(val, builder.getInt1(true)))

#endif
