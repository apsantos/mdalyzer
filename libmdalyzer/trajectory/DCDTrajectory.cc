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
void DCDTrajectory::readHeader(FILE* fileptr)
    {
        
    int errcode;

    errcode = read_dcdheader(fileptr, &m_n_dcdparticles_c, &m_n_frames_c, &m_frame_start_c,
                            &m_frame_skip_c, &m_time_step, &m_n_fixed_c, &m_freeparticles_c,
                            &m_reverse_endian_c, &m_charmm_flags_c);

    m_n_dcdparticles = std::abs(m_n_dcdparticles_c);
    m_n_frames = std::abs(m_n_frames_c);
/*
    m_frame_start = std::abs(frameStart);
    m_frame_skip  = std::abs(frameSkip);
    m_freeparticles = freeatoms;
    m_reverse_endian = std::abs(reverseEndian);
    m_namnf = std::abs(namnf);
    m_charmm_flags = std::abs(charmm_flags);
*/

    if (errcode < 0 || m_n_dcdparticles <= 0) 
        {
        if (errcode == DCD_BADFORMAT)
            {
            throw std::runtime_error( "ERROR: (DCDTrajectory) Improper format for DCD file" + m_file);
            }
        else if (errcode == DCD_BADMALLOC)
            {
            throw std::runtime_error( "ERROR: (DCDTrajectory) DCD file not open" + m_file);
            }
        else if (m_n_dcdparticles <= 0) 
            {
            throw std::runtime_error(  "ERROR: (DCDTrajectory) No atoms found in DCD file" + m_file);
            }
        else
            {
  	        throw std::runtime_error( "ERROR: (DCDTrajectory) Early end to DCD file" + m_file);
            }
        } 
    }

void DCDTrajectory::readTimeStep(FILE* fileptr, std::vector< Vector3<double> >& positions)
    {
    // read in next step, if available
    int errcode;
    float *X = new float[m_n_dcdparticles];
    float *Y = new float[m_n_dcdparticles];
    float *Z = new float[m_n_dcdparticles];

    errcode = read_dcdstep(fileptr, m_n_dcdparticles_c, X, Y, Z, m_n_fixed_c,
    	 (m_first_dcdread == 0), m_freeparticles_c, m_reverse_endian_c, m_charmm_flags_c);

    if (m_first_dcdread == 0)
        {
        m_first_dcdread = 1;
        }
    // check for errors
    if (errcode < 0) 
        {
        if (errcode == (-1)) 
            {
            throw std::runtime_error( "End of DCD file reached." + m_file);
            }
        else if (errcode == DCD_BADFORMAT)
            {
            throw std::runtime_error( "ERROR: Improper format for DCD file" + m_file);
            }
        else
            {
            throw std::runtime_error( "ERROR: Early end to DCD file" + m_file);
            }
        }
    // convert the c arrays to our API form
    for (unsigned int ipart = 0; ipart < m_n_dcdparticles; ++ipart)
        {
            Vector3<double> pos_v3; 
            pos_v3.x = X[ipart];
            pos_v3.y = X[ipart];
            pos_v3.z = X[ipart];

            positions.push_back(pos_v3);
        }
    // delete the arrays
    delete [] X;
    delete [] Y;
    delete [] Z;
    }

void DCDTrajectory::read()
    {

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
    readHeader(fileptr);

    // check that the number of paticles in the DCD match with the input file

    // could possibly add the option to read a specific range of the DCD file trajectory
    // read the DCD timestep
    for (unsigned int frame_idx = 0; frame_idx < m_n_frames; ++frame_idx)
        {
        std::vector< Vector3<double> > pos;
        readTimeStep(fileptr, pos);
        }
    // convert 

    // close DCD file
    }
    /*
void DCDTrajectory::readHeader(const std::ifstream& file, bool pbd_info)
    {
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
