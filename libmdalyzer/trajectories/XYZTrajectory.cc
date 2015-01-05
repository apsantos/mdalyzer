/*! 
 * \file XYZTrajectory.cc
 * \author Joseph R. Vella
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Reads XYZ files
 */
#include "XYZTrajectory.h"

#include <boost/algorithm/string.hpp>
#include <boost/python.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

/*! By default, an XYZTrajectory always attempts to parse the timestep from the comment line */
XYZTrajectory::XYZTrajectory()
    : m_parse_xyz_timestep(true)
    {
    }

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
        readFromFile(file);
        file.close();
        }
    m_must_read_from_file = false;
    }

/*! 
 * \param f file to parse
 * \returns shared pointer to the newly read Frame
 *
 * A Frame is required to have a timestep and a box set. If either of these does not exist, an exception is thrown.
 * The simulation box may be triclinic, so we save the tilt factors. If the image properties are supplied, we unwrap the
 * particle positions.
 */
void XYZTrajectory::readFromFile(std::ifstream& file)
    {
    // string container for parsing line by line
    std::string line;
    
    while (getline(file, line))
        {
        // skip over empty lines until particle number line
        if (line.empty())
            continue;
            
        // first content line is # of particles
        std::istringstream line_parser(line);
        unsigned int n_particles(0);
        if (!(line_parser >> n_particles))
            {
            throw std::runtime_error("XYZTrajectory: first line must be number of particles in frame");
            }
            
        // instantiate the Frame
        boost::shared_ptr<Frame> cur_frame( new Frame(n_particles) );
        
        double time_step(0.0);
        if (getline(file,line))
            {
            // if we have parsed other frames, then try to parse
            if (m_parse_xyz_timestep)
                {
                // found a time set, try to parse a time out
                int found_time = line.find("t=");
                if (found_time >= 0 && (found_time+2) < (int)line.length())
                    {
                    line_parser.str(line.substr(found_time+2));
                    line_parser.clear();
                
                    if ( !(line_parser >> time_step) )
                        {
                        // failed to read the timestep as a float, can't use it
                        m_parse_xyz_timestep = false;
                        
                        // if we already pushed back frames with the time, need to throw an error
                        if (m_frames.size() > 0)
                            {
                            throw std::runtime_error("XYZTrajectory: all frames must have time in comment if one does");
                            }
                        }
                    }
                else
                    {
                    // there is no time step switch to auto naming by frame id
                    m_parse_xyz_timestep = false;
                    
                    // if we already pushed back frames with the time, need to throw an error
                    if (m_frames.size() > 0)
                        {
                        throw std::runtime_error("XYZTrajectory: all frames must have time in comment if one does");
                        }
                    }
                }
                
            // if parsing is switched off, use the frame id
            if (!m_parse_xyz_timestep)
                {
                time_step = (double)m_frames.size();
                }
            cur_frame->setTime(time_step);
            }
        else
            {
            throw std::runtime_error("XYZTrajectory: a comment line must follow the particle count");
            }
            
        // loop on particles now
        unsigned int cur_particle(0);
        std::string name_i;
        Vector3<double> pos_i;
        
        while (cur_particle < cur_frame->getN() && getline(file,line))
            {
            line_parser.str(line);
            line_parser.clear();
            
            if (line_parser >> name_i >> pos_i.x >> pos_i.y >> pos_i.z)
                {
                boost::algorithm::trim(name_i);
                if (name_i.length() > 0)
                    {
                    cur_frame->setName(cur_particle, name_i);
                    }
                cur_frame->setPosition(cur_particle, pos_i);
                }
            else
                {
                throw std::runtime_error("XYZTrajectory: a particle line is type, x, y, z");
                }
            ++cur_particle;
            }
            
        m_frames.push_back(cur_frame);
        }
	}

void export_XYZTrajectory()
    {
    using namespace boost::python;
    class_<XYZTrajectory, boost::shared_ptr<XYZTrajectory>, bases<Trajectory>, boost::noncopyable>("XYZTrajectory");
    }
