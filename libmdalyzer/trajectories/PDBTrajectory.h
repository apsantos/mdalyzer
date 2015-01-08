/*!
 * \file PDBTrajectory.h
 * \author Sang Beom Kim
 * \author Michael P. Howard
 * \date 30 December 2014
 * \brief Declaration of PDBTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_PDBTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_PDBTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>

//! PDB parser
/*!
 * Declaration of the Trajectory for the PDB file format, which is extensively documented elsewhere. Multiple frames
 * can be contained in the same file provided they are partitioned by MODEL and ENDMDL. A single CRYST1 is assumed
 * for a single PDB file. The time of each Frame is inferred from its MODEL integer id, which is then optionally
 * scaled by a timestep between each Frame. PDB is a fixed column file format, so whitespace must be properly obeyed.
 *
 * \ingroup trajectories
 */
class PDBTrajectory : public Trajectory
    {
    public:
        //! constructor with timestep and frame terminator
        PDBTrajectory(double timestep);
        
        //! default destructor
        virtual ~PDBTrajectory() {};
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        double m_pdb_timestep;      //!< spacing between pdb frames
        
        //! internal method for reading a single PDB file into a Frame
        void readFromFile(std::ifstream& file);
    };

//! Python export for PDBTrajectory
void export_PDBTrajectory();

#endif //MDALYZER_TRAJECTORIES_PDBTRAJECTORY_H_
