/*!
 * \file TRRTrajectory.h
 * \author Sang Beom Kim
 * \date 18 December 2014
 * \brief Declaration of TRRTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_TRRTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_TRRTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>
extern "C" {
#include "xdrfile.h"
#include "xdrfile_xtc.h"
#include "xdrfile_trr.h"
}

//! TRR parser
/*!
 * Declaration of the Trajectory for the arbitrary precision TRR file format:
 *
 * XDR library is used to read all information (position, velocity, box, timestep) at once for each frame.
 *
 *  \ingroup trajectories
 */
class TRRTrajectory : public Trajectory
    {
    public:
        //! default constructor
        TRRTrajectory();
        
        //! default destructor
        virtual ~TRRTrajectory() {};
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        int end_file;                               //!< Marking the end of each file
        
        //! internal method for reading a single TRR file into a Frame
        int readFromFile(XDRFILE* trjFileXDR_, int& natoms_);
    };

//! Python export for TRRTrajectory
void export_TRRTrajectory();

#endif //MDALYZER_TRAJECTORIES_TRRTRAJECTORY_H_
