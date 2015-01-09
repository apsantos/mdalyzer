#include <string>
#include <vector>
#include <boost/python.hpp>

#include "Trajectory.h"
#include "HOOMDXMLTrajectory.h"
#include "DCDTrajectory.h"
#include "GROTrajectory.h"
#include "PDBTrajectory.h"
#include "XYZTrajectory.h"
#include "TRRTrajectory.h"
#include "XTCTrajectory.h"

#include "Frame.h"
#include "TriclinicBox.h"

#include "Analyzer.h"
#include "DensityProfile.h"
#include "TemperatureProfile.h"
#include "MeanSquaredDisplacement.h"
#include "VelocityAutocorrelation.h"
#include "Clustering.h"
#include "RadialDistributionFunction.h"

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
    export_DCDTrajectory();
    export_HOOMDXMLTrajectory();
    export_GROTrajectory();
    export_PDBTrajectory();
    export_XYZTrajectory();
    export_TRRTrajectory();
    export_XTCTrajectory();
    
    // data structures
    export_Frame();
    export_TriclinicBox();
    
    // analyzers
    export_Analyzer();
    export_DensityProfile();
    export_TemperatureProfile();
    export_MeanSquaredDisplacement();
    export_VelocityAutocorrelation();
    export_Clustering();
    export_RadialDistributionFunction();
    }

