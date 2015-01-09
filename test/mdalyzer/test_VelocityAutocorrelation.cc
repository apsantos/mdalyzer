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
BOOST_AUTO_TEST_CASE(VACF_test_output)
    {
    //! Load HOOMDXML trajectory initial
	boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
    traj->addFile("test/unit/vacf/frame.0000000000.xml");
	traj->addFile("test/unit/vacf/frame.0000000100.xml");
    //! Add VACF analyzer
	boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj, "vacf", 1) );
    traj->addAnalyzer( vacf, "vacf");
    traj->analyze();

    Vector3< double > vacf_in;
    double vacf_tot_in = 0.0;
    double time = 0.0;
    std::string line;
    
    //! read output files
    std::ifstream A_file("vacf_A.dat");
    //! check that the file exists
    BOOST_REQUIRE(A_file.good());
    std::getline(A_file, line); // read header
    //! Check that the vacf values match expected values at the first timestep
    A_file >> time >> vacf_tot_in >> vacf_in.x >> vacf_in.y >> vacf_in.z ;
    BOOST_CHECK_CLOSE(time, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(vacf_tot_in, 5.4742009, test_very_close);
    BOOST_CHECK_CLOSE(vacf_in.x, 1.7332571, test_very_close);
    BOOST_CHECK_CLOSE(vacf_in.y, 2.1191002, test_very_close);
    BOOST_CHECK_CLOSE(vacf_in.z, 1.6218436, test_very_close);
    //! Check that the vacf values are correct at a non-zero timestep
    A_file >> time >> vacf_tot_in >> vacf_in.x >> vacf_in.y >> vacf_in.z ;
    BOOST_CHECK_CLOSE(time, 50.0, test_very_close);
    BOOST_CHECK_CLOSE(vacf_tot_in, 0.79958086, test_close);
    BOOST_CHECK_CLOSE(vacf_in.x, -0.82383554, test_close);
    BOOST_CHECK_CLOSE(vacf_in.y, 2.1111491, test_close);
    BOOST_CHECK_CLOSE(vacf_in.z, -0.48773272, test_close);
    A_file.close();
    //! remove VACF output files
    remove("vacf_A.dat");

    }
	//! test exception handling
BOOST_AUTO_TEST_CASE(exceptions)
    {

    // Cannot remove types that don't exist NEED HELP HERE
 //       {    
 //       BOOST_CHECK_THROW(traj_1.analyze(), std::exception);
 //       }
    // Needs time data
        {
		//! Load hoomd file with no velocity in the file
		boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
   		traj->addFile("test/unit/vacf/frame_no_time.0000000000.xml");
   		traj->addFile("test/unit/vacf/frame_no_time.0000000100.xml");
		
	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		
        BOOST_CHECK_THROW(traj->analyze(), std::exception);
        } 
    // Needs velocity data
        {
		//! Load hoomd file with no velocity in the file
		boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
   		traj->addFile("test/unit/vacf/frame_no_vel.0000000000.xml");
		traj->addFile("test/unit/vacf/frame_no_vel.0000000100.xml");
		
	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		
        BOOST_CHECK_THROW(traj->analyze(), std::exception);
        } 
    // Needs velocities for all frames
//        {
		//! Load DCD trajectory with HOOMDXML initial missing velocities 
//		boost::shared_ptr<HOOMDXMLTrajectory> i_traj( new HOOMDXMLTrajectory(1.0) );
//		traj_4->addFile("test/unit/hoomd/frame_missing_vel.xml.1");
//		boost::shared_ptr<DCDTrajectory> traj( new DCDTrajectory(traj4, "test/unit/dcd/frame.dcd.2", 0, 0) );		

	    //! Add VACF analyzer
//		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj_4, "vacf", 10) );
//		traj->addAnalyzer( vacf, "vacf");
//		traj->analyze();
		
//        BOOST_CHECK_THROW(traj_4.analyze(), std::exception);
//        } 
    // Needs particle types
        {
		//! Load hoomd file with no tpyes in the file
		boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
   		traj->addFile("test/unit/vacf/frame_no_type.0000000000.xml");
		traj->addFile("test/unit/vacf/frame_no_type.0000000100.xml");
		
	    //! Add VACF analyzer
		boost::shared_ptr<VelocityAutocorrelation> vacf( new VelocityAutocorrelation( traj, "vacf", 10) );
		traj->addAnalyzer( vacf, "vacf");
		
        BOOST_CHECK_THROW(traj->analyze(), std::exception);
        } 
    }
BOOST_AUTO_TEST_SUITE_END()
