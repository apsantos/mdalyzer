/*! \file DCDTrajectory.cc
 *  \author Andrew P. Santos
 *  \brief Reads DCD files
 */
#include "DCDTrajectory.h"

#include <boost/python.hpp>
#include <sstream>
#include <fstream>

/*! \param file Path to DCD file to read
 */
DCDTrajectory::DCDTrajectory(const std::string& fileName, const unsigned int& frequency)
    : m_file(fileName), m_frequency(frequency)
    {
    }


/*! Main routine to parse out the necessary information from file
 */
void DCDTrajectory::read()
    {
    // check that there is an initial file
    // check that the DCD exists
  // only try to open as binary 
    // open GRO file
    std::ifstream file(m_file.c_str());
    if (!file.good()) 
        {
        throw std::runtime_error("ERROR: cannot find GRO file " +  m_file);
        }

    // read the DCD header
    bool pbd_info = false;
    std::string line;
    getline(file, line);
    //readHeader(file, pbd_info);
    std::istringstream iss_line(line);

	std::string header;
    std::vector<int> d(8, 0); // dummy variable
    int pbd_info_int = 0 ;
    int n_frames;
    int start_step;
    int interval;
    int delta;
    iss_line >> line >> header >> n_frames >> start_step >> interval >> d[2] >> d[3] >> d[4] >> d[5] >> d[6] >> d[7] >> delta >>  pbd_info_int ;

    //bool pbd_info = false ;
    if ( pbd_info_int == 1 )
        {
        pbd_info = true ;
        }
    file >> line;   // read an empty line
    unsigned int n_particles = 0;
    file >> n_particles; 
    // check that the number of paticles in the DCD match with the input file

    if (pbd_info)
        {
    	std::string pbd_line;
        // read a line
        getline(file, pbd_line);
        std::istringstream iss_line(pbd_line);
    
        std::vector<int> XLTABC(6, 0) ;
        iss_line >> pbd_line >> XLTABC[0] >> XLTABC[1] >> XLTABC[2] >> XLTABC[3] >> XLTABC[4] >> XLTABC[5] ;
        //readPDBinfo(file);
        }

    // skip three lines of header
    file >> line;
    file >> line;
    file >> line;
    // read the DCD timestep
    //for {unsigned int ipart = 0; ipart < m_n_particles}
    // convert 

    // close DCD file
    }
    /*
void DCDTrajectory::readHeader(const std::ifstream& file, bool pbd_info)
    {
    //read comment line
	std::string line;
    // read a line
    }
    std::istringstream iss_line(line);

	std::string header;
    std::vector<int> d(8, 0); // dummy variable
    int pbd_info_int = 0 ;
    iss_line >> line >> header >> n_frames >> start_step >> interval >> d[2] >> d[3] >> d[4] >> d[5] >> d[6] >> d[7] >> delta >>  pbd_info_int ;

    bool pbd_info = false ;
    if ( pbd_info_int == 1 )
        {
        pbd_info = true ;
        }
    file >> line;   // read an empty lien
    file >> m_n_particles; 
    // check that the number of paticles in the DCD match with the input file

    if (pbd_info)
        {
        readPDBinfo(file);
        }

    // skip three lines of header
    file >> line;
    file >> line;
    file >> line;
    }


void DCDTrajectory::readPDBinfo(const std::ifstream file)
    {
	std::string pbd_line;
    // read a line
    getline(file, pbd_line);
    std::istringstream iss_line(pbd_line);

    std::vector<int> XLTABC(6, 0) ;
    iss_line >> pbd_line >> XLTABC[0] >> XLTABC[1] >> XLTABC[2] >> XLTABC[3] >> XLTABC[4] >> XLTABC[5] ;

    }
*/
//! Python export for DCDTrajectory
void export_DCDTrajectory()
    {
    using namespace boost::python;
    class_<DCDTrajectory, boost::shared_ptr<DCDTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("DCDTrajectory", init< const std::string&, const unsigned int& >());
    }
