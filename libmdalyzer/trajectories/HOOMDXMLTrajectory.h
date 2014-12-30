/*!
 * \file HOOMDXMLTrajectory.h
 * \author Michael P. Howard
 * \date 16 December 2014
 * \brief Declaration of HOOMDXMLTrajectory reader
 */
#ifndef MDALYZER_TRAJECTORIES_HOOMDXMLTRAJECTORY_H_
#define MDALYZER_TRAJECTORIES_HOOMDXMLTRAJECTORY_H_

#include "Trajectory.h"
#include <string>
#include <vector>
#include "pugixml.hpp"

//! HOOMD XML parser
/*!
 *  Implementation of the Trajectory for HOOMD XML file format (v. >= 1.0 support). The pugixml reader
 *  is used to manipulate the XML files.
 *
 *  \ingroup trajectories
 */
class HOOMDXMLTrajectory : public Trajectory
    {
    public:
        //! default constructor
        HOOMDXMLTrajectory() {};
        
        //! default destructor
        virtual ~HOOMDXMLTrajectory() {};
        
        //! reads all attached files into Frame
        virtual void read();
    private:
        static float s_supported_hoomd_version;     //!< Flag for HOOMD support level
        
        //! internal method for reading a single HOOMD XML file into a Frame
        boost::shared_ptr<Frame> readFromFile(const std::string& f);
    };

//! Python export for HOOMDXMLTrajectory
void export_HOOMDXMLTrajectory();

#endif //MDALYZER_TRAJECTORIES_HOOMDXMLTRAJECTORY_H_
