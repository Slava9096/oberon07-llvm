
#ifndef __STATEMENT_H
#define __STATEMENT_H

#include <iostream>
#include <vector>

#include "base.h"
class StatementBlock : public Statement
{
    std::vector<Statement*>statements;
    public:
        StatementBlock(std::vector<Statement*>& statements)
        {
            this->statements = statements;
        }
        ~StatementBlock()
        {
            for(Statement* statement : statements) delete statement;
            statements.clear();
        }
        void Execute(Context* context) override
        {
            for(Statement* statement : statements) statement->Execute(context);
        }
};

class StatementWrite: public Statement
{
    std::string text;
    public:
        StatementWrite(std::string text)
        {
            this->text = text;
        }
        ~StatementWrite()
        {
        }
        void Execute(Context* context) override
        {
            std::cout << text;
        }
};

template<typename T>
class StatementAssign: public Statement
{
    LocationValue<T>* lvalue;
    ArithmeticExpression<T>* expression;
    public:
        StatementAssign(LocationValue<T>* lvalue, ArithmeticExpression<T>* expression)
        {
            this->lvalue = lvalue;
            this->expression = expression;
        }
        ~StatementAssign()
        {
            delete lvalue;
            delete expression;
        }
        void Execute(Context* context) override
        {
            int tmp = expression->Evaluate(context);
            lvalue->Set(tmp, context);
        }
};


class StatementWriteInt: public Statement
{
    ArithmeticExpression<int>* expression;
    public:
        StatementWriteInt(ArithmeticExpression<int>* expression)
        {
            this->expression = expression;
        }
        ~StatementWriteInt()
        {
            delete expression;
        }
        void Execute(Context* context) override
        {
            std::cout << ( expression->Evaluate(context) );
        }
};

class StatementReadInt: public Statement
{
    LocationValue<int>* lvalue;
    public:
        StatementReadInt(LocationValue<int>* lvalue)
        {
            this->lvalue= lvalue;
        }
        ~StatementReadInt()
        {
            delete lvalue;
        }
        void Execute(Context* context) override
        {
            int tmp;
            std::cin >> tmp;
            lvalue->Set(tmp, context);
        }
};

class StatementWhile: public Statement
{
    BooleanExpression* condition;
    Statement* statement;
    public:
        StatementWhile(BooleanExpression* condition, Statement* statement)
        {
            this->condition = condition;
            this->statement = statement;
        }
        ~StatementWhile()
        {
            delete condition;
            delete statement;
        }
        void Execute(Context* context) override
        {
            while (condition->Evaluate(context)) {
                statement->Execute(context);
            }
        }
};


class StatementIfElseIfElse: public Statement
{
    std::vector<BooleanExpression*> conditions;
    std::vector<Statement*> blocks;
    public:
        StatementIfElseIfElse(std::vector<BooleanExpression*> conditions, std::vector<Statement*> blocks)
        {
            this->conditions = conditions;
            this->blocks = blocks;
        }
        ~StatementIfElseIfElse()
        {
            for(BooleanExpression* condition : conditions) delete condition;
            for(Statement* block : blocks) delete block;
        }
        void Execute(Context* context) override
        {
            for (int i = 0; i < conditions.size(); i++) {
                if (conditions[i]->Evaluate(context)) {
                    blocks[i]->Execute(context);
                    break;
                }
            }
        }
};

#endif
