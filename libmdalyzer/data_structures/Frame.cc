/*!
 * \file Frame.cc
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Implementation of Frame data structure
 */
#include "Frame.h"
#include <boost/python.hpp>

Frame::Frame(unsigned int n)
    : m_n_particles(n), m_has_time(false), m_has_box(false), m_has_positions(false),
    m_has_velocities(false), m_has_names(false), m_has_types(false), m_has_diameters(false), m_has_masses(false)
    {
    }

/*!
 * \param positions vector of particle positions
 */
void Frame::setPositions(const std::vector< Vector3<double> >& positions)
    {
    if (positions.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
        
    m_positions = positions;
    m_has_positions = true;
    }
/*!
 * \param pid particle id to set
 * \param pos particle position
 *
 * Allocator is called if the positions have not been set yet
 */
void Frame::setPosition(unsigned int pid, const Vector3<double>& pos)
    {   
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        if (!m_has_positions)
            {
            m_positions.assign(m_n_particles, Vector3<double>());
            }
        m_positions[pid] = pos;
        m_has_positions = true;
        }
    }
 
/*!
 * \param velocities vector of particle velocities
 */   
void Frame::setVelocities(const std::vector< Vector3<double> >& velocities)
    {
    if (velocities.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_velocities = velocities;
    m_has_velocities = true;
    }
/*!
 * \param pid particle id to set
 * \param vel particle velocity
 *
 * Allocator is called if the velocities have not been set yet
 */
void Frame::setVelocity(unsigned int pid, const Vector3<double>& vel)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        if (!m_has_velocities)
            {
            m_velocities.assign(m_n_particles, Vector3<double>());
            }
        m_velocities[pid] = vel;
        m_has_velocities = true;
        }
    }
/*!
 * \param names vector of particle names
 */    
void Frame::setNames(const std::vector<std::string>& names)
    {
    if (names.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_names = names;
    m_has_names = true;
    }
/*!
 * \param pid particle id to set
 * \param name particle name
 *
 * Allocator is called if the names have not been set yet
 */
void Frame::setName(unsigned int pid, const std::string& name)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        if (!m_has_names)
            {
            m_names.assign(m_n_particles, std::string());
            }
        m_names[pid] = name;
        m_has_names = true;
        }
    }
    
/*!
 * \param types vector of particle types
 */    
void Frame::setTypes(const std::vector<unsigned int>& types)
    {
    if (types.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_types = types;
    m_has_types = true;
    }
/*!
 * \param pid particle id to set
 * \param type particle type id
 *
 * Allocator is called if the types have not been set yet
 */
void Frame::setType(unsigned int pid, unsigned int type)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        if (!m_has_types)
            {
            m_types.assign(m_n_particles,0);
            }
        m_types[pid] = type;
        m_has_types = true;
        }
    }

/*!
 * \param diams vector of particle diameters
 */      
void Frame::setDiameters(const std::vector<double>& diams)
    {
    if (diams.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_diameters = diams;
    m_has_diameters = true;
    }
/*!
 * \param pid particle id to set
 * \param diam particle diameter
 *
 * Allocator is called if the diameters have not been set yet
 */
void Frame::setDiameter(unsigned int pid, double diam)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        if (!m_has_diameters)
            {
            m_diameters.assign(m_n_particles,0.0);
            }
        m_diameters[pid] = diam;
        m_has_diameters = true;
        }
    }

/*!
 * \param masses vector of particle masses
 */  
void Frame::setMasses(const std::vector<double>& masses)
    {
    if (masses.size() != m_n_particles)
        throw std::runtime_error("Frame: set error, vector size must match number of particles");
    m_masses = masses;
    m_has_masses = true;
    }
/*!
 * \param pid particle id to set
 * \param type particle mass
 *
 * Allocator is called if the masses have not been set yet
 */
void Frame::setMass(unsigned int pid, double mass)
    {
    if (pid >= m_n_particles)
        throw std::runtime_error("Frame: set error, inadequate memory allocated");
    else
        {
        if (!m_has_masses)
            {
            m_masses.assign(m_n_particles, 0.0);
            }
        m_masses[pid] = mass;
        m_has_masses = true;
        }
    }
  
/*! \return snapshot time */  
double Frame::getTime() const
    {
    if (!m_has_time)
        throw std::runtime_error("Frame: time not set");
    return m_time;
    }
/*! \return snapshot simulation box */
const TriclinicBox& Frame::getBox() const
    {
    if (!m_has_box)
        throw std::runtime_error("Frame: box not set");
    return m_box;
    }
/*! \return list of particle positions */
const std::vector< Vector3<double> >& Frame::getPositions() const
    {
    if (!m_has_positions)
        throw std::runtime_error("Frame: positions not set");
    return m_positions;
    }
/*! \return list of particle velocities */
const std::vector< Vector3<double> >& Frame::getVelocities() const
    {
    if (!m_has_velocities)
        throw std::runtime_error("Frame: velocities not set");
    return m_velocities;
    }
/*! \return list of particle names */
const std::vector<std::string>& Frame::getNames() const
    {
    if (!m_has_names)
        throw std::runtime_error("Frame: names not set");
    return m_names;
    }
/*! \return list of particle types */
const std::vector<unsigned int>& Frame::getTypes() const
    {
    if (!m_has_types)
        throw std::runtime_error("Frame: types not set");
    return m_types;
    }
/*! \return list of particle diameters */
const std::vector<double>& Frame::getDiameters() const
    {
    if (!m_has_diameters)
        throw std::runtime_error("Frame: diameters not set");
    return m_diameters;
    }
/*! \return list of particle masses */
const std::vector<double>& Frame::getMasses() const
    {
    if (!m_has_masses)
        throw std::runtime_error("Frame: masses not set");
    return m_masses;
    }
    
void export_Frame()
    {
    using namespace boost::python;
    class_<Frame, boost::noncopyable, boost::shared_ptr<Frame> >("Frame", init<unsigned int>())
    .def("getTime", &Frame::getTime);
    }
