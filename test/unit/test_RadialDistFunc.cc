/*!
 * \file test_RadialDistFunc.cc
 * \author Elia Altabet
 * \date 9 January 2015
 * \brief Boost unit tests for Radial Distribution Function
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"

#include "Frame.h"
#include "TriclinicBox.h"
#include "VectorMath.h"
#include "Trajectory.h"
#include "XYZTrajectory.h"
#include "HOOMDXMLTrajectory.h"
#include "RadialDistributionFunction.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_SUITE(RDF)

BOOST_AUTO_TEST_CASE(RDF_test_output) {
   //! Load XYZ trajectory of Lennard-Jones particles
   boost::shared_ptr<XYZTrajectory> traj( new XYZTrajectory );
   traj->addFile("test/unit/rdf/lj_1000.xyz");
   Vector3<double> box_vec(13.78233718872, 13.78233718872, 13.78233718872);
   TriclinicBox box(box_vec);
   traj->setBox(box);

   //! Add RDF analyzer
   boost::shared_ptr<RadialDistributionFunction> RadFUNK( new RadialDistributionFunction( traj, "RadFUNK", 0.03, 3.0, 1) );
   traj->addAnalyzer( RadFUNK, "RadFUNK");
   traj->analyze();
    
   //! read files
   std::ifstream LAMMPS_g2r("test/unit/rdf/G_R.txt");
   std::ifstream MDALYZER_g2r("RadFUNK");
   //! check that the file exists
   BOOST_REQUIRE(MDALYZER_g2r.good());
   std::string line;
   std::getline(MDALYZER_g2r, line); std::getline(MDALYZER_g2r, line); // read header (2 lines)
   //! Check that the rdf values match LAMMPS output
   double Rad1 = 0.0, Rad2 = 0.0, g2r1 = 0.0, g2r2 = 0.0;
   double test_close_enough = 0.01;
   for (int i = 0; i < 100; ++i) {
      MDALYZER_g2r >> Rad1 >> g2r1;
      LAMMPS_g2r >> Rad2 >> g2r2;
//      printf("Rad: %f %f\n", Rad1, Rad2);
//      printf("G2R: %f %f\n", g2r1, g2r2);
      BOOST_CHECK_CLOSE(Rad1, Rad2, test_very_close); // makes sure testing apples vs. apples
      BOOST_CHECK_CLOSE(g2r1, g2r2, test_close_enough);
   }
    //! remove files
    remove("RadFUNK");
}

//! test exception handling
BOOST_AUTO_TEST_CASE(exceptions)    {

    // Needs a box
        {
        	//! Load hoomd file with no box in the file
		boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
   		traj->addFile("test/unit/vacf/frame_no_box.0000000000.xml");
   		traj->addFile("test/unit/vacf/frame_no_box.0000000100.xml");
		
                //! Add RDF analyzer
		boost::shared_ptr<RadialDistributionFunction> RadFUNK( new RadialDistributionFunction( traj, "RadFUNK", 1.0, 1.0, 1) );
		traj->addAnalyzer( RadFUNK, "RadFUNK");
		
        BOOST_CHECK_THROW(traj->analyze(), std::exception);
        } 
    // Needs positions
        {
		//! Load hoomd file with no positons in the file
		boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
   		traj->addFile("test/unit/vacf/frame_no_pos.0000000000.xml");
		traj->addFile("test/unit/vacf/frame_no_pos.0000000100.xml");
		
	        //! Add RDF analyzer
		boost::shared_ptr<RadialDistributionFunction> RadFUNK( new RadialDistributionFunction( traj, "RadFUNK", 1.0, 1.0, 1) );
		traj->addAnalyzer( RadFUNK, "RadFUNK");
		
        BOOST_CHECK_THROW(traj->analyze(), std::exception);
        } 
    // Needs maxR < nearest distance between edges
        {
		//! Load hoomd file 
		boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(0.5) );
   		traj->addFile("test/unit/vacf/frame.0000000000.xml");
		traj->addFile("test/unit/vacf/frame.0000000100.xml");
		
	        //! Add VACF analyzer                                                                                  // realy big maxR
		boost::shared_ptr<RadialDistributionFunction> RadFUNK( new RadialDistributionFunction( traj, "RadFUNK", 1.0, 100000000000.0, 1) );
		traj->addAnalyzer( RadFUNK, "RadFUNK");
		
        BOOST_CHECK_THROW(traj->analyze(), std::exception);
        } 
    }
BOOST_AUTO_TEST_SUITE_END()
