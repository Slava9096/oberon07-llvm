#ifndef __RELATIONALEXPRESSION_H
#define __RELATIONALEXPRESSION_H

#include "base.h"

    template<typename T>
    BINARYOP(RelationalExpressionEQ, RelationalExpression, ArithmeticExpression<T>, bool, == )
    template<typename T>
    BINARYOP(RelationalExpressionNEQ, RelationalExpression, ArithmeticExpression<T>, bool, != )
    template<typename T>
    BINARYOP(RelationalExpressionLess, RelationalExpression, ArithmeticExpression<T>, bool, < )
    template<typename T>
    BINARYOP(RelationalExpressionGreater, RelationalExpression, ArithmeticExpression<T>, bool, > )
    template<typename T>
    BINARYOP(RelationalExpressionLEQ, RelationalExpression, ArithmeticExpression<T>, bool, <= )
    template<typename T>
    BINARYOP(RelationalExpressionGRQ, RelationalExpression, ArithmeticExpression<T>, bool, >= )

#endif
