/*!
 * \file test_Trajectory.h
 * \author Michael P. Howard
 * \date 3 January 2015
 * \brief Declaration of templated generic tests to be called for all Trajectory implementations
 */
#ifndef TEST_UNIT_TEST_TRAJECTORY_H_
#define TEST_UNIT_TEST_TRAJECTORY_H_

#include "Frame.h"
#include "TriclinicBox.h"
#include "VectorMath.h"
#include "Trajectory.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

//! flags for which data a specific Trajectory should contain
struct TrajectoryCheck
    {
    TrajectoryCheck() : box(false), position(false), velocity(false), type(false), diameter(false), mass(false) {};
    bool box;
    bool position;
    bool velocity;
    bool type;
    bool diameter;
    bool mass;
    };

// templated functions have to be declared in the header if they are going to be linked into multiple files
//! test single frame reading
/*!
 * \param traj A Trajectory loaded into memory
 * \param config which data to validate
 *
 * Each part of the data is validated for a two particle system with fixed values. Each Trajectory
 * implementation should supply a sample data file conforming to the following:
 *
 * Box: Orthorhombic, (11,5,8)
 * Time: 0.0
 * Positions: ((1,2,3),(-3,-2,-1))
 * Velocities: ((3,-2,1),(-1,2,-3))
 * Names: (AA, BBB)
 * Diameters: (2,3)
 * Masses: (10,5)
 *
 * as appropriate to the file format.
 */
template<class T>
void test_read(const T& traj, const TrajectoryCheck& config)
    {
    boost::shared_ptr<Frame> frame = traj.getFrame(0);
    
    BOOST_REQUIRE_EQUAL(frame->getN(), 2);
    BOOST_CHECK_CLOSE(frame->getTime(), 0.0, test_very_close);
    
    if (config.box)
        {
        BOOST_REQUIRE(frame->hasBox());
        
        TriclinicBox box = frame->getBox();
        Vector3<double> length = box.getLength();
        BOOST_CHECK_CLOSE(length.x, 11.0, test_very_close);
        BOOST_CHECK_CLOSE(length.y, 5.0, test_very_close);
        BOOST_CHECK_CLOSE(length.z, 8.0, test_very_close);
        }
    
    if (config.position)
        {
        BOOST_REQUIRE(frame->hasPositions());
        
        std::vector< Vector3<double> > positions = frame->getPositions();
        BOOST_CHECK_CLOSE(positions[0].x, 1.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[0].y, 2.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[0].z, 3.0, test_very_close);
        
        BOOST_CHECK_CLOSE(positions[1].x, -3.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[1].y, -2.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[1].z, -1.0, test_very_close);
        }
    
    if (config.velocity)
        {
        BOOST_REQUIRE(frame->hasVelocities());
        
        std::vector< Vector3<double> > velocities = frame->getVelocities();
        BOOST_CHECK_CLOSE(velocities[0].x, 3.0, test_very_close);
        BOOST_CHECK_CLOSE(velocities[0].y, -2.0, test_very_close);
        BOOST_CHECK_CLOSE(velocities[0].z, 1.0, test_very_close);
        
        BOOST_CHECK_CLOSE(velocities[1].x, -1.0, test_very_close);
        BOOST_CHECK_CLOSE(velocities[1].y, 2.0, test_very_close);
        BOOST_CHECK_CLOSE(velocities[1].z, -3.0, test_very_close);
        }
        
    if (config.type)
        {
        BOOST_REQUIRE(frame->hasNames());
        BOOST_REQUIRE(frame->hasTypes());
        
        std::vector<std::string> names = frame->getNames();
        std::vector<unsigned int> types = frame->getTypes();
        
        BOOST_CHECK_EQUAL(names[0], "AA");
        BOOST_CHECK_EQUAL(names[1], "BBB");
        
        BOOST_CHECK_EQUAL(types[0], traj.getTypeByName("AA"));
        BOOST_CHECK_EQUAL(types[1], traj.getTypeByName("BBB"));
        }
    
    if (config.diameter)
        {
        BOOST_REQUIRE(frame->hasDiameters());
        
        std::vector<double> diams = frame->getDiameters();
        BOOST_CHECK_CLOSE(diams[0], 2.0, test_very_close);
        BOOST_CHECK_CLOSE(diams[1], 3.0, test_very_close);
        }
    
    if (config.mass)
        {
        BOOST_REQUIRE(frame->hasMasses());
        
        std::vector<double> mass = frame->getMasses();
        BOOST_CHECK_CLOSE(mass[0], 10.0, test_very_close);
        BOOST_CHECK_CLOSE(mass[1], 5.0, test_very_close);
        }
    }

