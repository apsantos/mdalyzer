#include "Compute.h"

#include <boost/python.hpp>

Compute::Compute(boost::shared_ptr<Trajectory> traj)
    : m_traj(traj)
    {
    }

/*!
 * Boost needs us to define a wrapper around compute
 */
struct ComputeWrap : public Compute, boost::python::wrapper<Compute>
    {
    ComputeWrap(boost::shared_ptr<Trajectory> traj) : Compute(traj) {}
    
    void evaluate()
        {
        this->get_override("evaluate")();
        }
    };

//! Boost Python export for Compute
void export_Compute()
    {
    using namespace boost::python;
    
    class_<ComputeWrap, boost::shared_ptr<ComputeWrap>, boost::noncopyable>
        ("Compute", init< boost::shared_ptr<Trajectory> >())
    .def("evaluate", pure_virtual(&Compute::evaluate));
    }
        