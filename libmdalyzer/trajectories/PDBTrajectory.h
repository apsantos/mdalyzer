/*!
 * \file PDBTrajectory.h
 * \author Sang Beom Kim
 * \date 17 December 2014
 * \brief Declaration of PDBTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_PDBTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_PDBTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>

//! PDB parser
/*!
 *  Implementation of the Trajectory for PDB file format
 *
 *  \ingroup trajectories
 */
class PDBTrajectory : public Trajectory
    {
    public:
        //! default constructor
        PDBTrajectory() {};
        
        //! default destructor
        virtual ~PDBTrajectory() {};
        
        //! attach a file to be parsed
        void addFile(const std::string& f);
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        std::vector<std::string> m_files;           //!< List of files to parse
        
        //! internal method for reading a single PDB file into a Frame
        boost::shared_ptr<Frame> readFromFile(std::ifstream& file);
    };

//! Python export for PDBTrajectory
void export_PDBTrajectory();

#endif //MDALYZER_TRAJECTORIES_PDBTRAJECTORY_H_
