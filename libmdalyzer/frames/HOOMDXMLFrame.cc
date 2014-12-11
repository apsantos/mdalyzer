/*! \file HOOMDXMLFrame.cc
 *  \author Michael P. Howard
 *  \brief Reads HOOMD XML files
 */
#include "HOOMDXMLFrame.h"

#include <boost/python.hpp>
#include <sstream>

/*! \param file Path to xml file to read
 */
HOOMDXMLFrame::HOOMDXMLFrame(const std::string& file)
    : m_file(file)
    {
    }

/*! Main routine to parse out the necessary information from file (v.1.0 supported)
 */
void HOOMDXMLFrame::readFromFile()
    {
    // static declaration of HOOMD version support
    static float s_supported_hoomd_version = 1.0;
    
    pugi::xml_document doc;
    pugi::xml_parse_result load_file = doc.load_file(m_file.c_str());
    if (load_file)
        {
        pugi::xml_node hoomd_xml = doc.child("hoomd_xml");
        
        float hoomd_version = hoomd_xml.attribute("version").as_float();
        if (hoomd_version >= s_supported_hoomd_version)
            {
            // obtain the configuration and time
            pugi::xml_node config = hoomd_xml.child("configuration");
            setTime(config.attribute("time_step").as_double());
            
            // process simulation box
            pugi::xml_node node = config.child("box");
            if (node)
                {
                // construct the simulation box
                Vector3<double> length(0.,0.,0.);
                if (node.attribute("lx") && node.attribute("ly") && node.attribute("lz"))
                    {
                    length.x = node.attribute("lx").as_double();
                    length.y = node.attribute("ly").as_double();
                    length.z = node.attribute("lz").as_double();
                    }
                else
                    {
                    // all box components need to be specified, badly formed xml
                    throw std::runtime_error("HOOMDXMLFrame: poorly formed xml, all box lengths must be set");
                    }
                                       
                
                // check for tilt (version 1.5 and newer)
                Vector3<double> tilt(0.,0.,0.);
                if (hoomd_version >= 1.5)
                    {
                    if (node.attribute("xy"))
                        {
                        tilt.x = node.attribute("xy").as_double();
                        }
                    if (node.attribute("xz"))
                        {
                        tilt.y = node.attribute("xz").as_double();
                        }
                    if (node.attribute("yz"))
                        {
                        tilt.z = node.attribute("yz").as_double();
                        }
                    }
                    
                m_box = TriclinicBox(length,tilt);
                m_has_box = true;
                }
                
            // process positions
            node = config.child("position");
            if (node)
                {
                tryParticlesFromNode(node);
                // reserve space if the number of positions is specified, only if this hasn't been done yet
                if (m_n_particles > 0)
                    {
                    m_positions.assign(m_n_particles,Vector3<double>());
                    }
                    
                // check if images are included, and wrap the positions if so. We can always get periodic images
                // be rewrapping later
                bool has_images = (config.child("image")) ? true : false;
                std::istringstream img_str;
                if (has_images)
                    {
                    img_str.str(config.child_value("image"));
                    }
                    
                std::istringstream pos_str(config.child_value("position"));
                Vector3<double> pos_i;
                unsigned int cur_particle(0);
                while (pos_str >> pos_i.x >> pos_i.y >> pos_i.z)
                    {
                    // shift image if necessary
                    if (has_images)
                        {
                        Vector3<double> image_vec(0.,0.,0.);
                        img_str >> image_vec.x >> image_vec.y >> image_vec.z;
                        m_box.shiftImage(image_vec, pos_i);
                        }
                        
                    fasterPushBack(pos_i, cur_particle, m_positions);
                    ++cur_particle;
                    }
                    
                updateParticleCount(cur_particle);
                m_has_positions = true;
                }
                
            // process velocities
            node = config.child("velocity");
            if (node)
                {
                tryParticlesFromNode(node);
                
                if (m_n_particles > 0)
                    {
                    m_velocities.assign(m_n_particles,Vector3<double>());
                    }
                
                std::istringstream vel_str(config.child_value("velocity"));
                Vector3<double> vel_i;
                unsigned int cur_particle(0);
                while (vel_str >> vel_i.x >> vel_i.y >> vel_i.z)
                    {
                    fasterPushBack(vel_i, cur_particle, m_velocities);
                    ++cur_particle;
                    }
                
                updateParticleCount(cur_particle);
                m_has_velocities = true;
                }
                
            // process masses
            node = config.child("mass");
            if (node)
                {
                tryParticlesFromNode(node);
                
                if (m_n_particles > 0)
                    {
                    m_masses.assign(m_n_particles,0.0);
                    }
                
                std::istringstream mass_str(config.child_value("mass"));
                double mass_i;
                unsigned int cur_particle(0);
                while (mass_str >> mass_i)
                    {
                    fasterPushBack(mass_i, cur_particle, m_masses);
                    ++cur_particle;
                    }
                
                updateParticleCount(cur_particle);
                m_has_masses = true;
                }
            
            // process diameters
            node = config.child("diameter");
            if (node)
                {
                tryParticlesFromNode(node);
                
                if (m_n_particles > 0)
                    {
                    m_diameters.assign(m_n_particles,0.0);
                    }
                
                std::istringstream diam_str(config.child_value("diameter"));
                double diam_i;
                unsigned int cur_particle(0);
                while (diam_str >> diam_i)
                    {
                    fasterPushBack(diam_i, cur_particle, m_diameters);
                    ++cur_particle;
                    }
                
                updateParticleCount(cur_particle);
                m_has_diameters = true;
                }
                
            // process types
            node = config.child("type");
            if (node)
                {
                tryParticlesFromNode(node);
                
                if (m_n_particles > 0)
                    {
                    m_types.assign(m_n_particles,"");
                    }
                std::istringstream type_str(config.child_value("type"));
                std::string type_i;
                unsigned int cur_particle(0);
                while (type_str >> type_i)
                    {
                    fasterPushBack(type_i, cur_particle, m_types);
                    ++cur_particle;
                    }
                
                updateParticleCount(cur_particle);
                m_has_types = true;
                }
            }
        else
            {
            // your version is too old, die
            throw std::runtime_error("HOOMDXMLFrame version is too old!");
            }
        }
    else
        {
        // failed to find file, raise hell with exceptions
        throw std::runtime_error("HOOMDXMLFrame file not found");
        }
    }

