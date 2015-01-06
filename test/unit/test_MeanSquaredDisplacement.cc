/*!
 * \file test_MeanSquaredDisplacement.cc
 * \author Andrew P. Santos
 * \date 5 January 2015
 * \brief Boost unit tests for DCD reader
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"

#include "Frame.h"
#include "TriclinicBox.h"
#include "VectorMath.h"
#include "Trajectory.h"
#include "DCDTrajectory.h"
#include "HOOMDXMLTrajectory.h"
#include "MeanSquaredDisplacement.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

BOOST_AUTO_TEST_SUITE(MSD)
/*! test that the DCD reader defaults to the output frequency and timestep to what is 
 * in the DCD file, if the user specifies 0
 */
BOOST_AUTO_TEST_CASE(MSD_)
    {
	boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
    i_traj->addFile("test/unit/hoomd/frame.xml.1");
	boost::shared_ptr<DCDTrajectory> traj( new DCDTrajectory(i_traj, "test/unit/dcd/frame.dcd.2", 0, 0) );
	boost::shared_ptr<MeanSquaredDisplacement> msd( new MeanSquaredDisplacement( traj, "msd", 10) );
    traj->addAnalyzer( msd, "msd");
    traj->analyze();
    
    }
BOOST_AUTO_TEST_SUITE_END()
