#ifndef __LVALUE_H
#define __LVALUE_H

#include "base.h"
#include "llvm/IR/DerivedTypes.h"

class LocationValueVariable : public LocationValue
{
    std::string name;
    public:
        LocationValueVariable(const std::string& name)
        {
            this->name = name;
        }
        ~LocationValueVariable()
        {
        }
        Types Evaluate(Context* context) override
        {
            return context->values[name];
        }
        void Set(Types value, Context* context) override
        {
            context->values[name] = value;
        }
        llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>& builder) override {
            llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
            llvm::AllocaInst* alloca = nullptr;

            // Look for existing alloca in the entry block
            for (auto& inst : currentFunc->getEntryBlock()) {
                if (auto* existingAlloca = llvm::dyn_cast<llvm::AllocaInst>(&inst)) {
                    if (existingAlloca->getName() == name) {
                        alloca = existingAlloca;
                        break;
                    }
                }
            }

            if (!alloca) {
                // If not found, create a new alloca in the entry block
                llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(), currentFunc->getEntryBlock().begin());
                // Check if this is a float variable by looking at the declaration
                bool isFloat = false;
                for (auto& inst : currentFunc->getEntryBlock()) {
                    if (auto* existingAlloca = llvm::dyn_cast<llvm::AllocaInst>(&inst)) {
                        if (existingAlloca->getName() == name) {
                            isFloat = existingAlloca->getAllocatedType()->isFloatTy();
                            break;
                        }
                    }
                }
                alloca = entryBuilder.CreateAlloca(isFloat ? builder.getFloatTy() : builder.getInt32Ty(), nullptr, name);
            }

            return builder.CreateLoad(alloca->getAllocatedType(), alloca, name + "_val");
        }
        llvm::Value* getPointer(llvm::LLVMContext* context, llvm::IRBuilder<>& builder) override {
            llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
            llvm::AllocaInst* alloca = nullptr;

            // Look for existing alloca in the entry block
            for (auto& inst : currentFunc->getEntryBlock()) {
                if (auto* existingAlloca = llvm::dyn_cast<llvm::AllocaInst>(&inst)) {
                    if (existingAlloca->getName() == name) {
                        alloca = existingAlloca;
                        break;
                    }
                }
            }

            if (!alloca) {
                // If not found, create a new alloca in the entry block
                llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(), currentFunc->getEntryBlock().begin());
                // Check if this is a float variable by looking at the declaration
                bool isFloat = false;
                for (auto& inst : currentFunc->getEntryBlock()) {
                    if (auto* existingAlloca = llvm::dyn_cast<llvm::AllocaInst>(&inst)) {
                        if (existingAlloca->getName() == name) {
                            isFloat = existingAlloca->getAllocatedType()->isFloatTy();
                            break;
                        }
                    }
                }
                alloca = entryBuilder.CreateAlloca(isFloat ? builder.getFloatTy() : builder.getInt32Ty(), nullptr, name);
            }

            return alloca;
        }
};

#endif
