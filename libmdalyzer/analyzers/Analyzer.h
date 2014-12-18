#ifndef MDALYZER_ANALYZERS_ANALYZER_H_
#define MDALYZER_ANALYZERS_ANALYZER_H_

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

class Trajectory; // forward declaration

class Analyzer : boost::noncopyable
    {
    public:
        Analyzer(boost::shared_ptr<Trajectory> traj);
        virtual ~Analyzer() {};
        
        virtual void evaluate() = 0;
    protected:
        boost::shared_ptr<Trajectory> m_traj;   //!< Trajectory associate with analyzer
    };

void export_Analyzer();
#endif //MDALYZER_ANALYZERS_ANALYZER_H_
