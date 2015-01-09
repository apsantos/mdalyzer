/*! 
 * \file XTCTrajectory.cc
 * \author Sang Beom Kim
 * \date 17 December 2014
 * \brief Reads XTC files
 */
#include "XTCTrajectory.h"

#include <boost/python.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>
extern "C" {
#include "xdrfile.h"
#include "xdrfile_xtc.h"
#include "xdrfile_trr.h"
}

#define PI 3.14159265

/*
 * Initial Constructor
 */
XTCTrajectory::XTCTrajectory()
	{
	}

/*!
 * Loops over all attached files and calls readFromFile on them.
 * Each file may contain multiple frames inside
 */
void XTCTrajectory::read()
    {
    for (unsigned int cur_f = 0; cur_f < m_files.size(); ++cur_f)
        {
        
        // open XTC file
        XDRFILE* trjFileXDR_;
        trjFileXDR_ = xdrfile_open(m_files[cur_f].c_str(), "r");
        if ( trjFileXDR_ == NULL )
            throw std::runtime_error("XTCTrajectory: cannot find XTC file " + m_files[cur_f]);
        
        int natoms_;
        char* fn_ = new char[m_files[cur_f].size() + 1];
        std::copy(m_files[cur_f].begin(), m_files[cur_f].end(), fn_);
        fn_[m_files[cur_f].size()] = '\0';
        if (read_xtc_natoms(fn_, &natoms_) != 0)
            {
            throw std::runtime_error("XTCTrajectory: cannot read \
            number of atoms " + m_files[cur_f]);
            }
        
        end_file = 0;
        
        // read frames until the end
        while ( end_file == 0 )
            {
            end_file = readFromFile(trjFileXDR_, natoms_);
            }
        
        xdrfile_close(trjFileXDR_);
        }
    m_must_read_from_file = false;
    }

/*! 
 * \param trjFileXDR_ file to parse, natoms_ number of atoms
 * \returns shared pointer to the newly read Frame
 *
 * A Frame is required to have a timestep and a box set. If either of these does not exist, an exception is thrown.
 * The simulation box may be triclinic, so we save the tilt factors. If the image properties are supplied, we unwrap the
 * particle positions.
 */
int XTCTrajectory::readFromFile(XDRFILE* trjFileXDR_, int& natoms_)
    {
        
    // instantiate the Frame
    boost::shared_ptr<Frame> cur_frame( new Frame(natoms_) );
        
    float time_step;                // time step for this frame
    int step_number;
    matrix box_i;
    TriclinicBox box;               // box information
    rvec* pos_i;          // position
    std::vector<Vector3<double> > positions(natoms_,
                                        Vector3<double>(0.,0.,0.));
    int n_particles = natoms_;   // number of particles


    pos_i = (rvec *)calloc(natoms_, sizeof(pos_i[0]));
    float precision = 1000.0;
    
    if ( 0 != read_xtc(trjFileXDR_, n_particles, &step_number, &time_step,
                       box_i, pos_i, &precision) )
        {
        return 1;
        
        }
    else
        {

        // process trilinic box
        Vector3<double> length(box_i[0][0],box_i[1][1],box_i[2][2]);
        Vector3<double> tilt(box_i[0][1],box_i[0][2],box_i[1][2]);
        box = TriclinicBox(length, tilt);

        // process coordinates
        for (int i = 0; i < natoms_; ++i)
            {
            positions[i].x = pos_i[i][0];
            positions[i].y = pos_i[i][1];
            positions[i].z = pos_i[i][2];
            }
        free(pos_i);
        
            
        // set cur_frame with n_particles and save information
        cur_frame = boost::shared_ptr<Frame>( new Frame(n_particles) );
        cur_frame->setPositions(positions);
        cur_frame->setTime(double(time_step));
        cur_frame->setBox(box);
        
        m_frames.push_back(cur_frame);
        
        return 0;
        }
    }

void export_XTCTrajectory()
    {
    using namespace boost::python;
    class_<XTCTrajectory, boost::shared_ptr<XTCTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("XTCTrajectory");
    }
