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
 *  Implementation of the Trajectory for XTC file format
 *
 *  \inXTCup trajectories
 */
class XTCTrajectory : public Trajectory
    {
    public:
        //! default constructor
        XTCTrajectory() {};
        
        //! default destructor
        virtual ~XTCTrajectory() {};
        
        //! attach a file to be parsed
        void addFile(const std::string& f);
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        std::vector<std::string> m_files;           //!< List of files to parse
        int end_file;                               //!< Marking the end of each file
        
        //! internal method for reading a single XTC file into a Frame
        boost::shared_ptr<Frame> readFromFile(XDRFILE* trjFileXDR_, int& natoms_);
    };

//! Python export for XTCTrajectory
void export_XTCTrajectory();

#endif //MDALYZER_TRAJECTORIES_XTCTRAJECTORY_H_
