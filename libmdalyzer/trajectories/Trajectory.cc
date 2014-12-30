/*!
 * \file Trajectory.cc
 * \author Michael P. Howard
 * \date 16 December 2014
 * \brief Implementation of Trajectory data structure
 */
#include "Trajectory.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <boost/python.hpp>

Trajectory::Trajectory()
    : m_must_read_from_file(true), m_n_particles(0), m_sorted(false), m_loc_box(NONE), m_loc_names(NONE),
    m_loc_types(NONE), m_loc_diameters(NONE), m_loc_masses(NONE)
    {
    }

/*!
 * \param compute Shared pointer to new Analyzer
 * \param name key for the Analyzer
 */    
void Trajectory::addAnalyzer(boost::shared_ptr<Analyzer> analyzer, const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator analyzer_i = m_analyzers.find(name);
    if (analyzer_i == m_analyzers.end())
        {
        m_analyzers[name] = analyzer;
        }
    else
        {
        throw std::runtime_error("Trajectory: cannot add Analyzer with same name");
        }
    }

/*! \param name key for the Compute to remove, as assigned when added. */
void Trajectory::removeAnalyzer(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator analyzer_i = m_analyzers.find(name);
    if (analyzer_i != m_analyzers.end())
        {
        m_analyzers.erase(analyzer_i);
        }
    else
        {
        throw std::runtime_error("Trajectory: cannot remove non-existent Analyzer");
        }
    }

/*!
 * \param name key for the Analyzer to return, as assigned when added
 * \returns A pointer to the Analyzer
 */
boost::shared_ptr<Analyzer> Trajectory::getAnalyzer(const std::string& name)
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator analyzer_i = m_analyzers.find(name);
    if (analyzer_i != m_analyzers.end())
        {
        return m_analyzers[name];
        }
    else
        {
        throw std::runtime_error("Trajectory: requested Analyzer does not exist");
        return boost::shared_ptr<Analyzer>(); // null ptr
        }
    }


/*!
 * \param name particle name
 * \returns integer id for type
 */
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
    
/*! \param name particle name */
void Trajectory::removeName(const std::string& name)
    {
    std::map<std::string, unsigned int>::iterator cur_type = m_type_map.find(name);
    if (cur_type != m_type_map.end())
        m_type_map.erase(cur_type);
    }
    
/*!
 * \param name particle name
 * \returns integer id for type
 */
unsigned int Trajectory::getTypeByName(const std::string& name)
    {
    std::map<std::string, unsigned int>::iterator cur_type = m_type_map.find(name);
    if (cur_type == m_type_map.end())
        throw std::runtime_error("Trajectory: name not found");
        
    return m_type_map[name];
    }

/*!
 * \param type integer id for type
 * \returns name particle name
 */
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
   
   
//! wrapper for time ordering shared pointers to frames
/*!
 * We need to access the time property of Frame objects in order to time sort them. Since we have pointers
 * we cannot simply overload the comparison operator for the Frame, so we use a wrapper struct to do the
 * comparison.
 */
struct FrameCompare
    {
    //! operator for comparison
    /*! 
     * \param f1 first frame
     * \param f2 second frame
     * \returns true if first frame has time earlier than second frame
     */
    inline bool operator()(boost::shared_ptr<Frame> f1, boost::shared_ptr<Frame> f2)
        {
        return (f1->getTime() < f2->getTime());
        }
    }; 
/*!
 * Uses the STL sort along with FrameCompare to time order the Frames.
 * \sa FrameCompare
 */
void Trajectory::sortFrames()
    {
    std::sort(m_frames.begin(), m_frames.end(), FrameCompare());
    }

/*!
 * A plain Trajectory currently may be instantiated but not read.
 */    
void Trajectory::read()
    {
    }

/*!
 * \param f file name to attach
 *
 * Any time a new file is attached, the Trajectory must be re-read from file. This could be handled in a smart way
 * flushing the read file list so that only newly added files are read, and not everything. This should be considered
 * in read() in the future.
 *
 * \note error checking for duplicates is currently not enabled, but we will implement this soon.
 */
