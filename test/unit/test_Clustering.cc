/*!
 * \file test_Clustering.cc
 * \author Sang Beom Kim
 * \date 9 January 2015
 * \brief Boost unit tests for Clustering
 */
#include <boost/test/unit_test.hpp>
#include "test_config.h"

#include "Frame.h"
#include "Trajectory.h"
#include "XTCTrajectory.h"
#include "Clustering.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>

BOOST_AUTO_TEST_SUITE(Cluster)
/*! test that the Clustering analyzer outputs the correct results, using a sample XTC trajectory
 */
BOOST_AUTO_TEST_CASE(Clustering_output_test)
    {
    //! Load XTC trajectory
    boost::shared_ptr<XTCTrajectory> traj( new XTCTrajectory );
    traj->addFile("test/unit/xtc/frame.xtc.singlefile");
    
    std::string clustering_out = "clustering.out";
    
    //! Add MSD analyzer
	boost::shared_ptr<Clustering> clustering( new Clustering( traj, clustering_out, 5.8) );
    traj->addAnalyzer( clustering, "clustering");
    traj->analyze();

    //! Values to check
    double frame_number, num_clus, size_clus;
    
    std::string line;
    
    //! read output files
    std::ifstream clus_file(clustering_out.c_str());
    //! check that the file exists
    BOOST_REQUIRE(clus_file.good());
    
    //! skip the comment lines
    for ( int i=0; i<7; ++i) std::getline(clus_file, line);
    
    //! check the output values for the first frame
    clus_file >> frame_number >> num_clus >> size_clus;
    BOOST_CHECK_CLOSE(frame_number, 0.0, test_very_close);
    BOOST_CHECK_CLOSE(num_clus, 1.0, test_very_close);
    BOOST_CHECK_CLOSE(size_clus, 2.0, test_very_close);

    //! check the output values for the second frame
    clus_file >> frame_number >> num_clus >> size_clus;
    BOOST_CHECK_CLOSE(frame_number, 1.0, test_very_close);
    BOOST_CHECK_CLOSE(num_clus, 2.0, test_very_close);
    BOOST_CHECK_CLOSE(size_clus, 1.0, test_very_close);

    clus_file.close();
    //! remove MSD output files
    remove(clustering_out.c_str());

    }

//! Check for exceptions
BOOST_AUTO_TEST_CASE(Cluster_exceptions)
    {
    // negative distance
        {
        boost::shared_ptr<XTCTrajectory> traj( new XTCTrajectory );
        traj->addFile("test/unit/xtc/frame.xtc.singlefile");
        std::string clustering_out = "clustering.out";
        //! Negative distance error
        BOOST_CHECK_THROW(boost::shared_ptr<Clustering> clustering(
            new Clustering( traj, clustering_out, -5.8)), std::exception );
        }

    }

BOOST_AUTO_TEST_SUITE_END()
