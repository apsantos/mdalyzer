/*! 
 * \file GROTrajectory.cc
 * \author Sang Beom Kim
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Implementation of GROTrajectory reader
 */
#include "GROTrajectory.h"

#include <boost/algorithm/string.hpp>
#include <boost/python.hpp>
#include <fstream>
#include <sstream>

GROTrajectory::GROTrajectory()
    : m_n_pos_chars(8), m_n_vel_chars(9)
    {
    }

/*!
 * \param precision number of decimal places in the positions
 */
GROTrajectory::GROTrajectory(unsigned int precision)
    : m_n_pos_chars(precision+5), m_n_vel_chars(precision+6)
    {
    }

/*!
 * Opens and loops over all attached files and calls readFromFile on them.
 * Each file may contain multiple frames inside
 */
void GROTrajectory::read()
    {
    for (unsigned int cur_f = 0; cur_f < m_files.size(); ++cur_f)
        {
        // open GRO file
        std::ifstream file(m_files[cur_f].c_str());
        if (!file.good())
            {
            throw std::runtime_error("GROTrajectory: cannot find GRO file " + m_files[cur_f]);
            }
        readFromFile(file);
        file.close();
        }
    m_must_read_from_file = false;
    }

/*! 
 * \param file string stream of current file to parse
 *
 * All file lines are looped over, ignoring white space, until a comment line is found that *must* contain
 * the timestep (t= <time>).
 */
