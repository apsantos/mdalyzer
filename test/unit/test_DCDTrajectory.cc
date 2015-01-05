/*!
 * \file test_HOOMDXMLTrajectory.cc
 * \author Andrew P. Santos
 * \date 5 January 2015
 * \brief Boost unit tests for DCD reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Trajectory.h"

#include "Frame.h"
#include "TriclinicBox.h"
#include "VectorMath.h"
#include "Trajectory.h"
#include "HOOMDXMLTrajectory.h"
#include "DCDTrajectory.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

BOOST_AUTO_TEST_SUITE(DCD)
//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(HOOMDXML_read_frame)
    {
	HOOMDXMLTrajectory traj;
	traj.addFile("test/unit/hoomd/frame.xml.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true;
	config.type = true; config.diameter = true; config.mass = true;
	
	test_read<HOOMDXMLTrajectory>(traj,config);
    }
//! test all information is read from multiple frames
BOOST_AUTO_TEST_CASE(HOOMDXML_read_multiframes)
    {
    HOOMDXMLTrajectory traj;
    traj.addFile("test/unit/hoomd/frame.xml.1");
    traj.addFile("test/unit/hoomd/frame.xml.2");
    traj.analyze();
    
    TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true;
	config.type = true; config.diameter = true; config.mass = true;
    
    test_multiframe<HOOMDXMLTrajectory>(traj, config);
    }
//! test file can be read if positions are not set (special first field)
BOOST_AUTO_TEST_CASE(HOOMDXML_read_only_velocity)
    {
    HOOMDXMLTrajectory traj;
    traj.addFile("test/unit/hoomd/frame.xml.only_velocity");
    traj.analyze();

    boost::shared_ptr<Frame> frame = traj.getFrame(0);    
    BOOST_REQUIRE(frame->hasVelocities());
        
    std::vector< Vector3<double> > velocities = frame->getVelocities();
    BOOST_CHECK_CLOSE(velocities[0].x, 3.0, test_very_close);
    BOOST_CHECK_CLOSE(velocities[0].y, -2.0, test_very_close);
    BOOST_CHECK_CLOSE(velocities[0].z, 1.0, test_very_close);
    
    BOOST_CHECK_CLOSE(velocities[1].x, -1.0, test_very_close);
    BOOST_CHECK_CLOSE(velocities[1].y, 2.0, test_very_close);
    BOOST_CHECK_CLOSE(velocities[1].z, -3.0, test_very_close);
    }
//! test exceptions are thrown for badly formed xml
BOOST_AUTO_TEST_CASE(HOOMDXML_exceptions)
    {
    BOOST_TEST_MESSAGE("Testing exception throws of HOOMDXMLTrajectory, expect 6 output messages");
    // file not found
        {
        HOOMDXMLTrajectory traj;
        traj.addFile("test/unit/hoomd/not.a.file.xml");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    
    // version too old
        {
        HOOMDXMLTrajectory traj;
        traj.addFile("test/unit/hoomd/frame.xml.version");
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    
    // missing timestep
        {
        HOOMDXMLTrajectory traj;
        traj.addFile("test/unit/hoomd/frame.xml.timestep");
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }  
    
    // no box
        {
        HOOMDXMLTrajectory traj;
        traj.addFile("test/unit/hoomd/frame.xml.nobox");
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }  
        
    // missing box edge
        {
        HOOMDXMLTrajectory traj;
        traj.addFile("test/unit/hoomd/frame.xml.box");
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    
    // velocity mismatch
        {
        HOOMDXMLTrajectory traj;
        traj.addFile("test/unit/hoomd/frame.xml.velocity");
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        } 
    }
BOOST_AUTO_TEST_SUITE_END()
