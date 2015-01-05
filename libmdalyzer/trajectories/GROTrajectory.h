/*!
 * \file GROTrajectory.h
 * \author Sang Beom Kim
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Declaration of GROTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_GROTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_GROTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>

//! GRO parser
/*!
 * Declaration of the Trajectory for the arbitrary precision GRO file format:
 *
 * comment line t= <time>
 *      number of particles
 * gmx topology[char 1-10] type[char 11-15] particle #[char 16-20] r.x r.y r.z v.x v.y v.z
 * ...
 * v1(x) v2(y) v3(z) v1(y) v1(z) v2(x) v2(z) v3(x) v3(z)
 *
 * where r is the position vector and v is the velocity vector. Positions must have fixed column width n+5 and
 * velocities must have fixed width n+6, where n is the decimal precision (default = 3). The reason we use this
 * rigid reading format is that GMX will not include white spaces if the numbers are too large, and this in turn
 * will cause stringstream reads to fail. Only the first three components are required to define an orthorhombic
 * box, and additional components will set a triclinic box.
 *
 *  \ingroup trajectories
 */
class GROTrajectory : public Trajectory
    {
    public:
        //! default constructor
        GROTrajectory();
        
        //! constructor with precision
        GROTrajectory(unsigned int precision);
        
        //! default destructor
        virtual ~GROTrajectory() {};
        
        //! reads all attached files into Frames
        virtual void read();
    private:
        std::string m_gro_parse_string; //!< formatting string for scanf to read gro line
        unsigned int m_gro_line_length; //!< minimum character length for a line
        
        //! reads a single GRO file stream into multiple Frames
        void readFromFile(std::ifstream& file);
    };

//! Python export for GROTrajectory
void export_GROTrajectory();

#endif //MDALYZER_TRAJECTORIES_GROTRAJECTORY_H_
