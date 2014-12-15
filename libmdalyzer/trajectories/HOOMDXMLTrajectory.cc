/*! \file HOOMDXMLTrajectory.cc
 *  \author Michael P. Howard
 *  \brief Reads HOOMD XML files
 */
#include "HOOMDXMLTrajectory.h"

#include <boost/python.hpp>
#include <sstream>
#include <iostream>

HOOMDXMLTrajectory::HOOMDXMLTrajectory()
    {
    }

void HOOMDXMLTrajectory::read()
    {
    for (unsigned int cur_f = 0; cur_f < m_files.size(); ++cur_f)
        {
        m_frames.push_back(readFromFile(m_files[cur_f]));
        }
    }

void HOOMDXMLTrajectory::addFile(const std::string& f)
    {
    m_files.push_back(f); // error check this later
    }

/*! Main routine to parse out the necessary information from file (v.1.0 supported)
 */
boost::shared_ptr<Frame> HOOMDXMLTrajectory::readFromFile(const std::string& f)
    {
    static float s_supported_hoomd_version = 1.0;
    
    boost::shared_ptr<Frame> cur_frame; //default initialization is null ptr
    
    pugi::xml_document doc;
    pugi::xml_parse_result load_file = doc.load_file(f.c_str());
    if (load_file)
        {
        pugi::xml_node hoomd_xml = doc.child("hoomd_xml");
        
        float hoomd_version = hoomd_xml.attribute("version").as_float();
        if (hoomd_version >= s_supported_hoomd_version)
            {
            
            // obtain the configuration and validate time is set
            pugi::xml_node config = hoomd_xml.child("configuration");
            if (!config.attribute("time_step"))
                {
                throw std::runtime_error("HOOMDXMLTrajectory: frames must have time set");
                }
            
            // process simulation box
            pugi::xml_node node = config.child("box");
            TriclinicBox box;
            if (node)
                {
                // construct the simulation box
                Vector3<double> length;
                if (node.attribute("lx") && node.attribute("ly") && node.attribute("lz"))
                    {
                    length.x = node.attribute("lx").as_double();
                    length.y = node.attribute("ly").as_double();
                    length.z = node.attribute("lz").as_double();
                    }
                else
                    {
                    // all box components need to be specified, badly formed xml
                    throw std::runtime_error("HOOMDXMLTrajectory: poorly formed xml, all box lengths must be set");
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
                    
                box = TriclinicBox(length,tilt);
                }
            else
                {
                throw std::runtime_error("HOOMDXMLTrajectory: frame requires a box");
                }
                
            // process positions
            node = config.child("position");
            if (node)
                {   
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
                std::vector<Vector3<double> > positions;
                unsigned int n_particles(0);
                while (pos_str >> pos_i.x >> pos_i.y >> pos_i.z)
                    {
                    // shift image if necessary
                    if (has_images)
                        {
                        Vector3<double> image_vec(0.,0.,0.);
                        img_str >> image_vec.x >> image_vec.y >> image_vec.z;
                        box.shiftImage(image_vec, pos_i);
                        }
                        
                    positions.push_back(pos_i);
                    ++n_particles;
                    }
                
                cur_frame = boost::shared_ptr<Frame>( new Frame(n_particles) );
                cur_frame->setPositions(positions);
                }
                
            // process velocities
            node = config.child("velocity");
            if (node)
                {
                std::vector< Vector3<double> > velocities; // in case the current frame isn't built yet
                std::istringstream vel_str(config.child_value("velocity"));
                
                Vector3<double> vel_i;
                unsigned int cur_particle(0);
                while (vel_str >> vel_i.x >> vel_i.y >> vel_i.z)
                    {
                    if (!cur_frame)
                        velocities.push_back(vel_i);
                    else
                        cur_frame->setVelocity(cur_particle, vel_i);
                        
                    ++cur_particle;
                    }
                    
                if (!cur_frame)
                    {
                    cur_frame = boost::shared_ptr<Frame>( new Frame(cur_particle) );
                    cur_frame->setVelocities(velocities);
                    }
                }
            
            // process masses    
            node = config.child("mass");
            if (node)
                {
                std::vector<double> masses; // in case the current frame isn't built yet
                std::istringstream mass_str(config.child_value("mass"));
                double mass_i;
                unsigned int cur_particle(0);
                while (mass_str >> mass_i)
                    {
                    if (!cur_frame)
                        masses.push_back(mass_i);
                    else
                        cur_frame->setMass(cur_particle, mass_i);
                        
                    ++cur_particle;
                    }
                
                if (!cur_frame)
                    {
                    cur_frame = boost::shared_ptr<Frame>( new Frame(cur_particle) );
                    cur_frame->setMasses(masses);
                    }
                }
            
            // process diameters
            node = config.child("diameter");
            if (node)
                {
                std::vector<double> diameters; // in case the current frame isn't built yet
                std::istringstream diam_str(config.child_value("diameter"));
                double diam_i;
                unsigned int cur_particle(0);
                while (diam_str >> diam_i)
                    {
                    if (!cur_frame)
                        diameters.push_back(diam_i);
                    else
                        cur_frame->setDiameter(cur_particle, diam_i);
                    ++cur_particle;
                    }
                    
                if (!cur_frame)
                    {
                    cur_frame = boost::shared_ptr<Frame>( new Frame(cur_particle) );
                    cur_frame->setDiameters(diameters);
                    }
                }
            
             if (cur_frame)
                {
                cur_frame->setTime(config.attribute("time_step").as_double());
                cur_frame->setBox(box);                    
                }   
                
            }
        else
            {
            // your version is too old, die
            throw std::runtime_error("HOOMDXMLTrajectory: version is too old!");
            }
        }
    else
        {
        // failed to find file, raise hell with exceptions
        throw std::runtime_error("HOOMDXMLTrajectory: file not found");
        }
    return cur_frame;
    }

//! Python export for HOOMDXMLTrajectory
void export_HOOMDXMLTrajectory()
    {
    using namespace boost::python;
    class_<HOOMDXMLTrajectory, boost::shared_ptr<HOOMDXMLTrajectory>, bases<Trajectory>, boost::noncopyable >
    ("HOOMDXMLTrajectory", init< >())
    .def("addFile", &HOOMDXMLTrajectory::addFile);
    }
