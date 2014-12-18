#include <string>
#include <vector>
#include <boost/python.hpp>

#include "Trajectory.h"
#include "HOOMDXMLTrajectory.h"

#include "Frame.h"

#include "Analyzer.h"
#include "DensityProfile.h"
// #include "MeanSquaredDisplacement.h"

#include "VectorMath.h"

BOOST_PYTHON_MODULE(libmdalyzer)
    {
    using namespace boost::python;
    
    // expose the vector3 struct so that we can make these objects
    class_< Vector3<unsigned int> >("Vector3uint", init<unsigned int, unsigned int, unsigned int>())
    .def_readwrite("x",&Vector3<unsigned int>::x)
    .def_readwrite("y",&Vector3<unsigned int>::y)
    .def_readwrite("z",&Vector3<unsigned int>::z);
    
    class_< Vector3<double> >("Vector3d", init<double, double, double>())
    .def_readwrite("x",&Vector3<double>::x)
    .def_readwrite("y",&Vector3<double>::y)
    .def_readwrite("z",&Vector3<double>::z);

    
    // trajectories
    export_Trajectory();
    export_HOOMDXMLTrajectory();
    
    // frames
    export_Frame();
//     export_HOOMDXMLFrame();
    
    // computes
    export_Analyzer();
    export_DensityProfile();
//     export_MeanSquaredDisplacement();
    }
