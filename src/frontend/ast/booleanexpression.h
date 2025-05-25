#ifndef __BOOLEANEXPRESSION_H
#define __BOOLEANEXPRESSION_H

#include "base.h"

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

    BINARYOP(BooleanExpressionAnd, BooleanExpression, BooleanExpression, bool,  && )
    BINARYOP(BooleanExpressionOr, BooleanExpression, BooleanExpression, bool,  || )
    UNARYOP(BooleanExpressionNot, BooleanExpression, BooleanExpression, bool, ! )

#endif
