/*! 
 * \file PDBTrajectory.cc
 * \author Sang Beom Kim
 * \author Michael P. Howard
 * \date 8 January 2015
 * \brief Implementation of PDBTrajectory reader
 */
#include "PDBTrajectory.h"

#include <boost/algorithm/string.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/python.hpp>

#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

PDBTrajectory::PDBTrajectory(double timestep)
    : m_pdb_timestep(timestep)
    {
    }

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
        readFromFile(file);
        file.close();
        }
    m_must_read_from_file = false;
    }

/*! 
 * \param file input file stream to parse
 */
void PDBTrajectory::readFromFile(std::ifstream& file)
    {
    // string containers for parsing line by line
    std::string line;
    std::istringstream line_parser; 
    
    // conversion factor to take angle degrees to radians
    const double degrees_to_radians = boost::math::constants::pi<double>()/180.0;
    
    // containers for each Frame data
    boost::shared_ptr<Frame> cur_frame;
    double frame_time(0.0);
    std::vector< Vector3<double> > positions;
    std::vector<std::string> names;
    
    // a single pdb file contains at most one cryst1 record
    bool found_box = false;
    TriclinicBox box;
    
    // flag for currently reading a model
    bool reading_frame = false;
    // track atoms as they are read to ensure ordering
    unsigned int last_atom_id = 0;
    
    while (getline(file, line))
        {
        // skip over empty lines
        if (line.empty())
            continue;   
        
        // each PDB line begins with a 6 character tag which we use to identify it
        std::string line_tag = line.substr(0,6);
        
        // simulation box
        if (!found_box && line_tag.compare("CRYST1") == 0)
            {
            if (line.length() < 47)
                {
                throw std::runtime_error("PDBTrajectory: CRYST1 line is not long enough");
                }
                
            // lattice vectors
            double a(0.0), b(0.0), c(0.0);
            // lattice angles
            double alpha(90.0), beta(90.0), gamma(90.0);
            
            // pdb formatting is a strict punch card, so can use sscanf with fixed widths
            int entries = sscanf(line.c_str(), "%*6s%9lf%9lf%9lf%7lf%7lf%7lf", &a, &b, &c, &alpha, &beta, &gamma);
            if (entries != 6)
                {
                throw std::runtime_error("PDBTrajectory: CRYST1 record must be a b c alpha beta gamma");
                found_box = false;
                }
            else
                {
                // extract the box lengths and tilts from the lattice constants
                // http://lammps.sandia.gov/doc/Section_howto.html (triclinic box)
                Vector3<double> length, tilt;
                length.x = a;
                tilt.x = b*cos(gamma*degrees_to_radians);
                tilt.y = c*cos(beta*degrees_to_radians);
                length.y = sqrt(b*b-tilt.x*tilt.x);
                tilt.z = (b*c*cos(alpha*degrees_to_radians)-tilt.x*tilt.y)/length.y;
                length.z = sqrt(c*c-tilt.y*tilt.y-tilt.z*tilt.z);
            
                box = TriclinicBox(length,tilt);
                found_box = true;
                }
            }
        // particle data, both atom entries look the same
        else if (reading_frame && (line_tag.compare("ATOM  ") == 0 || line_tag.compare("HETATM") == 0))
            {
            // make sure the line is long enough
            if (line.length() < 54)
                {
                throw std::runtime_error("PDBTrajectory: ATOM line is not long enough");
                }
            // get number
            unsigned int atom_id = atoi(line.substr(6,5).c_str());
            
            // get name
            std::string atom_name = line.substr(12,4);
            boost::algorithm::trim(atom_name);
            
            // get position
            Vector3<double> pos_i;
            pos_i.x = strtod(line.substr(30,8).c_str(),NULL);
            pos_i.y = strtod(line.substr(38,8).c_str(),NULL);
            pos_i.z = strtod(line.substr(46,8).c_str(),NULL);
            
            // ensure particle ordering
            if (atom_id > 0)
                {           
                if (atom_id != (last_atom_id+1)) 
                    {
                    throw std::runtime_error("PDBTrajectory: PDB atoms must be in numerical order starting from 1");
                    }
                }
                
            // push back particle data
            if (atom_name.length() > 0)
                {
                names.push_back(atom_name);
                }
            positions.push_back(pos_i);
            ++last_atom_id;
            }
        // model begins scanning
        else if (!reading_frame && line_tag.compare("MODEL ") == 0)
            {
            reading_frame = true;
            
            // clear out old data
            last_atom_id = 0;
            names.clear();
            positions.clear();
            
            // get the frame time step
            line_parser.str(line);
            line_parser.clear();
            std::string dummy_str;
            if (line_parser >> dummy_str >> frame_time)
                {            
                // scale the extracted timestep by the frame time skip
                frame_time -= 1.0; // model id begins from 1, so subtract to get time
                frame_time *= m_pdb_timestep;
                }
            else
                {
                throw std::runtime_error("PDBTrajectory: MODEL line must set the frame id");
                }
            }
        // end model, time to commit frame
        else if (reading_frame && line_tag.compare("ENDMDL") == 0)
            {
            if (positions.size() > 0)
                {
                // set the frame data
                cur_frame = boost::shared_ptr<Frame>( new Frame(positions.size()) );
                cur_frame->setNames(names);
                cur_frame->setPositions(positions);
                if (found_box)
                    {
                    cur_frame->setBox(box);
                    }
                else
                    {
                    throw std::runtime_error("PDBTrajectory: TriclinicBox must be set with CRYST1");
                    }
                cur_frame->setTime(frame_time);
                
                m_frames.push_back(cur_frame);
                }
            // reset to a scanning state
            reading_frame = false;
            }
        }
    }

void export_PDBTrajectory()
    {
    using namespace boost::python;
    class_<PDBTrajectory, boost::shared_ptr<PDBTrajectory>, bases<Trajectory>, boost::noncopyable>
    ("PDBTrajectory", init<double>());
    }
