#ifndef __AR_FUNCTION_KERNEL_H__
#define __AR_FUNCTION_KERNEL_H__


#include <stack>
#include <iostream>
#include "ar-simplex3d.h"



/**
 * Represents function suitable for the FunctionKernel. Albeit very restrictive 
 * (it only supports subtraction), although that should suffice for KineticSort.
 */
class ARFunction
{
    public:
        /// Represents the three forms of the involved functions. See Appendix B of LHI paper.
        enum                        FunctionForm                                        { none, rho, lambda, phi };

    public:
                                    ARFunction(FunctionForm f, ARSimplex3D* s): 
                                        form_(f), form2_(none),
                                        simplex_(s), simplex2_(0)                       {}
                                    ARFunction(const ARFunction& other):
                                        form_(other.form_), form2_(other.form2_),
                                        simplex_(other.simplex_), 
                                        simplex2_(other.simplex2_)                      {}

        ARFunction&                 operator-=(const ARFunction& other)                 { form2_ = other.form_; simplex2_ = other.simplex_; return *this; }
        ARFunction                  operator-(const ARFunction& other)                  { return (ARFunction(*this) -= other); }

        FunctionForm                form() const                                        { return form_; }
        FunctionForm                form2() const                                       { return form2_; }
        ARSimplex3D*                simplex() const                                     { return simplex_; }
        ARSimplex3D*                simplex2() const                                    { return simplex2_; }

        std::ostream&               operator<<(std::ostream& out) const                 { return (out << "(" << form_ << ", " << simplex_ << "), (" 
                                                                                                             << form2_ << ", " << simplex2_ << ")"); }

    private:
        FunctionForm                form_, form2_;      // the function is form_ - form2_
        ARSimplex3D                 *simplex_, *simplex2_;
};

std::ostream&
operator<<(std::ostream& out, const ARFunction& f)
{ return f.operator<<(out); }

/**
 * Function kernel specialized at solving the kinds of functions involved in 
 * ARVineyard construction. We cannot use a polynomial kernel (which includes 
 * rational functions) that we already have because not all involved functions
 * are rational.
 */
class ARFunctionKernel
{
    public:
        typedef                     double                                              FieldType;
        typedef                     FieldType                                           RootType;
        typedef                     std::stack<RootType>                                RootStack;
        typedef                     ARSimplex3D::RealValue                              SimplexFieldType;

        typedef                     ARFunction                                          Function;
        typedef                     Function::FunctionForm                              FunctionForm;
    
    public:
        static void                 solve(const Function& f, RootStack& stack);
        static RootType             root(RootType r)                                    { return r; }
        static RootType             root(SimplexFieldType r)                            { return CGAL::to_double(r); }
        static int                  sign_at(const Function& f, RootType r);
        static RootType             between(RootType r1, RootType r2)                   { return (r1 + r2)/2; }
        static int                  sign_at_negative_infinity(const Function& f);

        static FieldType            value_at(const Function& f, RootType v);
};

#include "ar-function-kernel.hpp"

#endif
