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
        DCDTrajectory(const std::string& fileName, const unsigned int& frequency);
        virtual ~DCDTrajectory() {};
        virtual void read();
        virtual int readHeader(FILE* fileptr);
        //virtual void readHeader(const std::ifstream& file, bool pdb_info);
        //virtual void readPDBinfo(const std::ifstream file);

    private:
        const std::string m_file;
        const unsigned int m_frequency;  // DCD timestep output frequency
        
    };

void export_DCDTrajectory();

#endif // MDALYZER_TRAJECTORY_DCDTRAJECTORY_H_
