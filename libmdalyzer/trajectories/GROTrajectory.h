/*!
 * \file GROTrajectory.h
 * \author Sang Beom Kim
 * \date 17 December 2014
 * \brief Declaration of GROTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_GROTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_GROTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>

//! GRO parser
/*!
 *  Implementation of the Trajectory for GRO file format
 *
 *  \ingroup trajectories
 */
class GROTrajectory : public Trajectory
    {
    public:
        //! default constructor
        GROTrajectory() {};
        
        //! default destructor
        virtual ~GROTrajectory() {};
        
        //! attach a file to be parsed
        void addFile(const std::string& f);
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        std::vector<std::string> m_files;           //!< List of files to parse
        
        //! internal method for reading a single GRO file into a Frame
        boost::shared_ptr<Frame> readFromFile(std::ifstream& file);
    };

//! Python export for GROTrajectory
void export_GROTrajectory();

#endif //MDALYZER_TRAJECTORIES_GROTRAJECTORY_H_
