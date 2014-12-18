/*!
 * \file XYZTrajectory.h
 * \author Joseph R. Vella
 * \date 18 December 2014
 * \brief Declaration of XYZTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_XYZTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_XYZTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>

//! XYZ parser
/*!
 *  Implementation of the Trajectory for XYZ file format
 *
 *  \ingroup trajectories
 */
class XYZTrajectory : public Trajectory
    {
    public:
        //! default constructor
        XYZTrajectory() {};
        
        //! default destructor
        virtual ~XYZTrajectory() {};
        
        //! attach a file to be parsed
        void addFile(const std::string& f);
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        std::vector<std::string> m_files;           //!< List of files to parse
        
        //! internal method for reading a single XYZ file into a Frame
        boost::shared_ptr<Frame> readFromFile(std::ifstream& file);
    };

//! Python export for XYZTrajectory
void export_XYZTrajectory();

#endif //MDALYZER_TRAJECTORIES_XYZTRAJECTORY_H_
