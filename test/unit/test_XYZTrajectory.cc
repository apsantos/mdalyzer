/*!
 * \file test_XYZTrajectory.cc
 * \author Michael P. Howard
 * \date 8 January 2015
 * \brief Boost unit tests for XYZ reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Trajectory.h"

#include "XYZTrajectory.h"

BOOST_AUTO_TEST_SUITE(XYZ)

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(read_frame)
    {
	XYZTrajectory traj;
	traj.addFile("test/unit/xyz/frame.xyz.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.position = true; config.type = true;
	
	test_read<XYZTrajectory>(traj,config);
    }

//! test all information is read from multiple files
BOOST_AUTO_TEST_CASE(read_multi_files)
    {
	XYZTrajectory traj;
	traj.addFile("test/unit/xyz/frame.xyz.1");
	traj.addFile("test/unit/xyz/frame.xyz.2");
	traj.analyze();
	
	TrajectoryCheck config;
	config.position = true; config.type = true;
	
	test_read<XYZTrajectory>(traj,config);
    }

//! test all information is read from a single file
BOOST_AUTO_TEST_CASE(read_multi_frames)
    {
	XYZTrajectory traj;
	traj.addFile("test/unit/xyz/frame.xyz.singlefile");
	traj.analyze();
	
	TrajectoryCheck config;
	config.position = true; config.type = true;
	
	test_read<XYZTrajectory>(traj,config);
    }

//! test exception handling
BOOST_AUTO_TEST_CASE(exceptions)
    {
    // file not found
        {
        XYZTrajectory traj;
        traj.addFile("test/unit/xyz/not.a.file.xyz");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // number not set
        {
        XYZTrajectory traj;
        traj.addFile("test/unit/xyz/frame.xyz.noN");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // time not set in first frame is fine
        {
        XYZTrajectory traj;
        traj.addFile("test/unit/xyz/frame.xyz.time");    
        BOOST_CHECK_NO_THROW(traj.analyze());
        } 
    // time not set in second frame
        {
        XYZTrajectory traj;
        traj.addFile("test/unit/xyz/frame.xyz.time_missing");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        } 
    // particle is missing a position
        {
        XYZTrajectory traj;
        traj.addFile("test/unit/xyz/frame.xyz.error");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        } 
    }
    
BOOST_AUTO_TEST_SUITE_END()
