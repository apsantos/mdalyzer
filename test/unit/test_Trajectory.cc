/*!
 * \file test_Trajectory.cc
 * \author Michael P. Howard
 * \date 3 January 2015
 * \brief Boost unit tests for generic Trajectory
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"

#include "Frame.h"
#include "TriclinicBox.h"
#include "VectorMath.h"
#include "Trajectory.h"
#include "HOOMDXMLTrajectory.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

BOOST_AUTO_TEST_SUITE(Trajectories)
//! test for frame ordering
BOOST_AUTO_TEST_CASE(frame_sort)
    {
    HOOMDXMLTrajectory traj;
    traj.addFile("test/unit/hoomd/frame.xml.1");
    traj.addFile("test/unit/hoomd/frame.xml.2");
    traj.addFile("test/unit/hoomd/frame.xml.3");
    traj.analyze();
    
    std::vector< boost::shared_ptr<Frame> > frames = traj.getFrames();
    BOOST_REQUIRE_EQUAL(frames.size(), 3);
    BOOST_CHECK_CLOSE(frames[0]->getTime(), 0.0, test_very_close);
    BOOST_CHECK_CLOSE(frames[1]->getTime(), 2.5, test_very_close);
    BOOST_CHECK_CLOSE(frames[2]->getTime(), 5.75, test_very_close);
    }
//! test for frame extraction from first frame
BOOST_AUTO_TEST_CASE(frame_extract)
    {
    HOOMDXMLTrajectory traj;
    traj.addFile("test/unit/hoomd/frame.xml.1");
    traj.addFile("test/unit/hoomd/frame.xml.2");
    traj.addFile("test/unit/hoomd/frame.xml.3");
    traj.analyze();
    
    std::vector< boost::shared_ptr<Frame> > frames = traj.getFrames();
    BOOST_REQUIRE_EQUAL(frames.size(), 3);

    // frame.xml.3 should have no type, diameter, mass    
    BOOST_CHECK(!frames[2]->hasNames());
    BOOST_CHECK(!frames[2]->hasTypes());
    BOOST_CHECK(!frames[2]->hasDiameters());
    BOOST_CHECK(!frames[2]->hasMasses());
    
    // check trajectory type extraction
        {
        BOOST_REQUIRE(traj.hasNames());
        BOOST_REQUIRE(traj.hasTypes());
        std::vector<std::string> names = traj.getNames();
        std::vector<unsigned int> types = traj.getTypes();
    
        BOOST_CHECK_EQUAL(names[0], "AA");
        BOOST_CHECK_EQUAL(names[1], "BBB");
    
        BOOST_CHECK_EQUAL(types[0], traj.getTypeByName("AA"));
        BOOST_CHECK_EQUAL(types[1], traj.getTypeByName("BBB"));
        }
    
    // check trajectory diameter extraction
        {
        BOOST_REQUIRE(traj.hasDiameters());
        std::vector<double> diams = traj.getDiameters();
        BOOST_CHECK_CLOSE(diams[0], 2.0, test_very_close);
        BOOST_CHECK_CLOSE(diams[1], 3.0, test_very_close);
        }
        
    // check trajectory mass extraction
        {
        BOOST_REQUIRE(traj.hasMasses());
        std::vector<double> mass = traj.getMasses();
        BOOST_CHECK_CLOSE(mass[0], 10.0, test_very_close);
        BOOST_CHECK_CLOSE(mass[1], 5.0, test_very_close);
        }
    }
//! test for public setters of trajectory data    
BOOST_AUTO_TEST_CASE(frame_override)
    {
    HOOMDXMLTrajectory traj;
    traj.addFile("test/unit/hoomd/frame.xml.3");
    
    // set box, names, diameters, and masses externally
    Vector3<double> box_len(1.0, 2.0, 3.0);
    TriclinicBox box(box_len);
    traj.setBox(box);
    
    std::vector<std::string> names(2);
    names[0] = "CCC"; names[1] = "Z";
    traj.setNames(names);
    
    std::vector<double> diams(2);
    diams[0] = 6.0; diams[1] = 3.0;
    traj.setDiameters(diams);
    
    std::vector<double> masses(2);
    masses[0] = 3.0; masses[1] = 4.0;
    traj.setMasses(masses);
    
    traj.analyze();
    
    // check result after analysis
    BOOST_REQUIRE(traj.hasNames());
    BOOST_REQUIRE(traj.hasTypes());
    BOOST_REQUIRE(traj.hasDiameters());
    BOOST_REQUIRE(traj.hasMasses());
    
    // extracted names and types
    std::vector<std::string> ex_names = traj.getNames();
    std::vector<unsigned int> ex_types = traj.getTypes();
    BOOST_CHECK_EQUAL(ex_names[0], names[0]);
    BOOST_CHECK_EQUAL(ex_names[1], names[1]);
    BOOST_CHECK_EQUAL(ex_types[0], traj.getTypeByName(names[0]));
    BOOST_CHECK_EQUAL(ex_types[1], traj.getTypeByName(names[1]));
    
    // extracted diameters
    std::vector<double> ex_diams = traj.getDiameters();
    BOOST_CHECK_CLOSE(ex_diams[0], diams[0], test_very_close);
    BOOST_CHECK_CLOSE(ex_diams[1], diams[1], test_very_close);
    
    // extracted masses
    std::vector<double> ex_mass = traj.getMasses();
    BOOST_CHECK_CLOSE(ex_mass[0], masses[0], test_very_close);
    BOOST_CHECK_CLOSE(ex_mass[1], masses[1], test_very_close);
    }
BOOST_AUTO_TEST_SUITE_END()
