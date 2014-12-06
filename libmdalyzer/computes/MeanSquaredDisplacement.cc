#include "MeanSquaredDisplacement.h"
#include "Trajectory.h"
#include "Frame.h"
#include "TriclinicBox.h"

#include <fstream>
#include <algorithm>

#include <boost/python.hpp>

MeanSquaredDisplacement::MeanSquaredDisplacement(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const unsigned int&  origins)
    : Compute(traj), m_file_name(file_name), m_origins(origins)
    {
    m_type_names.reserve(m_traj->getNumTypes());
    m_type_mass.();
    }


void MeanSquaredDisplacement::addType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
    if (name_it == m_type_names.end())
        {
        m_type_names.push_back(name);
        }
    }
void MeanSquaredDisplacement::deleteType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
    if (name_it == m_type_names.end())
        {
        throw std::runtime_error("MeanSquaredDisplacement cannot remove type that doesn't exit");
        }
    else
        {
        m_type_names.erase(name_it);
        }
    }

void MeanSquaredDisplacement::evaluate()
    {
    }

void export_MeanSquaredDisplacement()
    {
    using namespace boost::python;
    class_<MeanSquaredDisplacement, boost::shared_ptr<MeanSquaredDisplacement>, bases<Compute>, boost::noncopyable >
    ("MeanSquaredDisplacement", init< boost::shared_ptr<Trajectory>, const std::string&, unsigned int& >())
    .def("addType",&MeanSquaredDisplacement::addType)
    .def("deleteType",&MeanSquaredDisplacement::deleteType)
    }
    
