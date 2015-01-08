/*!
 * \file XTCTrajectory.cc
 * \author Sang Beom Kim
 * \date 8 January 2015
 * \brief Boost unit tests for XTC reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Trajectory.h"

#include "XTCTrajectory.h"

BOOST_AUTO_TEST_SUITE(XTC)

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(XTC_read_frame)
    {
	XTCTrajectory traj(3);
	traj.addFile("test/unit/xtc/frame.xtc.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = false; config.type = false;
	
	test_read<XTCTrajectory>(traj,config);
    }

//! test all information is read from multiple files
BOOST_AUTO_TEST_CASE(XTC_read_multiframes)
    {
	XTCTrajectory traj(3);
	traj.addFile("test/unit/xtc/frame.xtc.1");
	traj.addFile("test/unit/xtc/frame.xtc.2");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = false; config.type = false;
	
	test_read<XTCTrajectory>(traj,config);
    }

//! test all information is read from a file with multiple frames
BOOST_AUTO_TEST_CASE(XTC_read_multiframes_singlefile)
    {
	XTCTrajectory traj(3);
	traj.addFile("test/unit/xtc/frame.xtc.singlefile");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = false; config.type = false;
	
	test_read<XTCTrajectory>(traj,config);
    }

BOOST_AUTO_TEST_CASE(XTC_exceptions)
    {
    // file not found
        {
        XTCTrajectory traj(3);
        traj.addFile("test/unit/xtc/not.a.file.xtc");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    }
    
BOOST_AUTO_TEST_SUITE_END()