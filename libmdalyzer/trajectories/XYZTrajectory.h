/*!
 * \file XYZTrajectory.h
 * \author Joseph R. Vella
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Declaration of XYZTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_XYZTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_XYZTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>

//! XYZ parser
/*!
 * The XYZ file format, although not standardized, is generally interpreted as follows:
 <code>
 number of atoms
 comment... t=<time> (optional)
 type x   y   z
 ...
 </code>
 *
 * Multiple such frames may be defined in each file. If the time is supplied in the comment line in one frame,
 * it must be set for all frames in the trajectory. Otherwise, the time will be defined using the frame id in the
 * order that the files are read. The user must take care to properly time order the frames during input if times
 * are not explicitly set.
 *
 * \ingroup trajectories
 */
class XYZTrajectory : public Trajectory
    {
    public:
        //! default constructor
        XYZTrajectory();
        
        //! default destructor
        virtual ~XYZTrajectory() {};
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        bool m_parse_xyz_timestep;          //!< Flag to parse XYZ timestep
        
        //! internal method for reading a single XYZ file into a Frame
        void readFromFile(std::ifstream& file);
        
    };

//! Python export for XYZTrajectory
void export_XYZTrajectory();

#endif //MDALYZER_TRAJECTORIES_XYZTRAJECTORY_H_
