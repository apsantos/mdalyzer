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

    // read the frames and make sure there is a simulation box
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    if (!frames[0]->hasTime())
        {
        // error! there is not time box not found
        throw std::runtime_error("MeanSquaredDisplacement needs data on time");
        }
    unsigned int time = 10;
    Vector3< std::vector< std::vector<float> > > msd;//(m_type_names.size(), time) ;
    std::vector<float> zero(2,0.0);
    unsigned int reserve_size = std::max((int)m_traj->getNumTypes(),1); // if no types are specified, use all particles
    msd.x.reserve(reserve_size);
    msd.y.reserve(reserve_size);
    msd.z.reserve(reserve_size);
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
        {
        for (unsigned int cur_type = 0; cur_type < m_type_names.size(); cur_type++)
            {
            msd.x.push_back(zero);
            msd.y.push_back(zero);
            msd.z.push_back(zero);
            }
        }

    for (unsigned int cur_type = 0; cur_type < m_type_names.size(); ++cur_type)
        {
        std::string outf_name = m_file_name + "_" + m_type_names[cur_type] + ".dat";
        std::ofstream outf(outf_name.c_str());
        outf.precision(4);
        for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
            {
            boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
            double t = cur_frame->getTime(); 
            outf<<t;
            outf<<"\t"<<msd.x[cur_type][frame_idx];
            outf<<"\t"<<msd.y[cur_type][frame_idx];
            outf<<"\t"<<msd.z[cur_type][frame_idx];
            outf<<std::endl;
            }
        outf.close();
        }
    }

void export_MeanSquaredDisplacement()
    {
    using namespace boost::python;
    class_<MeanSquaredDisplacement, boost::shared_ptr<MeanSquaredDisplacement>, bases<Compute>, boost::noncopyable >
    ("MeanSquaredDisplacement", init< boost::shared_ptr<Trajectory>, const std::string&, const unsigned int& >())
    .def("addType",&MeanSquaredDisplacement::addType)
    .def("deleteType",&MeanSquaredDisplacement::deleteType);
    }
    
