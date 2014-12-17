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
DCDTrajectory::DCDTrajectory(const std::string& fileName)
    : m_file(fileName)
    {
    }

/*! Routine to read header information of the DCD file, check errors and interface the
    external library ReadDCD.cc to mdalyzer 
 */
void DCDTrajectory::readHeader(FILE* fileptr)
    {
        
    int errcode;

    errcode = read_dcdheader(fileptr, &m_n_dcdparticles_c, &m_n_frames_c, &m_frame_start_c,
                            &m_frame_skip_c, &m_time_step, &m_n_fixed_c, &m_freeparticles_c,
                            &m_reverse_endian_c, &m_charmm_flags_c);

    m_n_dcdparticles = std::abs(m_n_dcdparticles_c);
    m_n_frames = std::abs(m_n_frames_c);
    m_frame_skip = std::abs(m_frame_skip_c);

    if (errcode < 0 || m_n_dcdparticles <= 0) 
        {
        if (errcode == DCD_BADFORMAT)
            {
            throw std::runtime_error( "ERROR: (DCDTrajectory) Improper format for DCD file Header" + m_file);
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

/*! Routine to read a time step from the DCD file, check errors and interface the
    external library ReadDCD.cc to mdalyzer 
 */
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

/*! Main routine to parse out the necessary information from file
 */
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

    for (unsigned int frame_idx = 0; frame_idx < m_n_frames; ++frame_idx)
        {
        std::vector< Vector3<double> > pos;
        readTimeStep(fileptr, pos);
        }
    m_has_positions = true;

    // close DCD file
    close_dcd_read(fileptr, m_n_fixed_c, m_freeparticles_c);
    }

//! Python export for DCDTrajectory
void export_DCDTrajectory()
    {
    using namespace boost::python;
    class_<DCDTrajectory, boost::shared_ptr<DCDTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("DCDTrajectory", init< const std::string& >());
    }
