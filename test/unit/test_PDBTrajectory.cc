/*!
 * \file test_PDBTrajectory.cc
 * \author Michael P. Howard
 * \date 8 January 2015
 * \brief Boost unit tests for PDB reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Trajectory.h"

#include "PDBTrajectory.h"

BOOST_AUTO_TEST_SUITE(PDB)

//! test all information is read from a single frame
BOOST_AUTO_TEST_CASE(read_frame)
    {
	PDBTrajectory traj(2.5);
	traj.addFile("test/unit/pdb/frame.pdb.1");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.type = true;
	
	test_read<PDBTrajectory>(traj,config);
    }

//! test all information is read from multiple files
BOOST_AUTO_TEST_CASE(read_multi_files)
    {
	PDBTrajectory traj(2.5);
	traj.addFile("test/unit/pdb/frame.pdb.1");
	traj.addFile("test/unit/pdb/frame.pdb.2");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.type = true;
	
	test_read<PDBTrajectory>(traj,config);
    }

//! test all information is read from a single file
BOOST_AUTO_TEST_CASE(read_multi_frames)
    {
	PDBTrajectory traj(2.5);
	traj.addFile("test/unit/pdb/frame.pdb.singlefile");
	traj.analyze();
	
	TrajectoryCheck config;
	config.box = true; config.position = true; config.type = true;
	
	test_read<PDBTrajectory>(traj,config);
    }

//! test exception handling
BOOST_AUTO_TEST_CASE(exceptions)
    {
    // file not found
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/not.a.file.pdb");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // missing box CRYST1
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.nobox");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // badly formed CRYST1
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.badbox");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // badly formed ATOM
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.badatom");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // atoms out of order
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.ordering");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // no numbers, should ignore
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.nonumber");    
        BOOST_CHECK_NO_THROW(traj.analyze());
        }
    // missing a number, should be fine
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.onenumber");    
        BOOST_CHECK_NO_THROW(traj.analyze());
        }
    // missing a number, but miscounting fails
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.badonenumber");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    // missing model number
        {
        PDBTrajectory traj(2.5);
        traj.addFile("test/unit/pdb/frame.pdb.modelnum");    
        BOOST_CHECK_THROW(traj.analyze(), std::exception);
        }
    }
    
BOOST_AUTO_TEST_SUITE_END()
