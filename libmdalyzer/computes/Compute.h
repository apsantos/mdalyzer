#ifndef MDALYZER_COMPUTES_COMPUTE_H_
#define MDALYZER_COMPUTES_COMPUTE_H_

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

class Trajectory; // forward declaration

class Compute : boost::noncopyable
    {
    public:
        Compute(boost::shared_ptr<Trajectory> traj);
        virtual ~Compute() {};
        
        virtual void evaluate() = 0;
    protected:
        boost::shared_ptr<Trajectory> m_traj;   //!< Trajectory associate with compute
    };

void export_Compute();
#endif //MDALYZER_COMPUTES_COMPUTE_H_
