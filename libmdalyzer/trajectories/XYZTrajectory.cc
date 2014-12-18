/*! 
 * \file XYZTrajectory.cc
 * \author Joseph R. Vella
 * \date 18 December 2014
 * \brief Reads XYZ files
 */
#include "XYZTrajectory.h"

#include <boost/python.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

/*!
 * Loops over all attached files and calls readFromFile on them.
 * Each file may contain multiple frames inside
 */
void XYZTrajectory::read()
    {
    for (unsigned int cur_f = 0; cur_f < m_files.size(); ++cur_f)
        {
        
        // open XYZ file
        std::ifstream file(m_files[cur_f].c_str());
        if (!file.good())
        {
        throw std::runtime_error("XYZTrajectory: cannot find XYZ file " + m_files[cur_f]);
        }
        
        // read frames until the end
        while ( !file.eof() )
            {
            m_frames.push_back(readFromFile(file));
            }
        
        file.close();
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
void XYZTrajectory::addFile(const std::string& f)
    {
    m_must_read_from_file = true;
    m_files.push_back(f); // error check this later
    }

/*! 
 * \param f file to parse
 * \returns shared pointer to the newly read Frame
 *
 * A Frame is required to have a timestep and a box set. If either of these does not exist, an exception is thrown.
 * The simulation box may be triclinic, so we save the tilt factors. If the image properties are supplied, we unwrap the
 * particle positions.
 */
boost::shared_ptr<Frame> XYZTrajectory::readFromFile(std::ifstream& file)
    {
        
    boost::shared_ptr<Frame> cur_frame; //default initialization is null ptr
    
    std::string line, first_word;   // string for reading line and first-word
    std::string dummy_str;
    std::string name_i;
    
    double time_step = 0.0;         // time step for this frame
    TriclinicBox box;               // box information
    Vector3<double> pos_i;          // position
    std::vector<Vector3<double> > positions;
    Vector3<double> vel_i;          // velocity
    std::vector< Vector3<double> > velocities;
    unsigned int n_particles = 0;   // number of particles
    std::vector<std::string> names; // types of atoms
    
    // checkers
    bool has_box = false;
    bool has_time_step = false;
    bool has_velocity = false;
    
    // the first line contains number of atoms
    getline(file, line);
    std::istringstream iss_line(line);
    
    iss_line >> n_particles;
    
    
    // second line is a comment line to be ignored
    getline(file, line);
    iss_line.str(line);
    
    // read the types and coordinates
    for (unsigned int i=0; i<n_particles; ++i)
        {
        
        getline(file, line);
        iss_line.str(line);
        
        iss_line >> dummy_str >> name_i >> dummy_str
                 >> pos_i.x >> pos_i.y >> pos_i.z;
            
        positions.push_back(pos_i);
        names.push_back(name_i);    
        }
    
    
        
    /* assign the information to the cur_frame pointer
     * - check if box and time step information is present
     */
    
//    if ( !has_time_step )
//        throw std::runtime_error("GROTrajectory: frames must have time set");
//    else if ( !has_box )
//        throw std::runtime_error("GROTrajectory: frame requires a box");
        
    // set cur_frame with n_particles and save information
    cur_frame = boost::shared_ptr<Frame>( new Frame(n_particles) );
    cur_frame->setPositions(positions);
//    cur_frame->setTime(time_step);
//    cur_frame->setBox(box);
	}

void export_XYZTrajectory()
    {
    using namespace boost::python;
    class_<XYZTrajectory, boost::shared_ptr<XYZTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("XYZTrajectory", init< >())
    .def("addFile", &XYZTrajectory::addFile);
    }
