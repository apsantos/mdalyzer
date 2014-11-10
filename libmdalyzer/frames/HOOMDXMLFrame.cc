#include "HOOMDXMLFrame.h"
#include <sstream>

// static declaration of HOOMD version support
float HOOMDXMLFrame::s_supported_hoomd_version = 1.0;

HOOMDXMLFrame::HOOMDXMLFrame(const std::string& file)
    : m_file(file), m_num_particles(-1)
    {
    }

void HOOMDXMLFrame::readFromFile()
    {
    pugi::xml_document doc;
    pugi::xml_parse_result load_file = doc.load_file(m_file.c_str());
    if (load_file)
        {
        pugi::xml_node hoomd_xml = doc.child("hoomd_xml");
        
        float hoomd_version = hoomd_xml.attribute("version").as_float();
        if (hoomd_version < s_supported_hoomd_version)
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
                if (m_num_particles > 0)
                    {
                    m_positions.assign(m_num_particles,Vector3<double>());
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
                int cur_particle(0);
                while (pos_str >> pos_i.x >> pos_i.y >> pos_i.z)
                    {
                    // shift image if necessary
                    if (has_images)
                        {
                        Vector3<double> image_vec(0.,0.,0.);
                        img_str >> image_vec.x >> image_vec.y >> image_vec.z;
                        m_box.shiftImage(image_vec, pos_i);
                        }
                        
                    if (m_num_particles > 0) // it's faster to assign if we already know the array size
                        {
                        m_positions[cur_particle] = pos_i;
                        }
                    else
                        {
                        m_positions.push_back(pos_i);
                        }
                    ++cur_particle;
                    }
                    
                if (m_num_particles > 0 && m_num_particles != cur_particle)
                    {
                    // particle mismatch, error
                    }
                if (m_num_particles < 0)
                    {
                    m_num_particles = cur_particle;
                    }
                m_has_positions = true;
                }
                
            // process velocities
            node = config.child("velocity");
            if (node)
                {
                tryParticlesFromNode(node);
                
                if (m_num_particles > 0)
                    {
                    m_velocities.assign(m_num_particles,Vector3<double>());
                    }
                
                std::istringstream vel_str(config.child_value("velocity"));
                Vector3<double> vel_i;
                int cur_particle(0);
                while (vel_str >> vel_i.x >> vel_i.y >> vel_i.z)
                    {
                    if (m_num_particles > 0)
                        {
                        m_velocities[cur_particle] = vel_i;
                        }
                    else
                        {
                        m_velocities.push_back(vel_i);
                        }
                    }
                
                if (m_num_particles > 0 && m_num_particles != cur_particle)
                    {
                    // particle mismatch, error
                    }
                if (m_num_particles < 0)
                    {
                    m_num_particles = cur_particle;
                    }
                m_has_velocities = true;
                }
            }
        else
            {
            // your version is too old, die
            }
        }
    else
        {
        // failed to find file, raise hell with exceptions
        }
    }

inline void HOOMDXMLFrame::tryParticlesFromNode(pugi::xml_node node)
    {
    if (node.attribute("num"))
        {
            if (m_num_particles < 0)
                {
                m_num_particles = node.attribute("num").as_int();
                }
            else if(node.attribute("num").as_int() != m_num_particles)
                {
                // warn particle mismatch between sections
                }
        }
    }