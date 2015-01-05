/*! 
 * \file PDBTrajectory.cc
 * \author Sang Beom Kim
 * \author Michael P. Howard
 * \date 30 December 2014
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

PDBTrajectory::PDBTrajectory()
    : m_pdb_timestep(1.0)
    {
    }

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
    int last_atom_id = 0;
    
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
            double alpha(0.0), beta(0.0), gamma(0.0);
            
            // pdb formatting is a strict punch card, so can use sscanf with fixed widths
            sscanf(line.c_str(), "%*6s%9lf%9lf%9lf%7lf%7lf%7lf", &a, &b, &c, &alpha, &beta, &gamma);
            
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
        // particle data, both atom entries look the same
        else if (reading_frame && (line_tag.compare("ATOM  ") == 0 || line_tag.compare("HETATM") == 0))
            {
            // make sure the line is long enough
            if (line.length() < 46)
                {
                throw std::runtime_error("PDBTrajectory: ATOM line is not long enough");
                }
            
            // extract the atom data from scanf
            int atom_id(0);
            char atom_name[5];
            Vector3<double> pos_i;
            sscanf(line.c_str(), "%*6s%5d%4s%*15s%8lf%8lf%8lf", &atom_id, atom_name, &pos_i.x, &pos_i.y, &pos_i.z);
            
            // ensure particle ordering
            if (atom_id == (last_atom_id+1))
                {            
                // push back particle data
                names.push_back( std::string(atom_name) );
                positions.push_back(pos_i);
                last_atom_id = atom_id;
                }
            else
                {
                throw std::runtime_error("PDBTrajectory: PDB atoms must be in numerical order starting from 1");
                }
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
                cur_frame->setBox(box);
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
    class_<PDBTrajectory, boost::shared_ptr<PDBTrajectory>, bases<Trajectory>, boost::noncopyable>("PDBTrajectory")
    .def(init<double>());
    }
