/*!
 * \file test_TemperatureProfile.cc
 * \author Michael P. Howard
 * \date 3 January 2015
 * \brief Boost unit tests for temperature profile calculator
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Analyzer.h"

#include "VectorMath.h"
#include "HOOMDXMLTrajectory.h"
#include "TemperatureProfile.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <cstdio>
#include <fstream>

BOOST_AUTO_TEST_SUITE(temperature_profile)

//! test basic average density parsing for a two file, two particle system
BOOST_AUTO_TEST_CASE(basic)
    {
    FileFixture f;
    
    boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(1.0) );
    traj->addFile("test/unit/temperature/frame.xml.1");
    traj->addFile("test/unit/temperature/frame.xml.2");
    
    Vector3<unsigned int> bins(10,5,5);
    std::string outf(f.tmp + "/basic");
    f.addFile(outf + ".x.dat");
    f.addFile(outf + ".y.dat");
    f.addFile(outf + ".z.dat");
    boost::shared_ptr<TemperatureProfile> analyzer( new TemperatureProfile(traj, outf, bins) );
    
    traj->addAnalyzer(analyzer, "temperature");
    
    traj->analyze();
    
    // x profile
        {
        double coord[10] = {0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5};
        double temps[10] = {0., 1.5, 0., 0., 1.5, 0., 0., 0., 0., 0.};
        checkTwoColumn(f.tmp + "/basic.x.dat", coord, temps, 1, bins.x, test_very_close);
        }
    // y profile
        {
        double coord[5] = {0.5,1.5,2.5,3.5,4.5};
        double temps[5] = {0., 0., 1.5, 0., 1.5};
        checkTwoColumn(f.tmp + "/basic.y.dat", coord, temps, 1, bins.y, test_very_close);
        }
    // z profile
        {
        double coord[5] = {2.,6.,10.,14.,18.};
        double temps[5] = {1.5, 0., 0., 1.5, 0.};
        checkTwoColumn(f.tmp + "/basic.z.dat", coord, temps, 1, bins.z, test_very_close);
        }
    }

//! check that a single type can be calculated
BOOST_AUTO_TEST_CASE(types)
    {
    FileFixture f;
    
    boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(1.0) );
    traj->addFile("test/unit/temperature/frame.xml.1");
    traj->addFile("test/unit/temperature/frame.xml.2");
    
    
    Vector3<unsigned int> bins(10,5,5);
    std::string outf(f.tmp + "/onetype");
    f.addFile(outf + ".x.dat");
    f.addFile(outf + ".y.dat");
    f.addFile(outf + ".z.dat");
    
    boost::shared_ptr<TemperatureProfile> analyzer( new TemperatureProfile(traj, outf, bins) );
    analyzer->addType("AA");
    analyzer->deleteType("AA");
    analyzer->addType("AA");
    
    traj->addAnalyzer(analyzer, "temperature");
    
    traj->analyze();
    
    // x profile
        {
        double temps[10] = {0., 1.5, 0., 0., 1.5, 0., 0., 0., 0., 0.};
        checkTwoColumn(f.tmp + "/onetype.x.dat", NULL, temps, 1, bins.x, test_very_close);
        }
    // y profile
        {
        double temps[5] = {0., 0., 1.5, 0., 1.5};
        checkTwoColumn(f.tmp + "/onetype.y.dat", NULL, temps, 1, bins.y, test_very_close);
        }
    // z profile
        {
        double temps[5] = {1.5, 0., 0., 1.5, 0.};
        checkTwoColumn(f.tmp + "/onetype.z.dat", NULL, temps, 1, bins.z, test_very_close);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
