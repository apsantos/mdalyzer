/*! \file PDBFrame.cc
 *  \author Joseph R. Vella
 *  \brief Reads XYZ files
 */
#include "XYZFrame.h"

#include <boost/python.hpp>
#include <sstream>
#include <math.h>


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
    std::ifstream file(fileName.c_str());
    if (!file.good()) 
	{
        std::cerr << "ERROR: cannot find XYZ file " << fileName << endl;
        exit(0);
    }

    // read number of atoms
    file >> m_n_particles;
	
	//read comment line
	std::string line;
    file >> line;

	// read atom types and positions
	int type
	Vector3<double> coord(0.,0.,0.);
	
	for (int i = 0; i< m_n_particles; i++)
	{
	    //read type and coordinates
		file >> type >> coord.x >> coord.y >> coord.z;
		
		m_types.push_back(type);
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
    class_<XYZFrame, boost::shared_ptr<PDBFrame>, bases<Frame>, boost::noncopyable >
    ("XYZFrame", init< const std::string& >());
    }
    