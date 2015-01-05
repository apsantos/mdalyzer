/*! \file DCDTrajectory.cc
 *  \author Andrew P. Santos
 *  \brief Reads DCD files
 *  \todo check that there is an initial frame
 *  The user must be aware of whether the particle positions in the DCD file
 *  are wrapped or unwrapped around the simulation box.  It is correctness of the
 *  analyzers depends on this detail.
 */
#include "DCDTrajectory.h"

#include <boost/python.hpp>
#include <sstream>
#include <fstream>

/*! \param file Path to DCD file to read
 */
DCDTrajectory::DCDTrajectory( boost::shared_ptr<Trajectory> initial_traj, const std::string& fileName)
    : m_initial_traj(initial_traj), m_file(fileName)
    {
    }

/*! 
 *  Routine to read header information of the DCD file, check errors and interface the
 *  external library ReadDCD.cc to mdalyzer 
 *  \param fileptr FILE type pointer to the DCD file
 */
void DCDTrajectory::readHeader(FILE* fileptr)
    {
        
    int errcode;
    int frame_skip_c;             //!< frame read interval

    errcode = read_dcdheader(fileptr, &m_n_dcdparticles_c, &m_n_frames_c, &m_frame_start_c,
                            &frame_skip_c, &m_time_step, &m_n_fixed_c, &m_freeparticles_c,
                            &m_reverse_endian_c, &m_charmm_flags_c);

    m_n_dcdparticles = (unsigned int)m_n_dcdparticles_c;
    m_n_frames = (unsigned int)m_n_frames_c;
    m_frame_start = (unsigned int)m_frame_start_c;

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
 *  external library ReadDCD.cc to mdalyzer 
 *  \param fileptr FILE type pointer to the opened dcd file
 *  \returns Frame pointer with positions and types
 */
boost::shared_ptr<Frame> DCDTrajectory::readTimeStep(FILE* fileptr)
    {
    boost::shared_ptr<Frame> cur_frame; //default initialization is null ptr
    cur_frame = boost::shared_ptr<Frame>( new Frame(m_n_dcdparticles) );
    //cur_frame->setTypes( m_frames[0]->getTypes() );
    //cur_frame->setNames( m_frames[0]->getNames() );
    //cur_frame->setMasses( m_frames[0]->getMasses() );
    std::vector<float> X(m_n_dcdparticles, 0.);
    std::vector<float> Y(m_n_dcdparticles, 0.);
    std::vector<float> Z(m_n_dcdparticles, 0.);

    // read in next step, if available
    int errcode = read_dcdstep(fileptr, m_n_dcdparticles_c, &X[0], &Y[0], &Z[0], m_n_fixed_c,
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
            throw std::runtime_error( "End of DCD file reached: " + m_file );
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

    // convert the c arrays to our API 
    for (unsigned int ipart = 0; ipart < m_n_dcdparticles; ++ipart)
        {
            Vector3<double> pos; 
            pos.x = X[ipart];
            pos.y = Y[ipart];
            pos.z = Z[ipart];
            cur_frame->setPosition( ipart, pos ); 
        }
    return cur_frame;
    }

/*! Main routine to parse out the necessary information from file
 */
void DCDTrajectory::read()
    {
    // read the initial frame
    m_must_read_from_file = true;
    m_initial_traj->read();
    m_frames.push_back( m_initial_traj->getFrame(0) );
    // set information to the dcd trajectory
    //this->setNames( m_initial_traj->getNames() );
    //this->setDiameters( m_initial_traj->getDiameters() );
    //this->setMasses( m_initial_traj->getMasses() );
    // read the dcd 
    readFromFile();
    // Check thta the number of particles match the initial and DCD trajectories
    
    m_must_read_from_file = false;
    }

void DCDTrajectory::readFromFile()
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

    unsigned int start_time = 0;
    if ( m_frames[0]->hasTime() )
        {
        start_time = m_frames[0]->getTime();
        }
    else
        {
        start_time = m_frame_start;
        }

    // check that the number of paticles in the DCD match with the input file
    std::string outf_name = "dcdstuff.dat";
    std::ofstream outf(outf_name.c_str());
    outf.precision(4);
    outf<<"start time = "<<m_frame_start<<"\n";
    outf<<"dcd num frames = "<<m_n_frames<<"\n";
    outf<<"dcd num particless = "<<m_n_dcdparticles<<"\n";

    for (unsigned int frame_idx = 0; frame_idx < m_n_frames; ++frame_idx)
        {
        boost::shared_ptr<Frame> cur_frame = readTimeStep(fileptr);
        cur_frame->setTime( (double)(m_time_step * (frame_idx+1)) + start_time );
        m_frames.push_back(cur_frame);
        std::vector< Vector3<double> > pos = cur_frame->getPositions();
        Vector3<double> cur_pos = pos[m_n_dcdparticles-1];
        outf<<"frame "<<frame_idx+1<<"\t"<<cur_pos.x<<"\n";
        }
    outf<<"final number of frames = "<<m_frames.size()<<"\n";
    outf.close();

    // close DCD file
    close_dcd_read(fileptr, m_n_fixed_c, m_freeparticles_c);

    }

//! Python export for DCDTrajectory
void export_DCDTrajectory()
    {
    using namespace boost::python;
    class_<DCDTrajectory, boost::shared_ptr<DCDTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("DCDTrajectory", init< boost::shared_ptr<Trajectory>, const std::string& >());
    }
