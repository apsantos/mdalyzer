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
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_SUITE(MSD)
/*! test that the DCD reader defaults to the output frequency and timestep to what is 
 * in the DCD file, if the user specifies 0
 */
BOOST_AUTO_TEST_CASE(MSD_test_output_2_types)
    {
    //! Load DCD trajectory with HOOMDXML initial
	boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
    i_traj->addFile("test/unit/hoomd/frame.xml.1");
	boost::shared_ptr<DCDTrajectory> traj( new DCDTrajectory(i_traj, "test/unit/dcd/frame.dcd.2", 0, 0) );
    //! Add MSD analyzer
	boost::shared_ptr<MeanSquaredDisplacement> msd( new MeanSquaredDisplacement( traj, "msd", 10) );
    traj->addAnalyzer( msd, "msd");
    traj->analyze();

    Vector3< double > msd_in;
    double msd_tot_in = 0.0;
    double time = 0.0;
    std::string line;
    
    //! read output files
    std::ifstream AA_file("msd_AA.dat");
    //! check that the file exists
    BOOST_REQUIRE(AA_file.good());
    std::getline(AA_file, line); // read header
    //! Check that the msd values are 0 at the first timestep
    AA_file >> time >> msd_tot_in >> msd_in.x >> msd_in.y >> msd_in.z ;
    BOOST_CHECK_CLOSE(time, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.x, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.y, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.z, 0.0, test_very_close);
    //! Check that the msd values are correct at a non-zero timestep
    AA_file >> time >> msd_tot_in >> msd_in.x >> msd_in.y >> msd_in.z ;
    BOOST_CHECK_CLOSE(time, 1.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 2.81, test_close);
    BOOST_CHECK_CLOSE(msd_in.x, 1.0, test_close);
    BOOST_CHECK_CLOSE(msd_in.y, 1.0, test_close);
    BOOST_CHECK_CLOSE(msd_in.z, .81, test_close);
    AA_file.close();
    //! remove MSD output files
    remove("msd_AA.dat");

    //! read output files
    std::ifstream BBB_file("msd_BBB.dat");
    //! check that the file exists
    BOOST_REQUIRE(BBB_file.good());
    std::getline(BBB_file, line); // read header
    //! Check that the msd values are 0 at the first timestep
    BBB_file >> time >> msd_tot_in >> msd_in.x >> msd_in.y >> msd_in.z ;
    BOOST_CHECK_CLOSE(time, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.x, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.y, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.z, 0.0, test_very_close);
    //! Check that the msd values are correct at a non-zero timestep
    BBB_file >> time >> msd_tot_in >> msd_in.x >> msd_in.y >> msd_in.z ;
    BOOST_CHECK_CLOSE(time, 1.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 2.81, test_close);
    BOOST_CHECK_CLOSE(msd_in.x, 0.81, test_close);
    BOOST_CHECK_CLOSE(msd_in.y, 1.0, test_close);
    BOOST_CHECK_CLOSE(msd_in.z, 1.0, test_close);
    BBB_file.close();
    //! remove MSD output files
    remove("msd_BBB.dat");

    }
BOOST_AUTO_TEST_SUITE_END()
