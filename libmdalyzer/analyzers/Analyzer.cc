/*!
 * \file Analyzer.cc
 * \author Michael P. Howard
 * \brief Parent of the derived analyzers
 */
#include "Analyzer.h"

#include <boost/python.hpp>

Analyzer::Analyzer(boost::shared_ptr<Trajectory> traj)
    : m_traj(traj)
    {
    }

/*!
 * Wrapper around each compute.
 * Requred for Boost.
 */
struct AnalyzerWrap : public Analyzer, boost::python::wrapper<Analyzer>
    {
    AnalyzerWrap(boost::shared_ptr<Trajectory> traj) : Analyzer(traj) {}
    
    void evaluate()
        {
        this->get_override("evaluate")();   //!< Overide evaluate function for each analyzer
        }
    };

//! Boost Python export for Analyzer
void export_Analyzer()
    {
    using namespace boost::python;
    
    class_<AnalyzerWrap, boost::shared_ptr<AnalyzerWrap>, boost::noncopyable>
        ("Analyzer", init< boost::shared_ptr<Trajectory> >())
    .def("evaluate", pure_virtual(&Analyzer::evaluate));
    }
        
