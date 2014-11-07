#ifndef __COMPUTE_H__
#define __COMPUTE_H__

class Compute
    {
    public:
        Compute();
        ~Compute();
        
        bool shouldCompute() const;
        void evaluate();
    };

#endif //__COMPUTE_H__
