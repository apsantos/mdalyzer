#include <string>
#include <vector>
#include <boost/python.hpp>

#include "Trajectory.h"

#include "Frame.h"
#include "HOOMDXMLFrame.h"

#include "VectorMath.h"

BOOST_PYTHON_MODULE(mdalyzer)
    {
    using namespace boost::python;
    
    // expose the vector3 struct so that we can make these objects
    class_< Vector3<double> >("Vector3d", init<double, double, double>())
    .def_readwrite("x",&Vector3<double>::x)
    .def_readwrite("y",&Vector3<double>::y)
    .def_readwrite("z",&Vector3<double>::z);
    
    // trajectories
    export_Trajectory();
    
    // frames
    export_Frame();
    export_HOOMDXMLFrame();
    
    // computes
    }
