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
 *  Implementation of the Trajectory for TRR file format
 *
 *  \inTRRup trajectories
 */
class TRRTrajectory : public Trajectory
    {
    public:
        //! default constructor
        TRRTrajectory() {};
        
        //! default destructor
        virtual ~TRRTrajectory() {};
        
        //! attach a file to be parsed
        void addFile(const std::string& f);
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        std::vector<std::string> m_files;           //!< List of files to parse
        int end_file;                               //!< Marking the end of each file
        
        //! internal method for reading a single TRR file into a Frame
        boost::shared_ptr<Frame> readFromFile(XDRFILE* trjFileXDR_, int& natoms_);
    };

//! Python export for TRRTrajectory
void export_TRRTrajectory();

#endif //MDALYZER_TRAJECTORIES_TRRTRAJECTORY_H_
