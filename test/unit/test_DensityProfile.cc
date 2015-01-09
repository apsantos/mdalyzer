/*!
 * \file test_DensityProfile.cc
 * \author Michael P. Howard
 * \date 3 January 2015
 * \brief Boost unit tests for density profile calculator
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"
#include "test_Analyzer.h"

#include "VectorMath.h"
#include "HOOMDXMLTrajectory.h"
#include "DensityProfile.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <cstdio>
#include <fstream>

BOOST_AUTO_TEST_SUITE(density_profile)

//! test basic average density parsing for a two file, two particle system
BOOST_AUTO_TEST_CASE(basic)
    {
    FileFixture f;
    
    boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(1.0) );
    traj->addFile("test/unit/density/frame.xml.1");
    traj->addFile("test/unit/density/frame.xml.2");
    
    Vector3<unsigned int> bins(10,5,5);
    std::string outf(f.tmp + "/basic");
    f.addFile(outf + ".x.dat");
    f.addFile(outf + ".y.dat");
    f.addFile(outf + ".z.dat");
    boost::shared_ptr<DensityProfile> analyzer( new DensityProfile(traj, outf, bins) );
    analyzer->useMassWeighting(true);
    
    traj->addAnalyzer(analyzer, "density");
    
    traj->analyze();
    
    // x profile
        {
        double coord[10] = {0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5};
        double densities[10] = {0., 0.05, 0., 0., 0.05, 0., 0.025, 0.025, 0., 0.};
        checkTwoColumn(f.tmp + "/basic.x.dat", coord, densities, 1, bins.x, test_very_close);
        }
    // y profile
        {
        double coord[5] = {0.5,1.5,2.5,3.5,4.5};
        double densities[5] = {0.0125, 0., 0.025, 0.0125, 0.025};
        checkTwoColumn(f.tmp + "/basic.y.dat", coord, densities, 1, bins.y, test_very_close);
        }
    // z profile
        {
        double coord[5] = {2.,6.,10.,14.,18.};
        double densities[5] = {0.025, 0.0125, 0., 0.025, 0.0125};
        checkTwoColumn(f.tmp + "/basic.z.dat", coord, densities, 1, bins.z, test_very_close);
        }
    }

//! check that binning works when bins does not evenly divide box using number density
BOOST_AUTO_TEST_CASE(binning)
    {
    FileFixture f;
    
    boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(1.0) );
    traj->addFile("test/unit/density/frame.xml.1");
    traj->addFile("test/unit/density/frame.xml.2");
    
    
    Vector3<unsigned int> bins(4,2,6);
    std::string outf(f.tmp + "/binning");
    f.addFile(outf + ".x.dat");
    f.addFile(outf + ".y.dat");
    f.addFile(outf + ".z.dat");
    boost::shared_ptr<DensityProfile> analyzer( new DensityProfile(traj, outf, bins) );
    analyzer->useMassWeighting(false);
    traj->addAnalyzer(analyzer, "density");
    
    traj->analyze();
    
    // x profile
        {
        double densities[4] = {0.002,0.002,0.004,0.};
        checkTwoColumn(f.tmp + "/binning.x.dat", NULL, densities, 1, bins.x, test_very_close);
        }
    // y profile
        {
        double densities[2] = {0.002,0.002};
        checkTwoColumn(f.tmp + "/binning.y.dat", NULL, densities, 1, bins.y, test_very_close);
        }
    // z profile
        {
        double densities[6] = {0.003,0.,0.003,0.003,0.,0.003};
        checkTwoColumn(f.tmp + "/binning.z.dat", NULL, densities, 1, bins.z, test_very_close);
        }
    }

//! check that a single type can be calculated
BOOST_AUTO_TEST_CASE(types)
    {
    FileFixture f;
    
    boost::shared_ptr<HOOMDXMLTrajectory> traj( new HOOMDXMLTrajectory(1.0) );
    traj->addFile("test/unit/density/frame.xml.1");
    traj->addFile("test/unit/density/frame.xml.2");
    
    
    Vector3<unsigned int> bins(10,5,5);
    std::string outf(f.tmp + "/onetype");
    f.addFile(outf + ".x.dat");
    f.addFile(outf + ".y.dat");
    f.addFile(outf + ".z.dat");
    
    boost::shared_ptr<DensityProfile> analyzer( new DensityProfile(traj, outf, bins) );
    analyzer->useMassWeighting(true);
    analyzer->addType("AA");
    analyzer->addType("BBB");
    analyzer->deleteType("BBB");
    
    traj->addAnalyzer(analyzer, "density");
    
    traj->analyze();
    
    // x profile
        {
        double densities[10] = {0., 0.05, 0., 0., 0.05, 0., 0., 0., 0., 0.};
        checkTwoColumn(f.tmp + "/onetype.x.dat", NULL, densities, 1, bins.x, test_very_close);
        }
    // y profile
        {
        double densities[5] = {0., 0., 0.025, 0., 0.025};
        checkTwoColumn(f.tmp + "/onetype.y.dat", NULL, densities, 1, bins.y, test_very_close);
        }
    // z profile
        {
        double densities[5] = {0.025, 0., 0., 0.025, 0.};
        checkTwoColumn(f.tmp + "/onetype.z.dat", NULL, densities, 1, bins.z, test_very_close);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
