/*! 
 * \file PDBTrajectory.cc
 * \author Sang Beom Kim
 * \date 17 December 2014
 * \brief Reads PDB files
 */
#include "PDBTrajectory.h"

#include <boost/python.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>

#define PI 3.14159265

/*!
 * Loops over all attached files and calls readFromFile on them.
 * Each file may contain multiple frames inside
 */
void PDBTrajectory::read()
    {
    for (unsigned int cur_f = 0; cur_f < m_files.size(); ++cur_f)
        {
        
        // open PDB file
        std::ifstream file(m_files[cur_f].c_str());
        if (!file.good())
        {
        throw std::runtime_error("PDBTrajectory: cannot find PDB file " + m_files[cur_f]);
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
void PDBTrajectory::addFile(const std::string& f)
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
boost::shared_ptr<Frame> PDBTrajectory::readFromFile(std::ifstream& file)
    {
        
    boost::shared_ptr<Frame> cur_frame; //default initialization is null ptr
    
    std::string line, first_word;   // string for reading line and first-word
    std::string dummy_str;
    std::string name_i;
    
    double time_step = 0.0;         // time step for this frame
    TriclinicBox box;               // box information
    Vector3<double> pos_i;
    std::vector<Vector3<double> > positions;
    unsigned int n_particles = 0;   // number of particles
    std::vector<std::string> names; // types of atoms
    
    // checkers
    bool has_box = false;
    bool has_time_step = false;
    
    // read until the end of one frame
    while ( true )
        {
        // read a line
        getline(file, line);
        std::istringstream iss_line(line);
        
        // check the first word
        iss_line >> first_word;
        
        // ENDMDL marks the end of a frame
        if ( first_word.compare("ENDMDL") != 0 )
            break;
        
        if ( first_word.compare("TITLE") == 0 )
            {
            // find the time step
            while ( first_word.compare("t=") != 0 )
                iss_line >> first_word;
                
            iss_line >> time_step;
            
            has_time_step = true;
            }
        
        // box information
        else if ( first_word.compare("CRYST1") == 0 )
            {
            
            // read lattice constants
        
            // read a, b, c (in angstrom)
            double t_a, t_b, t_c = 0.;
            file >> t_a;
            file >> t_b;
            file >> t_c;
            
            // read angles (alpha, beta, gamma), in degrees
            double angle_alpha, angle_beta, angle_gamma = 0.;
            file >> angle_alpha;
            file >> angle_beta;
            file >> angle_gamma;
            
            // construct the simulation box
            Vector3<double> length(0.,0.,0.);
            Vector3<double> tilt(0.,0.,0.);
            
            // compute box length and tilt factors
            length.x = t_a;
            tilt.x = t_b * cos(angle_gamma * PI / 180.0);
            tilt.y = t_c * cos(angle_beta * PI / 180.0);
            length.y = pow( (t_b*t_b - tilt.x*tilt.x), 0.5);
            tilt.z = ( ( length.y * length.z * cos(angle_alpha * PI / 180.0) ) - \
                        ( tilt.x * tilt.y ) ) / length.y;
            length.z = pow( (t_c*t_c - tilt.y*tilt.y - tilt.z*tilt.z), 0.5);
            
            // assign box based on the length and tilt
            box = TriclinicBox(length,tilt);

            has_box = true;
            }
            
        // position
        else if ( first_word.compare("ATOM") == 0 )
            {
            // read until the coordinates
            iss_line >> dummy_str >> name_i;
            for (int ds=0; ds<3; ++ds)
                iss_line >> dummy_str;
            
            iss_line >> pos_i.x >> pos_i.y >> pos_i.z;
            
            positions.push_back(pos_i);
            names.push_back(name_i);
            ++n_particles;
            }
        }
        
        
    /* assign the information to the cur_frame pointer
     * - check if box and time step information is present
     */
    
    if ( !has_time_step )
        throw std::runtime_error("PDBTrajectory: frames must have time set");
    else if ( !has_box )
        throw std::runtime_error("PDBTrajectory: frame requires a box");
        
    // set cur_frame with n_particles and save information
    cur_frame = boost::shared_ptr<Frame>( new Frame(n_particles) );
    cur_frame->setPositions(positions);
    cur_frame->setNames(names);
    cur_frame->setTime(time_step);
    cur_frame->setBox(box);
    
    return cur_frame;
    }

void export_PDBTrajectory()
    {
    using namespace boost::python;
    class_<PDBTrajectory, boost::shared_ptr<PDBTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("PDBTrajectory", init< >())
    .def("addFile", &PDBTrajectory::addFile);
    }