void Trajectory::addFile(const std::string& f)
    {
    m_must_read_from_file = true;
    m_files.push_back(f); // error check this later
    }

/*!
 * Check simple things for the trajectory:
 *      + The Trajectory has at least one frame
 *      + Each Frame is in time order
 *      + The number of particles is constant
 */
void Trajectory::validate()
    {
    if (m_frames.size() == 0)
        throw std::runtime_error("Trajectory: no Frame attached for analysis");
    
    double last_frame_time = 0.;
    for (unsigned int cur_frame=0; cur_frame != m_frames.size(); ++cur_frame)
        {
        // check for time ordering
        if (cur_frame > 0 && m_frames[cur_frame]->getTime() <= last_frame_time)
            {
            throw std::runtime_error("Trajectory: bug, frames are not time ordered");
            }
            
        // check for number of particle staying the same if needed
        if (m_frames[cur_frame]->getN() != m_frames[0]->getN())
            throw std::runtime_error("Trajectory: all frames must have the same number of particles");
        
        last_frame_time = m_frames[cur_frame]->getTime();
        }
    }

/*!
 * Extracts information from the first Frame of the Trajectory if this data has not already been set by the user.
 * String particle names are then read into a Trajectory type map, which maps strings to integer identifiers.
 * The atom types are then set for each Frame using this type map.
 */    
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

/*!
 * Main execution loop for the Trajectory with exception catching. The Trajectory is first read into memory.
 * Frames are time ordered, and validated to stop the user from doing anything crazy. Trajectory level information
 * is extracted from the Frames. Calculations are then performed by iterating over Analyzers.
 */
void Trajectory::analyze()
    {
    std::map< std::string, boost::shared_ptr<Analyzer> >::iterator cur_analyzer;
    
    // main execution loop, so we want to catch exceptions as they are thrown and abort
    try
        {
        // read into memory from Frames or by overriden read()
        if (m_must_read_from_file)
            read();

        // sort and validate the frames
        sortFrames();
        validate();
        parse();
        
        // enter the compute loop
        // if computes should be cleaned up, they need to do this after they are done
        for (cur_analyzer = m_analyzers.begin(); cur_analyzer != m_analyzers.end(); ++cur_analyzer)
            {
            // perform the evaluation
            cur_analyzer->second->evaluate();
            }
        }
    catch (std::exception const & e)
        {
        std::cout<<e.what()<<std::endl;
        throw e;
        }
    }

//! Boost Python wrapper for the Trajectory
/*!
 * read() is a virtual function, so Boost Python needs an explicit wrapper for it. We want to be able to instantiate
 * plain Trajectory classes in case in the future the user could attach Frame content on the scripting level without
 * using a reader, so read() cannot be pure virtual. This means that we must default to an empty read() implementation.
 */
// struct TrajectoryWrap : public Trajectory, boost::python::wrapper<Trajectory>
//     {
//     TrajectoryWrap() : Trajectory() {}
//     
//     void read()
//         {
//         if (boost::python::override f = this->get_override("read"))
//             f();
//         else
//             Trajectory::read();
//         }
//         
//     void default_read() { this->Trajectory::read(); }
//     
//     void addFile(const std::string& file)
//         {
//         if (boost::python::override f = this->get_override("addFile"))
//             f(file);
//         else
//             Trajectory::addFile(file);
//         }
//     void default_addFile(const std::string& file) { this->Trajectory::addFile(file); }
//     };

void export_Trajectory()
    {
    using namespace boost::python;
    class_<Trajectory, boost::shared_ptr<Trajectory>, boost::noncopyable>("Trajectory")
    .def("analyze",&Trajectory::analyze)
    .def("addCompute",&Trajectory::addAnalyzer)
    .def("removeCompute",&Trajectory::removeAnalyzer)
    .def("getCompute",&Trajectory::getAnalyzer)
    .def("getFrame",&Trajectory::getFrame)
    .def("setBox",&Trajectory::setBox)
    .def("setNames",&Trajectory::setNames)
    .def("setDiameters",&Trajectory::setDiameters)
    .def("setMasses",&Trajectory::setMasses)
    .def("read", &Trajectory::read)
    .def("addFile", &Trajectory::addFile);
    }
