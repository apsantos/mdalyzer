/*! \file DCDTrajectory.h
 *  \author Andrew P. Santos
 */
#ifndef MDALYZER_TRAJECTORY_DCDTRAJECTORY_H_
#define MDALYZER_TRAJECTORY_DCDTRAJECTORY_H_

#include "Trajectory.h"
#include "ReadDCD.h"
#include <fstream>
#include <cstdio>

/*! \class DCDTrajectory
 *  \brief DCD parser
 *
 *  Extension of Trajectory that parses DCD output regardless of endianism.  
 *  Note: "This software [ReadDCD.C,h] includes code developed by the Theoretical Biophysics Group 
 *  in the Beckman Institute for Advanced Science and Technology at the 
 *  University of Illinois at Urbana-Champaign."
 *
 *  \ingroup trajectory
 */
class DCDTrajectory : public Trajectory
    {
    public:
        DCDTrajectory(boost::shared_ptr<Trajectory> initial_traj, 
                     const std::string& fileName, double time_step, unsigned int dcd_freq);
        virtual ~DCDTrajectory() {};
        virtual void read();
        void readFromFile();
        void readHeader(FILE* fileptr);
        boost::shared_ptr<Frame> readTimeStep(FILE* fileptr);

    private:
        boost::shared_ptr<Trajectory> m_initial_traj; //!< initial trajectory with particle type and pos
        const std::string m_file;       //!< file name

        /// Variables used to call C ReadDCD.cc functions
        int m_n_dcdparticles_c;         //!< num of particles in the dcd file
        int *m_freeparticles_c;         //!< particles that change position over time
        int m_reverse_endian_c;         //!< Flag for reversing binary endiaism; i.e. how do you crack you eggs?
        int m_n_fixed_c;                //!< num of fixed particles (i.e. zeolite)
        int m_charmm_flags_c;           //!< flags for what information is available
        int m_first_dcdread;            //!< is the dcd file opened?
        
        double m_time_step;             //!< user defined dcd timestep 
        double m_dcd_freq;              //!< user defined number of steps between coordinates
        unsigned int m_n_dcdparticles;   
        unsigned int m_n_frames;
        unsigned int m_frame_start;     //!< starting frame for reading
        
    };

//! Python export for DCDTrajectory
void export_DCDTrajectory();

#endif // MDALYZER_TRAJECTORY_DCDTRAJECTORY_H_
