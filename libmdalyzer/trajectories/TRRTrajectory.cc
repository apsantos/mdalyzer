/*! 
 * \file TRRTrajectory.cc
 * \author Sang Beom Kim
 * \date 17 December 2014
 * \brief Reads TRR files
 */
#include "TRRTrajectory.h"

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

/*!
 * Loops over all attached files and calls readFromFile on them.
 * Each file may contain multiple frames inside
 */
void TRRTrajectory::read()
    {
    for (unsigned int cur_f = 0; cur_f < m_files.size(); ++cur_f)
        {
        
        // open TRR file
        XDRFILE* trjFileXDR_;
        trjFileXDR_ = xdrfile_open(m_files[cur_f].c_str(), "r");
        if ( trjFileXDR_ == NULL )
            throw std::runtime_error("TRRTrajectory: cannot find TRR file " + m_files[cur_f]);
        
        int natoms_;
        char* fn_ = new char[m_files[cur_f].size() + 1];
        std::copy(m_files[cur_f].begin(), m_files[cur_f].end(), fn_);
        fn_[m_files[cur_f].size()] = '\0';
        if (read_trr_natoms(fn_, &natoms_) != 0)
            {
            throw std::runtime_error("TRRTrajectory: cannot read \
            number of atoms " + m_files[cur_f]);
            }
        
        end_file = 0;
        
        // read frames until the end
        while ( end_file == 0 )
            {
            m_frames.push_back(readFromFile(trjFileXDR_, natoms_));
            }
        
        xdrfile_close(trjFileXDR_);
        }
    m_must_read_from_file = false;
    }

/*!
 * \param f file name to attach
 *
 * Any time a new file is attached, the Trajectory must be re-read from file. This could be handled in a smart way
 * flushing the read file list so that only newly added files are read, and not everything. This should be considered
 * in read() in the future.
 *
 * \note error checking for duplicates is currently not enabled, but we will implement this soon.
 */
void TRRTrajectory::addFile(const std::string& f)
    {
    m_must_read_from_file = true;
    m_files.push_back(f); // error check this later
    }

/*! 
 * \param f file to parse, natoms_ number of atoms
 * \returns shared pointer to the newly read Frame
 *
 * A Frame is required to have a timestep and a box set. If either of these does not exist, an exception is thrown.
 * The simulation box may be triclinic, so we save the tilt factors. If the image properties are supplied, we unwrap the
 * particle positions.
 */
boost::shared_ptr<Frame> TRRTrajectory::readFromFile(XDRFILE* trjFileXDR_, int& natoms_)
    {
        
    boost::shared_ptr<Frame> cur_frame; //default initialization is null ptr
    
    float time_step;                // time step for this frame
    int step_number;
    matrix box_i;
    TriclinicBox box;               // box information
    rvec* pos_i;                    // position
    std::vector<Vector3<double> > positions(natoms_,
                                        Vector3<double>(0.,0.,0.));
    rvec* vel_i;                    // velocity
    std::vector< Vector3<double> > velocities;
    rvec* for_i;                    // force
    int n_particles = natoms_;      // number of particles

    float lambda_;

    pos_i = (rvec *)calloc(natoms_, sizeof(pos_i[0]));
    vel_i = (rvec *)calloc(natoms_, sizeof(vel_i[0]));
    for_i = (rvec *)calloc(natoms_, sizeof(for_i[0]));
    
    if ( 0 != read_trr(trjFileXDR_, n_particles, &step_number, &time_step,
                       &lambda_, box_i, pos_i, vel_i, for_i) )
        {
        end_file = 1;
        }

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
    
    // process velocities
    for (int i = 0; i < natoms_; ++i)
        {
        velocities[i].x = vel_i[i][0];
        velocities[i].y = vel_i[i][1];
        velocities[i].z = vel_i[i][2];
        }
    free(pos_i);
    
        
    // set cur_frame with n_particles and save information
    cur_frame = boost::shared_ptr<Frame>( new Frame(n_particles) );
    cur_frame->setPositions(positions);
    cur_frame->setTime(double(time_step));
    cur_frame->setBox(box);
    cur_frame->setVelocities(velocities);
    
    return cur_frame;
    }

void export_TRRTrajectory()
    {
    using namespace boost::python;
    class_<TRRTrajectory, boost::shared_ptr<TRRTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("TRRTrajectory", init< >())
    .def("addFile", &TRRTrajectory::addFile);
    }
