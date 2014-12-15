#include "Trajectory.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <boost/python.hpp>

Trajectory::Trajectory()
    : m_must_read_from_file(true), m_loc_box(NONE), m_loc_names(NONE),
    m_loc_types(NONE), m_loc_diameters(NONE), m_loc_masses(NONE), m_n_particles(0), m_sorted(false)
    {
    }

/*!
 * \param compute Shared pointer to new Compute
 * \param name String key for the Compute, generated on Python level
 *
 * As each frame in the Trajectory is read, each Compute is queried to determine if it needs to calculate on a Frame.
 * We store Computes in a map to make it easier to iterate on them as boost:shared_ptrs.
 */    
void Trajectory::addCompute(boost::shared_ptr<Analyzer> compute, const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator compute_i = m_computes.find(name);
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
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator compute_i = m_computes.find(name);
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
boost::shared_ptr<Analyzer> Trajectory::getCompute(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator compute_i = m_computes.find(name);
    if (compute_i != m_computes.end())
        {
        return m_computes[name];
        }
    else
        {
        // Doesn't exist, raise hell and return a null pointer
        return boost::shared_ptr<Analyzer>(); // null ptr
        }
    }


//! add an atom name to the trajectory type map
unsigned int Trajectory::addName(const std::string& name)
    {
    std::map<std::string, unsigned int>::iterator cur_type = m_type_map.find(name);
    if (cur_type == m_type_map.end())
        {
        unsigned int type_id = m_type_map.size();
        m_type_map[name] = type_id;
        }
    return m_type_map[name];
    }
    
//! remove an atom name from the type map
void Trajectory::removeName(const std::string& name)
    {
    std::map<std::string, unsigned int>::iterator cur_type = m_type_map.find(name);
    if (cur_type != m_type_map.end())
        m_type_map.erase(cur_type);
    }
    
//! convert atom name to trajectory type id number
unsigned int Trajectory::getTypeByName(const std::string& name)
    {
    std::map<std::string, unsigned int>::iterator cur_type = m_type_map.find(name);
    if (cur_type == m_type_map.end())
        throw std::runtime_error("Trajectory: name not found");
        
    return m_type_map[name];
    }

//! convert trajectory type id to atom name
std::string Trajectory::getNameByType(unsigned int type)
    {
    std::map<std::string, unsigned int>::iterator cur_type;
    for (cur_type = m_type_map.begin(); cur_type != m_type_map.end(); ++cur_type)
        {
        if (cur_type->second == type)
            return cur_type->first;
        }
    throw std::runtime_error("Trajectory: type not found");
    return "";
    }

struct frameCompare
    {
        inline bool operator()(boost::shared_ptr<Frame> f1, boost::shared_ptr<Frame> f2)
            {
            return (f1->getTime() < f2->getTime());
            }
    };
void Trajectory::sortFrames()
    {
    std::sort(m_frames.begin(), m_frames.end(), frameCompare());
    }

/*!
 * An abstract trajectory does not read
 */    
void Trajectory::read()
    {
    }
    
//! parse the type map and other trajectory long properties
// void Trajectory::parse()
//     {
//     }

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
    
void Trajectory::parse()
    {
    m_n_particles = m_frames[0]->getN();
    
    if (m_loc_box == NONE && m_frames[0]->hasBox())
        {
        m_loc_box = FRAME;
        m_box = m_frames[0]->getBox();
        }
    if (m_loc_diameters == NONE && m_frames[0]->hasDiameters())
        {
        m_loc_diameters = FRAME;
        m_diameters = m_frames[0]->getDiameters();
        }
    if (m_loc_masses == NONE && m_frames[0]->hasMasses())
        {
        m_loc_masses = FRAME;
        m_masses = m_frames[0]->getMasses();
        }
    if (m_loc_names == NONE && m_frames[0]->hasNames())
        {
        m_loc_names = FRAME;
        m_loc_types = FRAME;
        m_names = m_frames[0]->getNames();
        }
    
    // parse frame by frame for types         
    for (unsigned int frame_idx = 0; frame_idx < m_frames.size(); ++frame_idx)
        {
        if (m_frames[frame_idx]->hasNames())
            {
            const std::vector<std::string> names = m_frames[frame_idx]->getNames();
    
            for (unsigned int p_idx = 0; p_idx < names.size(); ++p_idx)
                {
                std::map<std::string, unsigned int>::iterator type_pair = m_type_map.find(names[p_idx]);
                unsigned int type = (type_pair == m_type_map.end()) ? addName(names[p_idx]) : type_pair->second;
                m_frames[frame_idx]->setType(p_idx, type);
                }
            }
        }
    
    // parse the types for the trajectory level if needed
    if (m_loc_names != NONE)
        {
        m_loc_types = m_loc_names;
        m_types.resize(m_names.size());
        for (unsigned int p_idx = 0; p_idx < m_names.size(); ++p_idx)
            {
            std::map<std::string, unsigned int>::iterator type_pair = m_type_map.find(m_names[p_idx]);
            m_types[p_idx] = (type_pair == m_type_map.end()) ? addName(m_names[p_idx]) : type_pair->second;
            }
        }
    }

void Trajectory::analyze()
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator cur_compute;
    
    // main execution loop, so we want to catch exceptions as they are thrown and abort
    try
        {
        // read into memory from Frames or by overriden read()
        read();

        // sort and validate the frames
        sortFrames();
        validate();
        parse();
        
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

/*!
 * Boost needs us to define a wrapper around Trajectory
 */
struct TrajectoryWrap : public Trajectory, boost::python::wrapper<Trajectory>
    {
    TrajectoryWrap() : Trajectory() {}
    
    void read()
        {
        if (boost::python::override f = this->get_override("read"))
            f();
        else
            Trajectory::read();
        }
        
    void default_read() { this->Trajectory::read(); }
    };

void export_Trajectory()
    {
    using namespace boost::python;
    class_<Trajectory, boost::shared_ptr<Trajectory>, boost::noncopyable >("Trajectory", init<>())
    .def("analyze",&Trajectory::analyze)
    .def("addCompute",&Trajectory::addCompute)
    .def("removeCompute",&Trajectory::removeCompute)
    .def("getCompute",&Trajectory::getCompute)
    .def("getFrame",&Trajectory::getFrame)
    .def("read", &Trajectory::read, &TrajectoryWrap::default_read);
    }
