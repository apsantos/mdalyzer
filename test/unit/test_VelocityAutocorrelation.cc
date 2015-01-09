/*!
 * \file test_VelocityAutocorrelation.cc
 * \author Joseph R. Vella
 * \date 8 January 2015
 * \brief Boost unit tests for Velocity Autocorrelation Function Analyzer
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"

#include "Frame.h"
#include "TriclinicBox.h"
#include "VectorMath.h"
#include "Trajectory.h"
#include "DCDTrajectory.h"
#include "HOOMDXMLTrajectory.h"
#include "VelocityAutocorrelation.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_SUITE(VACF)
/*! test that the DCD reader defaults to the output frequency and timestep to what is 
 * in the DCD file, if the user specifies 0
 */
BOOST_AUTO_TEST_CASE(VACF_test_output_2_types)
    {
    //! Load DCD trajectory with HOOMDXML initial
	boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
    i_traj->addFile("test/unit/hoomd/frame.xml.1");
	boost::shared_ptr<DCDTrajectory> traj( new DCDTrajectory(i_traj, "test/unit/dcd/frame.dcd.2", 0, 0) );
    //! Add VACF analyzer
	boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj, "vacf", 10) );
    traj->addAnalyzer( vacf, "vacf");
    traj->analyze();

    Vector3< double > msd_in;
    double msd_tot_in = 0.0;
    double time = 0.0;
    std::string line;
    
    //! read output files
    std::ifstream AA_file("vacf_AA.dat");
    //! check that the file exists
    BOOST_REQUIRE(AA_file.good());
    std::getline(AA_file, line); // read header
    //! Check that the vacf values match expected values at the first timestep
    AA_file >> time >> vacf_tot_in >> vacf_in.x >> vacf_in.y >> vacf_in.z ;
    BOOST_CHECK_CLOSE(time, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.x, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.y, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_in.z, 0.0, test_very_close);
    //! Check that the vacf values are correct at a non-zero timestep
    AA_file >> time >> vacf_tot_in >> vacf_in.x >> vacf_in.y >> vacf_in.z ;
    BOOST_CHECK_CLOSE(time, 1.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 2.81, test_close);
    BOOST_CHECK_CLOSE(msd_in.x, 1.0, test_close);
    BOOST_CHECK_CLOSE(msd_in.y, 1.0, test_close);
    BOOST_CHECK_CLOSE(msd_in.z, .81, test_close);
    AA_file.close();
    //! remove VACF output files
    remove("vacf_AA.dat");

    //! read output files
    std::ifstream BBB_file("vacf_BBB.dat");
    //! check that the file exists
    BOOST_REQUIRE(BBB_file.good());
    std::getline(BBB_file, line); // read header
    //! Check that the vacf values match expected values at the first timestep
    BBB_file >> time >> vacf_tot_in >> vacf_in.x >> vacf_in.y >> vacf_in.z ;
    BOOST_CHECK_CLOSE(time, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(vacf_tot_in, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(vacf_in.x, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(vacf_in.y, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(vacf_in.z, 0.0, test_very_close);
    //! Check that the vacf values are correct at a non-zero timestep
    BBB_file >> time >> vacf_tot_in >> vacf_in.x >> vacf_in.y >> vacf_in.z ;
    BOOST_CHECK_CLOSE(time, 1.0, test_very_close);
    BOOST_CHECK_CLOSE(msd_tot_in, 2.81, test_close);
    BOOST_CHECK_CLOSE(msd_in.x, 0.81, test_close);
    BOOST_CHECK_CLOSE(msd_in.y, 1.0, test_close);
    BOOST_CHECK_CLOSE(msd_in.z, 1.0, test_close);
    BBB_file.close();
    //! remove VACF output files
    remove("vacf_BBB.dat");

    }
	//! test exception handling
BOOST_AUTO_TEST_CASE(exceptions)
    {

    // Cannot remove types that don't exist NEED HELP HERE
        {    
        BOOST_CHECK_THROW(traj_1.analyze(), std::exception);
        }
    // Needs time data
        {
		//! Load  HOOMDXML no time data
		boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
		traj_2->addFile("test/unit/hoomd/frame_missing_time.xml.1");

	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj_2, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		traj->analyze();
		
        BOOST_CHECK_THROW(traj_2.analyze(), std::exception);
        }
    // Needs velocity data
        {
		//! Load xyz file (no velocity in this file)
		boost::shared_ptr<XYZTrajectory> i_traj( new XYZTrajectory(1.0) );
		traj_3->addFile("test/unit/hoomd/frame.xyz");
		
	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj_3, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		traj->analyze();
		
        BOOST_CHECK_NO_THROW(traj_3.analyze());
        } 
    // Needs velocities for all frames
        {
		//! Load DCD trajectory with HOOMDXML initial missing velocities 
		boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
		traj_4->addFile("test/unit/hoomd/frame_missing_vel.xml.1");
		boost::shared_ptr<DCDTrajectory> traj( new DCDTrajectory(traj4, "test/unit/dcd/frame.dcd.2", 0, 0) );		

	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj_4, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		traj->analyze();
		
        BOOST_CHECK_THROW(traj_4.analyze(), std::exception);
        } 
    // Needs particle types
        {
		//! Load DCD trajectory with HOOMDXML initial missing types
		boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
		traj_5->addFile("test/unit/hoomd/frame_missing_types.xml.1");
		boost::shared_ptr<DCDTrajectory> traj( new DCDTrajectory(traj_5, "test/unit/dcd/frame.dcd.2", 0, 0) );

	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj_4, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		traj->analyze();
		
        BOOST_CHECK_THROW(traj_5.analyze(), std::exception);
        } 
    }
BOOST_AUTO_TEST_SUITE_END()
