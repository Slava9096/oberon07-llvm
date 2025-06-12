#ifndef __LVALUE_H
#define __LVALUE_H

#include "base.h"
#include "llvm/IR/DerivedTypes.h"
#include <stdexcept>

class LocationValueVariable : public LocationValue
{
    std::string name;
    public:
        LocationValueVariable(std::string name) 
        {
            this->name = name;
        }
        ~LocationValueVariable() override {}
        void Set(Types value, Context* context) override
        {
            context->values[name] = value;
        }
        std::string GetName() const override {
            return name;
        }
        Types Evaluate(Context* context) override
        {
            return context->values[name];
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder, SymbolTable* symbolTable) override {
            Symbol* symbol = symbolTable->lookup(name);

            // If variable is not found
            if (!symbol || !symbol->allocaInst) {
                throw std::runtime_error("Variable '" + name + "' not declared");
            }

            llvm::AllocaInst* alloca = symbol->allocaInst;

            return builder.CreateLoad(alloca->getAllocatedType(), alloca);
        }
};

#endif
