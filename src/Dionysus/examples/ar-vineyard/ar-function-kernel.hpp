#include <utilities/log.h>
#include <cmath>

#if LOGGING
static rlog::RLogChannel* rlARFunctionKernel =                      DEF_CHANNEL("ar/function-kernel", rlog::Log_Debug);
static rlog::RLogChannel* rlARFunctionKernelValue =                 DEF_CHANNEL("ar/function-kernel/value", rlog::Log_Debug);
#endif


/* For now only rho and phi are implemented */
void
ARFunctionKernel::
solve(const Function& f, RootStack& stack)
{
    AssertMsg(stack.empty(), "Stack must be empty before solving");
    AssertMsg((f.form() != Function::none) && (f.form2() != Function::none), "Solving possible only for differences");

    FunctionForm f1 = f.form(), f2 = f.form2();
    const ARSimplex3D   *s1 = f.simplex(), 
                        *s2 = f.simplex2();
    if (f1 < f2)    { std::swap(f1,f2); std::swap(s1,s2); }         // for simplicity enforce function order 
                                                                    // to handle fewer cases explicitly
    AssertMsg(f1 != Function::lambda && f2 != Function::lambda, "Lambda not implemented yet");

    rLog(rlARFunctionKernel,    "Solve: function1 = (%i, %p), function2 = (%i, %p)",
                                f1, s1, f2, s2);
    
    //if (f1 == Function::phi && f2 == Function::phi)         return;
    //if (f1 == Function::rho && f2 == Function::rho)         return;

    if (f1 == Function::phi && f2 == Function::rho)
    {
        SimplexFieldType r = s2->alpha() - s1->phi_const();
        rLog(rlARFunctionKernel, "  phi = rho => r^2 = %s (%lf)", tostring(r).c_str(), root(r));
        stack.push(root(r));
    }

    if (f1 == Function::phi && f2 == Function::lambda)
    {
        rLog(rlARFunctionKernel, "  phi = lambda");
        SimplexFieldType r2 = (s2->rho() + s2->v() - s2->s() - s1->phi_const());
        r2 *= r2;
        r2 /= 4*s2->v();
        r2 += s2->s();
        if (r2 >= s2->s() + s2->v())
            stack.push(root(r2));

        SimplexFieldType r1 = s2->rho() - s1->phi_const();
        if (r1 <= s2->s() + s2->v())
            stack.push(root(r1));
    }
    
    // FIXME: this is far from complete!
    if (f1 == Function::lambda && f2 == Function::lambda)
    {
        rLog(rlARFunctionKernel, "  lambda = lambda");
        if ((s1->s() + s1->v() < s2->s() + s2->v()))                // let f1 be the function with larger break point
        {   std::swap(f1,f2); std::swap(s1,s2); }

        if (s1->rho() > s2->rho())
        {
            RootType r = root(s2->s() + s2->v() + s1->rho() - s2->rho()) + 2*sqrt(root(s2->v()*(s1->rho() - s2->rho())));
            if (r < root(s1->s() + s1->v()) && r > root(s2->s() + s2->v()))
                stack.push(r);
        }
    }

    if (f1 == Function::lambda && f2 == Function::rho)
    {
        rLog(rlARFunctionKernel, "  lambda = rho");
        // perhaps no solutions instead of an assertion is the right way to deal with this
        AssertMsg(s2->alpha() > s1->rho(), "Rho_0^2 must be greater than Rho^2");

        RootType r = sqrt(root(s2->v()*(s2->alpha() - s1->rho())));         // damn square roots
        r *= 2;
        r += root(s1->s() + s1->v() + s2->alpha() - s1->rho());
    }
    rLog(rlARFunctionKernel, "  Stack size: %i", stack.size());
    if (stack.size() > 0) rLog(rlARFunctionKernel, "  Top: %lf", stack.top());
}

int
ARFunctionKernel::
sign_at(const Function& f, RootType r)
{
    FieldType v = value_at(f,r);
    if (v > 0)  return true;
    else        return false;
}

int
ARFunctionKernel::
sign_at_negative_infinity(const Function& f)
{
    FunctionForm f1 = f.form(), f2 = f.form2();
    const ARSimplex3D   *s1 = f.simplex(), 
                        *s2 = f.simplex2();
    int multiplier = 1;
    if (f1 < f2) { std::swap(f1, f2); std::swap(s1, s2); multiplier = -1; }
    
    AssertMsg(f1 != Function::lambda && f2 != Function::lambda, "Lambda not implemented yet");

    if (f1 == Function::phi && f2 == Function::phi)
    {
        if (s1->phi_const() == s2->phi_const()) return  0;
        if (s1->phi_const() > s2->phi_const())  return  1;        // multiplier must be 1
        else                                    return -1;
    }
    
    if (f1 == Function::phi && f2 == Function::rho)
        return -multiplier;

    if (f1 == Function::rho && f2 == Function::rho)
    {
        if (s1->alpha() == s2->alpha())         return  0;
        if (s1->alpha() > s2->alpha())          return  1;        // multiplier must be 1
        else                                    return -1;
    }

    AssertMsg(false, "The case analysis should be exhaustive in sign at -infinity");
    return false;
}
        
ARFunctionKernel::FieldType            
ARFunctionKernel::
value_at(const Function& f, RootType v)
{
    FunctionForm f1 = f.form(), f2 = f.form2();
    ARSimplex3D     *s1 = f.simplex(), 
                    *s2 = f.simplex2();

    AssertMsg(f2 == Function::none && s2 == 0, "Value_at knows only about functions themselves, not their differences");
    AssertMsg(f1 != Function::lambda, "Lambda not implemented yet");
    rLog(rlARFunctionKernelValue,    "Value_at: function = (%i, %p)", f1, s1);

    if (f1 == Function::phi)
        return v + root(s1->phi_const());

    if (f1 == Function::rho)
        return root(s1->alpha());
    
    AssertMsg(false, "The case analysis should be exhaustive in value_at");
    return 0;
}
