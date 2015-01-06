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
#include "DCDTrajectory.h"
#include "HOOMDXMLTrajectory.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

BOOST_AUTO_TEST_SUITE(DCD)
//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(DCD_read_frame)
    {
	HOOMDXMLTrajectory traj(1.0);
	traj.addFile("test/unit/hoomd/frame.xml.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.position = true;
	config.type = true; config.diameter = true; config.mass = true;
	
	test_read<HOOMDXMLTrajectory>(traj,config);
    }
//! test all information is read from multiple frames
BOOST_AUTO_TEST_CASE(DCD_read_multiframes)
    {
	boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
    i_traj->addFile("test/unit/hoomd/frame.xml.1");
	DCDTrajectory traj(i_traj, "test/unit/dcd/frame.dcd.2", 2.5, 1);
    traj.analyze();
    
    TrajectoryCheck config;
	config.position = true;
    
    test_multiframe<DCDTrajectory>(traj, config);
    }
/*! test that the DCD reader defaults to the output frequency and timestep to what is 
 * in the DCD file, if the user specifies 0
 */
BOOST_AUTO_TEST_CASE(DCD_read_default)
    {
	boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
    i_traj->addFile("test/unit/hoomd/frame.xml.1");
	DCDTrajectory traj(i_traj, "test/unit/dcd/frame.dcd.2", 0, 0);
    traj.analyze();
    
    std::vector< boost::shared_ptr<Frame> > frames = traj.getFrames();
    BOOST_CHECK_CLOSE(frames[1]->getTime(), 1.0, test_very_close);
    }
BOOST_AUTO_TEST_SUITE_END()
