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
int DCDTrajectory::readHeader(FILE* fileptr)
    {
    int errcode;
    int dcdatoms;
    int nFrames;
    int frameStart;
    int frameSkip;
    double timeStep;
    int namnf;
    int *freeatoms;
    int reverseEndian;
    int charmmFlags;

    errcode = read_dcdheader(fileptr, &dcdatoms, &nFrames, &frameStart,
                            &frameSkip, &timeStep, &namnf, &freeatoms,
                            &reverseEndian, &charmmFlags);
    if (errcode < 0 || dcdatoms <= 0) 
        {
        if (errcode == DCD_BADFORMAT)
            {
            throw std::runtime_error( "ERROR: (LoadDCD) Improper format for DCD file" + m_file);
            }
        else if (errcode == DCD_BADMALLOC)
            {
            throw std::runtime_error( "ERROR: (LoadDCD) DCD file not open" + m_file);
            }
        else if (dcdatoms <= 0) 
            {
            throw std::runtime_error(  "ERROR: (LoadDCD) No atoms found in DCD file" + m_file);
            }
        else
            {
  	        throw std::runtime_error( "ERROR: (LoadDCD) Early end to DCD file" + m_file);
            }
        } 
    return errcode;
    }
void DCDTrajectory::read()
    {
    int head_err;

    // open the file
    const char * c_filename = m_file.c_str();
    FILE* fileptr = open_dcd_read(c_filename);
    // check that the DCD exists
    if (fileptr == NULL)
        {
        throw std::runtime_error("ERROR: cannot find DCD file " +  m_file);
        }
    

    // check that there is an initial frame 

    // read the DCD header
    head_err = readHeader(fileptr);

    // check that the number of paticles in the DCD match with the input file

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
