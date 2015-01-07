/*!
 * \file GROTrajectory.cc
 * \author Michael P. Howard
 * \date 6 January 2015
 * \brief Boost unit tests for GRO reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Trajectory.h"

#include "GROTrajectory.h"

BOOST_AUTO_TEST_SUITE(GRO)

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(GRO_read_frame)
    {
	GROTrajectory traj(3);
	traj.addFile("test/unit/gro/frame.gro.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true; config.type = true;
	
	test_read<GROTrajectory>(traj,config);
    }

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(GRO_read_multiframes)
    {
	GROTrajectory traj(3);
	traj.addFile("test/unit/gro/frame.gro.1");
	traj.addFile("test/unit/gro/frame.gro.2");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true; config.type = true;
	
	test_read<GROTrajectory>(traj,config);
    }

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(GRO_read_multiframes_singlefile)
    {
	GROTrajectory traj(3);
	traj.addFile("test/unit/gro/frame.gro.singlefile");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true; config.type = true;
	
	test_read<GROTrajectory>(traj,config);
    }

BOOST_AUTO_TEST_CASE(GRO_exceptions)
    {
    // file not found
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/not.a.file.gro");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // no time label
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/frame.gro.notime");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // string follows t=
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/frame.gro.timestr");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // num particles set
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/frame.gro.noN");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // particle index out of range
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/frame.gro.badidx");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // missing a line
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/frame.gro.short");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // missing box
        {
        GROTrajectory traj(3);
        traj.addFile("test/unit/gro/frame.gro.box");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    }
    
BOOST_AUTO_TEST_SUITE_END()