//! Try to update the particle count using the pugixml node
/*! \param node Node that may have a num attribute
 */
inline void HOOMDXMLFrame::tryParticlesFromNode(pugi::xml_node node)
    {
    if (node.attribute("num"))
        {
        updateParticleCount(node.attribute("num").as_uint());
        }
    }
//! Update/validate the particle count. We don't allow changes.
/*! \param n_particle Current number of particles to try to update to
 */ 
inline void HOOMDXMLFrame::updateParticleCount(unsigned int n_particle)
    {
    if (m_n_particles > 0 && m_n_particles != n_particle)
        {
        // particle mismatch, error
        throw std::runtime_error("HOOMDXMLFrame found differing numbers of particle between sections");
        }
    if (m_n_particles == 0)
        {
        m_n_particles = n_particle;
        }
    }

//! Perform a "push back" on a vector, but prefer to write by index first, which was faster in microbenchmarks
/*! \param val Item to push back
 *  \param cur_particle Current particle id
 *  \param vec STL vector for output
 */
template<typename T>
inline void HOOMDXMLFrame::fasterPushBack(const T val, unsigned int cur_particle, std::vector<T>& vec)
    {
    if (m_n_particles > 0 && cur_particle < m_n_particles)
        {
        vec[cur_particle] = val;
        }
    else
        {
        vec.push_back(val);
        }
    }

//! Python export for HOOMDXMLFrame       
void export_HOOMDXMLFrame()
    {
    using namespace boost::python;
    class_<HOOMDXMLFrame, boost::shared_ptr<HOOMDXMLFrame>, bases<Frame>, boost::noncopyable >
    ("HOOMDXMLFrame", init< const std::string& >());
    }
    