void GROTrajectory::readFromFile(std::ifstream& file)
    {
    // string container for parsing line by line
    std::string line;
    
    while (getline(file, line))
        {
        // skip over empty lines until we find a comment line
        if (line.empty())
            continue;
            
        std::istringstream line_parser;
        
        // extract time step from comment line
        double time_step(0.0);
        int found_time = line.find("t=");
        if (found_time >= 0 && (found_time+2) < (int)line.length())
            {
            line_parser.str(line.substr(found_time+2));
            line_parser.clear();
            if (!(line_parser >> time_step))
                {
                throw std::runtime_error("GROTrajectory: time step must follow t=");
                }
            }
        else
            {
            throw std::runtime_error("GROTrajectory: time step is required in comment line");
            }
            
        // extract the number of atoms and construct the frame
        boost::shared_ptr<Frame> cur_frame;
        if (getline(file,line))
            {
            line_parser.str(line);
            line_parser.clear();
            unsigned int n_particles(0);
            if (line_parser >> n_particles)
                {
                cur_frame = boost::shared_ptr<Frame>( new Frame(n_particles) );
                cur_frame->setTime(time_step);
                }
            else
                {
                throw std::runtime_error("GROTrajectory: number of particles must be set");
                }
            }
        else
            {
            throw std::runtime_error("GROTrajectory: number of particles must be set");
            }
            
        // loop on particles now
        unsigned int cur_particle(0);
        Vector3<double> pos_i, vel_i;
        
        while (cur_particle < cur_frame->getN() && getline(file,line))
            {
            if (line.length() < 20)
                {
                throw std::runtime_error("GROTrajectory: particle lines are at least 20 characters long to set top");
                }
                
            // name is 5 char long and starts at position 10
            std::string name = line.substr(10,5);
            boost::algorithm::trim(name); // white space is not meaningful
            
            // particle id is 5 char long and starts at position 15
            unsigned int particle_id = readSubstring<unsigned int>(line, line_parser, 15, 5,
                                       "GROTrajectory: atom number not set for particle line " + cur_particle);     
            --particle_id; // runs 1 to N, so must subtract one
            
            // set the name now that the particle id is known
            if(name.length() > 0)
                {
                cur_frame->setName(cur_particle, name);
                }
        
            // now we have to snag the positions and velocities one by one
            unsigned int cur_read_pos = 20;
            // x position
            pos_i.x = readSubstring<double>(line,
                                            line_parser,
                                            cur_read_pos,
                                            m_n_pos_chars,
                                            "GROTrajectory: missing x position");
            cur_read_pos += m_n_pos_chars;
            
            // y position
            pos_i.y = readSubstring<double>(line,
                                            line_parser,
                                            cur_read_pos,
                                            m_n_pos_chars,
                                            "GROTrajectory: missing y position");
            cur_read_pos += m_n_pos_chars;
                
            // z position
            pos_i.z = readSubstring<double>(line,
                                            line_parser,
                                            cur_read_pos,
                                            m_n_pos_chars,
                                            "GROTrajectory: missing z position");
            cur_read_pos += m_n_pos_chars;
            
            // x velocity
            vel_i.x = readSubstring<double>(line,
                                            line_parser,
                                            cur_read_pos,
                                            m_n_vel_chars,
                                            "GROTrajectory: missing x velocity");
            cur_read_pos += m_n_vel_chars;
            
            // y velocity
            vel_i.y = readSubstring<double>(line,
                                            line_parser,
                                            cur_read_pos,
                                            m_n_vel_chars,
                                            "GROTrajectory: missing y velocity");
            cur_read_pos += m_n_vel_chars;
              
            // z velocity          
            vel_i.z = readSubstring<double>(line,
                                            line_parser,
                                            cur_read_pos,
                                            m_n_vel_chars,
                                            "GROTrajectory: missing z velocity");
            cur_read_pos += m_n_vel_chars;
            
            // set the position and velocity of the particle
            cur_frame->setPosition(particle_id, pos_i);
            cur_frame->setVelocity(particle_id, vel_i);
            
            ++cur_particle;
            }
        if (cur_particle < cur_frame->getN())
            {
            throw std::runtime_error("GROTrajectory: number of particles read does not match specified number");
            }
        
        // acquire the simulation box
        // gro uses a funny ordering:
        // v1(x) v2(y) v3(z) v1(y) v1(z) v2(x) v2(z) v3(x) v3(z)
        // and always requires the first three be present
        if (getline(file, line))
            {
            line_parser.str(line);
            line_parser.clear();
            
            Vector3<double> v1,v2,v3;
            line_parser >> v1.x >> v2.y >> v3.z;
            
            // nested check for the quantities, since we only need to check for more if they exist
            if (line_parser >> v1.y)
                {
                if (line_parser >> v1.z)
                    {
                    if (line_parser >> v2.x)
                        {
                        if (line_parser >> v2.z)
                            {
                            if (line_parser >> v3.x)
                                {
                                if (line_parser >> v3.z)
                                    {
                                    // we have all the values now, stop
                                    }
                                }
                            }
                        }
                    }
                }
            
            // now construct the box using three arbitrarily oriented lattice vectors
            TriclinicBox box(v1,v2,v3);
            cur_frame->setBox(box);
            }
        else
            {
            throw std::runtime_error("GROTrajectory: box must be specified");
            }
        m_frames.push_back(cur_frame);
        }
    }
    
/*!
 * \param line current string line to extract from
 * \param line_parser stringstream object to hold the string
 * \param start position to begin reading from
 * \param len length to read
 * \param errmsg message to print out if substring is not in range or not readable
 *
 * \return the extracted value
 */
template <typename T>
inline T GROTrajectory::readSubstring(const std::string& line,
                                      std::istringstream& line_parser,
                                      unsigned int start,
                                      unsigned int len,
                                      const std::string& errmsg)
    {
    T val(0.0);
    if (start >= line.length())
        {
        throw std::runtime_error(errmsg);
        }
    line_parser.str(line.substr(start,len));
    line_parser.clear();
    if (!(line_parser >> val))
        {
        throw std::runtime_error(errmsg);
        }
    return val;
    }

void export_GROTrajectory()
    {
    using namespace boost::python;
    class_<GROTrajectory, boost::shared_ptr<GROTrajectory>, bases<Trajectory>, boost::noncopyable>("GROTrajectory")
    .def(init<unsigned int>());
    }
