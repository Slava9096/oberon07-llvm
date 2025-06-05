#ifndef __LVALUE_H
#define __LVALUE_H

#include "base.h"
#include "llvm/IR/DerivedTypes.h"

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
        Types Evaluate(Context* context) override
        {
            return context->values[name];
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder) override {
            llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
            llvm::AllocaInst* alloca = nullptr;

            // Ищем существующее alloca
            for (auto& inst : currentFunc->getEntryBlock()) {
                if ((alloca = llvm::dyn_cast<llvm::AllocaInst>(&inst))) {
                    if (alloca->getName() == name) {
                        return builder.CreateLoad(alloca->getAllocatedType(), alloca, name + "_val");
                    }
                }
            }

            // Если не найдено — ошибка (переменная не объявлена)
            throw std::runtime_error("Variable '" + name + "' not declared");
        }
        llvm::Value* getPointer(llvm::LLVMContext* context, llvm::IRBuilder<>& builder) override {
            llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
            llvm::AllocaInst* alloca = nullptr;

            // Check if variable already exists
            for (auto& inst : currentFunc->getEntryBlock()) {
                if (auto* existingAlloca = llvm::dyn_cast<llvm::AllocaInst>(&inst)) {
                    if (existingAlloca->getName() == name) {
                        alloca = existingAlloca;
                        break;
                    }
                }
            }

            if (!alloca) {
                throw std::runtime_error("Variable '" + name + "' not declared");
            }

            return alloca;
        }
};

#endif
