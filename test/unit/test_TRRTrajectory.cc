/*!
 * \file TRRTrajectory.cc
 * \author Sang Beom Kim
 * \date 8 January 2015
 * \brief Boost unit tests for TRR reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Trajectory.h"

#include "TRRTrajectory.h"

BOOST_AUTO_TEST_SUITE(TRR)

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(TRR_read_frame)
    {
	TRRTrajectory traj(3);
	traj.addFile("test/unit/trr/frame.trr.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true; config.type = false;
	
	test_read<TRRTrajectory>(traj,config);
    }

//! test all information is read from multiple files
BOOST_AUTO_TEST_CASE(TRR_read_multiframes)
    {
	TRRTrajectory traj(3);
	traj.addFile("test/unit/trr/frame.trr.1");
	traj.addFile("test/unit/trr/frame.trr.2");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true; config.type = false;
	
	test_read<TRRTrajectory>(traj,config);
    }

//! test all information is read from a file with multiple frames
BOOST_AUTO_TEST_CASE(TRR_read_multiframes_singlefile)
    {
	TRRTrajectory traj(3);
	traj.addFile("test/unit/trr/frame.trr.singlefile");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.velocity = true; config.type = false;
	
	test_read<TRRTrajectory>(traj,config);
    }

BOOST_AUTO_TEST_CASE(TRR_exceptions)
    {
    // file not found
        {
        TRRTrajectory traj(3);
        traj.addFile("test/unit/trr/not.a.file.trr");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    }
    
BOOST_AUTO_TEST_SUITE_END()