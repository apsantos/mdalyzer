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
        DCDTrajectory(const std::string& fileName);
        virtual ~DCDTrajectory() {};
        virtual void read();
        virtual void readHeader(FILE* fileptr);
        virtual void readTimeStep(FILE* fileptr, std::vector< Vector3<double> >& positions);
        //virtual void readHeader(const std::ifstream& file, bool pdb_info);
        //virtual void readPDBinfo(const std::ifstream file);

    private:
        // variables used to call C ReadDCD.cc functions
        int m_n_dcdparticles_c;
        int m_n_frames_c;
        int m_frame_start_c;
        int m_frame_skip_c;
        int *m_freeparticles_c;
        int m_reverse_endian_c;
        int m_n_fixed_c;
        int m_charmm_flags_c;
        int m_first_dcdread;
        
        double m_time_step;
        unsigned int m_n_dcdparticles;
        unsigned int m_n_frames;
        unsigned int m_frame_skip;
        const std::string m_file; // file name
        
    };

void export_DCDTrajectory();

#endif // MDALYZER_TRAJECTORY_DCDTRAJECTORY_H_
