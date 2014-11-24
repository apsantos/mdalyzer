#include "Trajectory.h"

#include <algorithm>

Trajectory::Trajectory()
    : m_must_read_from_file(true), m_n_particles(0), m_sorted(false)
    {
    }
Trajectory::~Trajectory()
    {
    }

void Trajectory::addFrame(boost::shared_ptr<Frame> frame)
    {
    m_must_read_from_file = true;
    m_frames.push_back(frame);
    }

/*!
 * \param compute Shared pointer to new Compute
 * \param name String key for the Compute, generated on Python level
 *
 * As each frame in the Trajectory is read, each Compute is queried to determine if it needs to calculate on a Frame.
 * We store Computes in a map to make it easier to iterate on them as boost:shared_ptrs.
 */    
void Trajectory::addCompute(boost::shared_ptr<Compute> compute, const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator compute_i = m_computes.find(name);
    if (compute_i == m_computes.end())
        {
        m_computes[name] = compute;
        }
    else
        {
        // Do some type of error handling, like throw an exception
        }
    }

/*!
 * \param name String key for the Compute to remove, as assigned when added
 *
 * The Compute is erased from the map, so this is a harsh removal. All associated data for the Compute is destroyed.
 */
void Trajectory::removeCompute(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator compute_i = m_computes.find(name);
    if (compute_i != m_computes.end())
        {
        m_computes.erase(compute_i);
        }
    else
        {
        // Doesn't exist, can't delete
        }
    }

/*!
 * \param name String key for the Compute to return, as assigned when added
 *
 * \returns A shared pointer to the Compute
 */
boost::shared_ptr<Compute> Trajectory::getCompute(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator compute_i = m_computes.find(name);
    if (compute_i != m_computes.end())
        {
        return m_computes[name];
        }
    else
        {
        // Doesn't exist, raise hell and return a null pointer
        return boost::shared_ptr<Compute>(); // null ptr
        }
    }

void Trajectory::sortFrames()
    {
    std::sort(m_frames.begin(), m_frames.end());
    }

/*!
 * By default, a Trajectory is read by reading all of the constituent Frames into memory
 */    
void Trajectory::read()
    {
    if (m_must_read_from_file) // only load the file if we need to
        {
        // load frames into memory
        for (unsigned int cur_frame_idx=0; cur_frame_idx != m_frames.size(); ++cur_frame_idx)
            {
            boost::shared_ptr<Frame> cur_frame = m_frames[cur_frame_idx];
            
            // read into memory
            cur_frame->readFromFile();
            
            // on the first frame, we snag some properties for all properties
            if (cur_frame_idx == 0)
                {
                m_has_positions = (cur_frame->hasPositions());
                m_has_velocities = (cur_frame->hasVelocities());
                m_has_masses = (cur_frame->hasMasses());
                m_has_diameters = (cur_frame->hasDiameters());
                m_has_types = (cur_frame->hasTypes());
                
                m_n_particles = cur_frame->getN();
                    
                // construct the type map if present
                if (m_has_types)
                    {
                    std::vector<std::string> types = cur_frame->getTypes();
                    for (unsigned int i=0; i < types.size(); ++i)
                        {
                        std::vector<std::string>::iterator type_it = std::find(m_type_map.begin(),m_type_map.end(),types[i]);
                        if (type_it == m_type_map.end())
                            {
                            m_type_map.push_back(types[i]);
                            }
                        }
                    }
                
                
                }
            else
                {
                if ( (m_has_positions && !cur_frame->hasPositions()) || (m_has_velocities && !cur_frame->hasVelocities()))
                    {
                    throw std::runtime_error("Not all frames have positions or velocities");
                    }
                }
            }
        m_must_read_from_file = false; // files have been read, so don't need to read again until something changes
        }
    }

/*!
 * Perform simple checks on the Trajectory, and cleanup Frame discrepancies
 */
void Trajectory::validate()
    {
    double last_frame_time = 0.;
    for (unsigned int cur_frame=0; cur_frame != m_frames.size(); ++cur_frame)
        {
        // check for time ordering
        if (cur_frame > 0 && m_frames[cur_frame]->getTime() <= last_frame_time)
            {
            // error handling, throw an exception and bail
            }
            
        // check for number of particle staying the same if needed
        
        // force type mapping to be the same
        
        last_frame_time = m_frames[cur_frame]->getTime();
        }
    }

unsigned int Trajectory::getTypeByName(const std::string& name) const
    {
    for (unsigned int cur_type=0; cur_type < m_type_map.size(); ++cur_type)
        {
        if (m_type_map[cur_type] == name)
            {
            return cur_type;
            }
        }
    // not found, error!
    throw std::runtime_error("can't find type!");
    return 0;
    }
    
std::string Trajectory::getNameByType(unsigned int type) const
    {
    if (type >= m_type_map.size())
        {
        // error, out of range
        }
    return m_type_map[type];
    }

void Trajectory::analyze()
    {
    std::map< std::string, boost::shared_ptr<Compute> >::iterator cur_compute;
    
    // main execution loop, so we want to catch exceptions as they are thrown and abort
    try
        {
        // read into memory from Frames or by overriden read()
        read();
        
        // sort and validate the frames
        sortFrames();
        validate();
        
        // enter the compute loop
        // if computes should be cleaned up, they need to do this after they are done
        for (cur_compute = m_computes.begin(); cur_compute != m_computes.end(); ++cur_compute)
            {
            // perform the evaluation
            cur_compute->second->evaluate();
            }
        }
    catch (std::exception const & e)
        {
        std::cout<<e.what()<<std::endl;
        throw e;
        }
    }

void export_Trajectory()
    {
    using namespace boost::python;
    class_<Trajectory, boost::shared_ptr<Trajectory> >("Trajectory", init<>())
    .def("analyze",&Trajectory::analyze)
    .def("addCompute",&Trajectory::addCompute)
    .def("removeCompute",&Trajectory::removeCompute)
    .def("getCompute",&Trajectory::getCompute)
    .def("addFrame",&Trajectory::addFrame);
    }
