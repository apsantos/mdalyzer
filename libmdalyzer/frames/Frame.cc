/*! \file Frame.cc
 *  \author Michael P. Howard
 *  \brief Implementation of Frame data structure
 */
#include "Frame.h"
#include <boost/python.hpp>

Frame::Frame()
    : m_time(-1.0), m_n_particles(0), m_has_time(false), m_has_positions(false), m_has_velocities(false), m_has_types(false),
    m_has_diameters(false), m_has_masses(false)
    {
    }
    
//! Get the number of particles by checking Frame data if it is not yet set
unsigned int Frame::getN()
    {
    if (m_n_particles == 0)
        {
        if (m_has_positions) m_n_particles = m_positions.size();
        else if (m_has_velocities) m_n_particles = m_velocities.size();
        else if (m_has_types) m_n_particles = m_types.size();
        else if (m_has_diameters) m_n_particles = m_diameters.size();
        else if (m_has_masses) m_n_particles = m_masses.size();
        }
    return m_n_particles;
    }
    
//! Empty implementation of reader for generic Frame
void Frame::readFromFile()
    {
    }
    
//! Boost Python export of Frame
void export_Frame()
    {
    using namespace boost::python;
    class_<Frame, boost::noncopyable, boost::shared_ptr<Frame> >("Frame", init< >())
    .def("readFromFile",&Frame::readFromFile)
    .def("getTime",&Frame::getTime);
    }
