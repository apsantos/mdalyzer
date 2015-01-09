/*!
 * \file test_DCDTrajectory.cc
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
#include "GROTrajectory.h"
#include "PDBTrajectory.h"
#include "XYZTrajectory.h"
#include "HOOMDXMLTrajectory.h"

#include <boost/shared_ptr.hpp>

BOOST_AUTO_TEST_SUITE(DCD)
//! test all information is read from gro input
BOOST_AUTO_TEST_CASE(read_gro)
    {
	boost::shared_ptr<GROTrajectory> i_traj( new GROTrajectory(4) );
    i_traj->addFile("test/unit/dcd/frame.gro.1");
	DCDTrajectory traj(i_traj, "test/unit/dcd/frame.dcd.2", 2.5, 1);
    traj.analyze();
    
    TrajectoryCheck config;
	config.position = true;
    
    test_multiframe<DCDTrajectory>(traj, config);
    }
//! test all information is read from pdb input
BOOST_AUTO_TEST_CASE(read_pdb)
    {
	boost::shared_ptr<PDBTrajectory> i_traj( new PDBTrajectory(1.0) );
    i_traj->addFile("test/unit/dcd/frame.pdb.1");
	DCDTrajectory traj(i_traj, "test/unit/dcd/frame.dcd.2", 2.5, 1);
    traj.analyze();
    
    TrajectoryCheck config;
	config.position = true;
    
    test_multiframe<DCDTrajectory>(traj, config);
    }
//! test all information is read from xyz input
BOOST_AUTO_TEST_CASE(read_xyz)
    {
	boost::shared_ptr<XYZTrajectory> i_traj( new XYZTrajectory() );
    i_traj->addFile("test/unit/dcd/frame.xyz.1");
	DCDTrajectory traj(i_traj, "test/unit/dcd/frame.dcd.2", 2.5, 1);
    traj.analyze();
    
    TrajectoryCheck config;
	config.position = true;
    
    test_multiframe<DCDTrajectory>(traj, config);
    }
//! test all information is read from xml input
BOOST_AUTO_TEST_CASE(read_xml)
    {
	boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
    i_traj->addFile("test/unit/dcd/frame.xml.1");
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
    i_traj->addFile("test/unit/dcd/frame.xml.1");
	DCDTrajectory traj(i_traj, "test/unit/dcd/frame.dcd.2", 0, 0);
    traj.analyze();
    
    std::vector< boost::shared_ptr<Frame> > frames = traj.getFrames();
    BOOST_CHECK_CLOSE(frames[1]->getTime(), 1.0, test_very_close);
    }
BOOST_AUTO_TEST_SUITE_END()
