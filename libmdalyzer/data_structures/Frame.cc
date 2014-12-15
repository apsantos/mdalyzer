/*! \file Frame.cc
 *  \author Michael P. Howard
 *  \brief Implementation of Frame data structure
 */
#include "Frame.h"
#include <boost/python.hpp>

Frame::Frame(unsigned int n)
    : m_n_particles(n), m_has_time(false), m_has_box(false), m_has_positions(false),
    m_has_velocities(false), m_has_types(false), m_has_diameters(false), m_has_masses(false)
    {
    
    m_positions.assign(n, Vector3<double>());
    m_velocities.assign(n, Vector3<double>());
    m_names.assign(n, "");
    m_types.assign(n, 0);
    m_diameters.assign(n, 0.0);
    m_masses.assign(n, 0.0);
    }

void Frame::setPositions(const std::vector< Vector3<double> >& positions)
    {
    if (positions.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
        
    m_positions = positions;
    m_has_positions = true;
    }
void Frame::setPosition(unsigned int pid, const Vector3<double>& pos)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        m_positions[pid] = pos;
        m_has_positions = true;
        }
    }
    
void Frame::setVelocities(const std::vector< Vector3<double> >& velocities)
    {
    if (velocities.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_velocities = velocities;
    m_has_velocities = true;
    }
void Frame::setVelocity(unsigned int pid, const Vector3<double>& vel)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        m_velocities[pid] = vel;
        m_has_velocities = true;
        }
    }
    
void Frame::setNames(const std::vector<std::string>& names)
    {
    if (names.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_names = names;
    m_has_names = true;
    }
void Frame::setName(unsigned int pid, const std::string& name)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        m_names[pid] = name;
        m_has_names = true;
        }
    }
    
void Frame::setTypes(const std::vector<unsigned int>& types)
    {
    if (types.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_types = types;
    m_has_types = true;
    }
void Frame::setType(unsigned int pid, unsigned int type)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        m_types[pid] = type;
        m_has_types = true;
        }
    }
    
void Frame::setDiameters(const std::vector<double>& diams)
    {
    if (diams.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_diameters = diams;
    m_has_diameters = true;
    }
void Frame::setDiameter(unsigned int pid, double diam)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        m_diameters[pid] = diam;
        m_has_diameters = true;
        }
    }

void Frame::setMasses(const std::vector<double>& masses)
    {
    if (masses.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_masses = masses;
    m_has_masses = true;
    }
void Frame::setMass(unsigned int pid, double mass)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        m_masses[pid] = mass;
        m_has_masses = true;
        }
    }
    
double Frame::getTime() const
    {
    if (!m_has_time)
        throw std::runtime_error("Frame: time not set");
    return m_time;
    }
const TriclinicBox& Frame::getBox() const
    {
    if (!m_has_box)
        throw std::runtime_error("Frame: box not set");
    return m_box;
    }
const std::vector< Vector3<double> >& Frame::getPositions() const
    {
    if (!m_has_positions)
        throw std::runtime_error("Frame: positions not set");
    return m_positions;
    }
const std::vector< Vector3<double> >& Frame::getVelocities() const
    {
    if (!m_has_velocities)
        throw std::runtime_error("Frame: velocities not set");
    return m_velocities;
    }
const std::vector<std::string>& Frame::getNames() const
    {
    if (!m_has_names)
        throw std::runtime_error("Frame: names not set");
    return m_names;
    }
const std::vector<unsigned int> Frame::getTypes() const
    {
    if (!m_has_types)
        throw std::runtime_error("Frame: types not set");
    return m_types;
    }
const std::vector<double> Frame::getDiameters() const
    {
    if (!m_has_diameters)
        throw std::runtime_error("Frame: diameters not set");
    return m_diameters;
    }
const std::vector<double> Frame::getMasses() const
    {
    if (!m_has_masses)
        throw std::runtime_error("Frame: masses not set");
    return m_masses;
    }
    
//! Boost Python export of Frame
void export_Frame()
    {
    using namespace boost::python;
    class_<Frame, boost::noncopyable, boost::shared_ptr<Frame> >("Frame", init<unsigned int>())
    .def("getTime", &Frame::getTime);
    }
