/*! \file PDBFrame.cc
 *  \author Joseph R. Vella
 *  \brief Reads XYZ files
 */
#include "XYZFrame.h"

#include <boost/python.hpp>
#include <fstream>
#include <sstream>


/*! \param file Path to xyz file to read
 */
XYZFrame::XYZFrame(const std::string& fileName)
    : m_file(fileName)
    {
    }

/*! Main routine to parse out the necessary information from file
 */
void XYZFrame::readFromFile()
    {
    
    // open XYZ file
    std::ifstream file(m_file.c_str());
    if (!file.good()) 
        {
        throw std::runtime_error("ERROR: cannot find XYZ file " +  m_file);
        }

    // read number of atoms
    file >> m_n_particles;
	
	//read comment line
	std::string line;
    file >> line;

	// read atom types and positions
	int type = 0;
	Vector3<double> coord(0.,0.,0.);
	
	for (unsigned int i = 0; i< m_n_particles; i++)
        {
	    //read type and coordinates
		file >> type >> coord.x >> coord.y >> coord.z;
		
        // convert type from int to str
        std::ostringstream convert_type;
        convert_type << type;
        
		m_types.push_back(convert_type.str());
		m_positions.push_back(coord);
        }
	
	m_has_types = true;
	
	// XYZ file contains none of the following
	m_has_velocities = false;
	m_has_masses = false;
	m_has_diameters = false;
	m_has_box = false;
	}	


//! Python export for XYZFrame
void export_XYZFrame()
    {
    using namespace boost::python;
    class_<XYZFrame, boost::shared_ptr<XYZFrame>, bases<Frame>, boost::noncopyable >
    ("XYZFrame", init< const std::string& >());
    }
    