//! test multi frame reading
/*!
 * \param traj A Trajectory loaded into memory
 * \param config which data to validate
 *
 * In addition to the first file tested in test_read, a second frame must be loaded containing the following:
 *
 * Time: 2.5
 * Positions: ((2,1,3.9),(-3.9,-1,-2))
 * Velocities: ((2,3,-4),(8.5,-2.5,-1.5))
 * Names: (AA, BBB)
 * Diameters: (2,3)
 * Masses: (10,5)
 *
 * Name, diameter, and mass must be supplied explicitly in the Frame.
 */
template<class T>
void test_multiframe(const T& traj, const TrajectoryCheck& config)
    {
    std::vector< boost::shared_ptr<Frame> > frames = traj.getFrames();
    BOOST_REQUIRE_EQUAL(frames.size(), 2);
    
    BOOST_CHECK_CLOSE(frames[1]->getTime(), 2.5, test_very_close);
        
    if (config.position)
        {
        BOOST_REQUIRE(frames[1]->hasPositions());
        
        std::vector< Vector3<double> > positions = frames[1]->getPositions();
        BOOST_CHECK_CLOSE(positions[0].x, 2.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[0].y, 1.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[0].z, 3.9, test_very_close);
        
        BOOST_CHECK_CLOSE(positions[1].x, -3.9, test_very_close);
        BOOST_CHECK_CLOSE(positions[1].y, -1.0, test_very_close);
        BOOST_CHECK_CLOSE(positions[1].z, -2.0, test_very_close);
        }
    
    if (config.velocity)
        {
        BOOST_REQUIRE(frames[1]->hasVelocities());
        
        std::vector< Vector3<double> > velocities = frames[1]->getVelocities();
        BOOST_CHECK_CLOSE(velocities[0].x, 2.0, test_very_close);
        BOOST_CHECK_CLOSE(velocities[0].y, 3.0, test_very_close);
        BOOST_CHECK_CLOSE(velocities[0].z, -4.0, test_very_close);
        
        BOOST_CHECK_CLOSE(velocities[1].x, 8.5, test_very_close);
        BOOST_CHECK_CLOSE(velocities[1].y, -2.5, test_very_close);
        BOOST_CHECK_CLOSE(velocities[1].z, -1.5, test_very_close);
        }
        
    if (config.type)
        {
        BOOST_REQUIRE(frames[1]->hasNames());
        BOOST_REQUIRE(frames[1]->hasTypes());
        
        std::vector<std::string> names = frames[1]->getNames();
        std::vector<unsigned int> types = frames[1]->getTypes();
        
        BOOST_CHECK_EQUAL(names[0], "AA");
        BOOST_CHECK_EQUAL(names[1], "BBB");
        
        BOOST_CHECK_EQUAL(types[0], traj.getTypeByName("AA"));
        BOOST_CHECK_EQUAL(types[1], traj.getTypeByName("BBB"));
        }
    
    if (config.diameter)
        {
        BOOST_REQUIRE(frames[1]->hasDiameters());
        
        std::vector<double> diams = frames[1]->getDiameters();
        BOOST_CHECK_CLOSE(diams[0], 2.0, test_very_close);
        BOOST_CHECK_CLOSE(diams[1], 3.0, test_very_close);
        }
    
    if (config.mass)
        {
        BOOST_REQUIRE(frames[1]->hasMasses());
        
        std::vector<double> mass = frames[1]->getMasses();
        BOOST_CHECK_CLOSE(mass[0], 10.0, test_very_close);
        BOOST_CHECK_CLOSE(mass[1], 5.0, test_very_close);
        }
    }

#endif //  TEST_UNIT_TEST_TRAJECTORY_H_

