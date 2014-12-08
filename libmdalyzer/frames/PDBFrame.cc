/*! \file PDBFrame.cc
 *  \author Sang Beom Kim
 *  \brief Reads PDB files
 */
#include "PDBFrame.h"

#include <boost/python.hpp>
#include <fstream>
#include <sstream>
#include <math.h>

#define PI 3.14159265

/*! \param file Path to pdb file to read
 */
PDBFrame::PDBFrame(const std::string& fileName)
    : m_file(fileName)
    {
    }

/*! Main routine to parse out the necessary information from file
 */
void PDBFrame::readFromFile()
    {
    
    // open PDB file
    std::ifstream file(m_file.c_str());
    if (!file.good())
        {
        throw std::runtime_error("ERROR: cannot find PDB file " + m_file);
        }

    // read until CRYST1 header
    std::string line;
    file >> line;
    while (line.compare("CRYST1") != 0)
        {
        getline(file, line);
        file >> line;
        
        // throw error if the file does not have CRYST1 line
        // which contains the box dimension info
        if ( file.eof() )
            {
            throw std::runtime_error("ERROR: PDB file should contain line staring with \"CRYST1\" which contains info of box dimensions");
            }
        }

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
    m_box = TriclinicBox(length,tilt);
    
    
    // read until ATOM header
    file >> line;
    while (line.compare("ATOM") != 0)
        {
        getline(file, line);
        file >> line;
        }

    // read until TER header
    char * buffer;
    buffer = new char[11];
    std::string atom_type;
    int dummyread_int;
    m_n_particles = 0;
    Vector3<double> coord(0.,0.,0.);
    
    while (line.compare("TER") != 0)
        {
        // ignore the first 11 characters
        file.read(buffer,11);
        
        // read the type
        file >> atom_type >> line >> line >> dummyread_int;
        
        // read coordinates
        file >> coord.x >> coord.y >> coord.z;
        
        // insert type and coordinates
        m_types.push_back(atom_type);
        m_positions.push_back(coord);
        
        getline(file, line);
        file >> line;
        
        // update number of particles
        ++m_n_particles;
        }
  
    // PDB file contains
    m_has_box = true;
    m_has_types = true;
  
    // PDB file contains none of the following
    m_has_velocities = false;
    m_has_masses = false;
    m_has_diameters = false;
}


//! Python export for PDBFrame
void export_PDBFrame()
    {
    using namespace boost::python;
    class_<PDBFrame, boost::shared_ptr<PDBFrame>, bases<Frame>, boost::noncopyable >
    ("PDBFrame", init< const std::string& >());
    }
    
