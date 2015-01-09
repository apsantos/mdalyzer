/*!
 * \file XTCTrajectory.h
 * \author Sang Beom Kim
 * \date 18 December 2014
 * \brief Declaration of XTCTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_XTCTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_XTCTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>
extern "C" {
#include "xdrfile.h"
#include "xdrfile_xtc.h"
#include "xdrfile_trr.h"
}

//! XTC parser
/*!
 * Declaration of the Trajectory for the arbitrary precision XTC file format:
 *
 * XDR library is used to read all information (position, box, timestep) at once for each frame.
 *
 *  \ingroup trajectories
 */
class XTCTrajectory : public Trajectory
    {
    public:
        //! default constructor
        XTCTrajectory();
        
        //! default destructor
        virtual ~XTCTrajectory() {};
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        int end_file;                               //!< Marking the end of each file
        
        //! internal method for reading a single XTC file into a Frame
        int readFromFile(XDRFILE* trjFileXDR_, int& natoms_);
    };

//! Python export for XTCTrajectory
void export_XTCTrajectory();

#endif //MDALYZER_TRAJECTORIES_XTCTRAJECTORY_H_
