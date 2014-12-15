/*! \file HOOMDXMLTrajectory.h
 *  \author Michael P. Howard
 */
#ifndef MDALYZER_TRAJECTORIES_HOOMDXMLTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_HOOMDXMLTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>
#include "pugixml.hpp"

/*! \class HOOMDXMLFrame
 *  \brief HOOMD XML parser
 *
 *  Extension of the Frame data structure that parses HOOMD XML output (v. 1.0.0 or more recent). The pugixml reader
 *  is used to 
 *
 *  \ingroup frames
 */
class HOOMDXMLTrajectory : public Trajectory
    {
    public:
        HOOMDXMLTrajectory();
        virtual ~HOOMDXMLTrajectory() {};
        
        void addFile(const std::string& f);
        
        virtual void read();
    private:
        static float s_supported_hoomd_version;
        
        std::vector<std::string> m_files;
        boost::shared_ptr<Frame> readFromFile(const std::string& f);
        
        inline void tryParticlesFromNode(pugi::xml_node node);
    };

void export_HOOMDXMLTrajectory();

#endif //MDALYZER_TRAJECTORIES_HOOMDXMLTRAJECTORY_H_
