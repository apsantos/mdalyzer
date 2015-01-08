/*!
 * \file Analyzer.h
 * \author Michael P. Howard
 * \brief Declaration of Analyzer structure
 */
#ifndef MDALYZER_ANALYZERS_ANALYZER_H_
#define MDALYZER_ANALYZERS_ANALYZER_H_

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

/*! \ingroup libmdalyzer
 * @{
 * \defgroup analyzers
 * \brief All implementations of the Analyzer
 * @}
 */

class Trajectory; // forward declaration

//! Analyze particle data from Molecular Dynamics simulations
/*!
 * An Analyzer evaluates a trajectory the data from a molecular simulation relevant to the property being measured
 * 
 * Analyzers are attached to trajectories; multiple may be performed on each trajectory.
 *
 * \ingroup analyzers
 * \sa Trajectory
 */
class Analyzer : boost::noncopyable
    {
    public:
        Analyzer(boost::shared_ptr<Trajectory> traj);
        virtual ~Analyzer() {};
        
        virtual void evaluate() = 0;
    protected:
        boost::shared_ptr<Trajectory> m_traj;   //!< Trajectory associate with analyzer
    };

//! Export Trajectory to Python module
void export_Analyzer();
#endif //MDALYZER_ANALYZERS_ANALYZER_H_
