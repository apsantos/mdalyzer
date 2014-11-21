#ifndef MDALYZER_COMPUTES_COMPUTE_H_
#define MDALYZER_COMPUTES_COMPUTE_H_

class Compute
    {
    public:
        Compute();
        ~Compute();
        
        void setup();
        void evaluate();
        void cleanup();
    };

#endif //MDALYZER_COMPUTES_COMPUTE_H_
