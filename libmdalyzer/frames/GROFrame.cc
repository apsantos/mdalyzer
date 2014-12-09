/*! \file PDBFrame.cc
 *  \author Sang Beom Kim
 *  \brief Reads GRO files
 */
#include "GROFrame.h"

#include <boost/python.hpp>
#include <fstream>
#include <sstream>


/*! \param file Path to GRO file to read
 */
GROFrame::GROFrame(const std::string& fileName)
    : m_file(fileName)
    {
    }

/*! Main routine to parse out the necessary information from file
 */
void GROFrame::readFromFile()
    {
    
    // open GRO file
    std::ifstream file(m_file.c_str());
    if (!file.good()) 
        {
        throw std::runtime_error("ERROR: cannot find GRO file " +  m_file);
        }
        
    //read comment line
	std::string line;
    file >> line;

    // read number of atoms
    file >> m_n_particles;

	// read atom types and positions and velocitieis (if present)
	int type = 0;
	Vector3<double> coord(0.,0.,0.);
    Vector3<double> veloc(0.,0.,0.);
	
	for (unsigned int i = 0; i< m_n_particles; i++)
        {
        
        // read a line
        getline(file, line);
        std::istringstream iss_line(line);
        
        std::string atom_type;
        int dummyread_int;
        
        //read type and coordinates
		iss_line >> line >> dummyread_int >> coord.x >> coord.y >> coord.z;
        
        // read velocities if present
        if ( iss_line >> veloc.x )
            {
            iss_line >> veloc.y >> veloc.z;
            m_has_velocities = true;
            m_velocities.push_back(veloc);
            }
        
		m_types.push_back(atom_type.str());
		m_positions.push_back(coord);
        }
        
    // read the box dimensions and the tilt factors (if present)
    // (UNIT: nanometers)
    getline(file, line)
    std::istringstream iss_line(line);
    double dummyread_double;
    
    Vector3<double> length(0.,0.,0.);
    Vector3<double> tilt(0.,0.,0.);
    
    iss_line >> length.x >> length.y >> length.z;
    if ( iss_line >> dummyread_double )
        {
            iss_line >> dummyread_double >> tilt.x >> dummyread_double >> tilt.y >> tilt.z;
        }
    else
        {
            tilt.x = 0.;
            tilt.y = 0.;
            tilt.z = 0.;
        }
    
    // assign box based on the length and tilt
    m_box = TriclinicBox(length,tilt);
    
	m_has_types = true;
	m_has_box = true;
    
	// GRO file contains none of the following
	m_has_masses = false;
	m_has_diameters = false;
	
	}	


//! Python export for GROFrame
void export_GROFrame()
    {
    using namespace boost::python;
    class_<GROFrame, boost::shared_ptr<GROFrame>, bases<Frame>, boost::noncopyable >
    ("GROFrame", init< const std::string& >());
    }
    
