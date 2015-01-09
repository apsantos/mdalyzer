/*!
 * \file DensityProfile.cc
 * \author Micheal P. Howard
 * \brief Declaration of DensityProfile Analyzer 
 */
#include "DensityProfile.h"
#include "Trajectory.h"
#include "Frame.h"
#include "TriclinicBox.h"

#include <cmath>
#include <fstream>
#include <algorithm>

#include <boost/python.hpp>
/*! \ingroup libmdalyzer
 * @{
 * \defgroup analyzers
 * \brief Calculate density of particles and/or group of particles as a function of position
 * @}
 */

/*! 
 * \brief DensityProfile constructor
 * \param traj Boost shared_ptr to a Trajectory object
 * \param file_name output file name .dat will be appended
 * \param bins user-defined spacing of bins which space is divided
 */

DensityProfile::DensityProfile(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const Vector3<unsigned int>& bins)
    : Analyzer(traj), m_file_name(file_name), m_bins(bins), m_mass_weighted(true)
    {
    m_type_names.reserve(m_traj->getNumTypes());
    }

/*! 
 * \brief Add a particle type to the DensityProfile Analyzer
 * \param name Particle type name
 */
void DensityProfile::addType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
    if (name_it == m_type_names.end())
        {
        m_type_names.push_back(name);
        }
    }

/*! 
 * \brief Remove a particle type to the DensityProfile Analyzer
 * \param name Particle type name
 */
void DensityProfile::deleteType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
    // Throw an error if the type to be deleted is not part of the trajectory
    if (name_it == m_type_names.end())
        {
        throw std::runtime_error("DensityProfile cannot remove type that doesn't exit");
        }
    else
        {
        m_type_names.erase(name_it);
        }
    }

/*! 
 * \brief Write the header of the output file
 * \param direction cartesian direction (x, y or z)
 * \param outf fstream output file object
 */
inline void DensityProfile::writeHeader(const std::string& direction, std::ofstream& outf) const
    {
    // output header
    outf<<"# "<<direction;
    if (m_type_names.size() > 0)
        {
        for (unsigned int cur_type = 0; cur_type < m_type_names.size(); ++cur_type)
            {
            outf<<"\t"<<m_type_names[cur_type];
            }
        }
    else
        {
        outf<<"\taverage";
        }
    outf<<std::endl;
    }

/*! 
 * \brief Main DensityProfile routine
 */
void DensityProfile::evaluate()
    {
    // read the frames 
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    // Check if there is a simulation box; throw an exception if there is not one
    if (!m_traj->hasBox())
        {
        // error! box not found
        throw std::runtime_error("DensityProfile needs a simulation box in the first frame");
        }
    TriclinicBox box = m_traj->getBox();
    Vector3<double> box_len = box.getLength();

    // Create the density data structure
    // reserve memory for density profile
    // density.x[type][bin]
    Vector3<double> dr(box_len.x/((double)m_bins.x),box_len.y/((double)m_bins.y),box_len.z/((double)m_bins.z));
        
    Vector3< std::vector< std::vector<double> > > density;
    unsigned int reserve_size = std::max((int)m_traj->getNumTypes(),1); // if no types are specified, use all particles
        
    if (m_bins.x > 0) density.x.reserve(reserve_size);
    if (m_bins.y > 0) density.y.reserve(reserve_size);
    if (m_bins.z > 0) density.z.reserve(reserve_size);
    for (unsigned int i=0; i < reserve_size; ++i)
        {
        if (m_bins.x > 0)
            {
            std::vector<double> type_dens(m_bins.x,0.0);
            density.x.push_back(type_dens);
            }
        if (m_bins.y > 0)
            {
            std::vector<double> type_dens(m_bins.y,0.0);
            density.y.push_back(type_dens);
            }
        if (m_bins.z > 0)
            {
            std::vector<double> type_dens(m_bins.z,0.0);
            density.z.push_back(type_dens);
            }
        }
    
    // build the density profiles
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
        {
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
        // Check that the frame has a box; throw an error if it does not match the trajectory's
        if (cur_frame->hasBox())
            {
            TriclinicBox cur_box = cur_frame->getBox();
            if (cur_box.getLength() != box_len)
                {
                throw std::runtime_error("Average DensityProfile cannot be computed with variable box size");
                }
            }
        
        // Check if Frames have postions; throw an exception if not
        if (!cur_frame->hasPositions())
            {
            throw std::runtime_error("DensityProfile needs positions for all frames");
            }
        std::vector< Vector3<double> > pos = cur_frame->getPositions();
        
        // Get types from the Frame or the trajectory
        std::vector<unsigned int> type;
        if (cur_frame->hasTypes())
            {
            type = cur_frame->getTypes();
            }
        else if (m_traj->hasTypes())
            {
            type = m_traj->getTypes();
            }
            
        // Get masses from the Frame or the trajectory
        std::vector<double> mass;
        if (cur_frame->hasMasses())
            {
            mass = cur_frame->getMasses();
            }
        else if (m_traj->hasMasses())
            {
            mass = m_traj->getMasses();
            }
            
        for (unsigned int i=0; i < m_traj->getN(); ++i)
            {
            // check if this atom is one of our types
            unsigned int type_idx_i = (m_type_names.size() > 0 && m_traj->hasTypes()) ? type[i] : 0;
            
            /*! Wrap the positions back into an orthorhombic box that ensures they lie on [0,L)
             * since we are not in the business of doing triclinic density profiles. This ensures that the
             * bin we floor will be in the right range
             */
            Vector3<double> cur_pos = pos[i];
            if (m_bins.x > 0)
                {
                cur_pos.x -= box_len.x*floor(cur_pos.x/box_len.x);
                density.x[type_idx_i][(unsigned int)(cur_pos.x/dr.x)] += ((m_mass_weighted && m_traj->hasMasses()) ? mass[i] : 1.0);
                }
            if (m_bins.y > 0)
                {
                cur_pos.y -= box_len.y*floor(cur_pos.y/box_len.y);
                density.y[type_idx_i][(unsigned int)(cur_pos.y/dr.y)] += ((m_mass_weighted && m_traj->hasMasses()) ? mass[i] : 1.0);
                }
            if (m_bins.z > 0)
                {
                cur_pos.z -= box_len.z*floor(cur_pos.z/box_len.z);
                density.z[type_idx_i][(unsigned int)(cur_pos.z/dr.z)] += ((m_mass_weighted && m_traj->hasMasses()) ? mass[i] : 1.0);
                }
            }
        }
    
    // handle output
    
    // map the string types to indices so we know which to grab
    std::vector<unsigned int> type_map(m_type_names.size());
    for (unsigned int cur_type = 0; cur_type < m_type_names.size(); ++cur_type)
        {
        type_map[cur_type] = m_traj->getTypeByName(m_type_names[cur_type]);
        }
        
    if (m_bins.x > 0)
        {
        double density_norm = dr.x*box_len.y*box_len.z*(double)frames.size();
        std::string outf_name = m_file_name + ".x.dat";
        std::ofstream outf(outf_name.c_str());
        outf.precision(8);
        
        writeHeader("x", outf);
        
        // dump the output
        for (unsigned int cur_bin = 0; cur_bin < m_bins.x; ++cur_bin)
            {
            outf<<(0.5+(double)(cur_bin))*dr.x;
            if (type_map.size() > 0)
                {
                for (unsigned int cur_type = 0; cur_type < type_map.size(); ++cur_type)
                    {
                    outf<<"\t"<<density.x[type_map[cur_type]][cur_bin]/density_norm;
                    }
                }
            else
                {
                outf<<"\t"<<density.x[0][cur_bin]/density_norm;
                }
            outf<<std::endl;
            }
        
        outf.close();
        }
        
    if (m_bins.y > 0)
        {
        double density_norm = dr.y*box_len.x*box_len.z*(double)frames.size();
        std::string outf_name = m_file_name + ".y.dat";
        std::ofstream outf(outf_name.c_str());
        outf.precision(8);
        
        writeHeader("y", outf);
        
        // dump the output
        for (unsigned int cur_bin = 0; cur_bin < m_bins.y; ++cur_bin)
            {
            outf<<(0.5+(double)(cur_bin))*dr.y;
            if (type_map.size() > 0)
                {
                for (unsigned int cur_type = 0; cur_type < type_map.size(); ++cur_type)
                    {
                    outf<<"\t"<<density.y[type_map[cur_type]][cur_bin]/density_norm;
                    }
                }
            else
                {
                outf<<"\t"<<density.y[0][cur_bin]/density_norm;
                }
            outf<<std::endl;
            }
        
        outf.close();
        }
    
    if (m_bins.z > 0)
        {
        double density_norm = dr.z*box_len.x*box_len.y*(double)frames.size();
        std::string outf_name = m_file_name + ".z.dat";
        std::ofstream outf(outf_name.c_str());
        outf.precision(8);
        
        writeHeader("z", outf);
        
        // dump the output
        for (unsigned int cur_bin = 0; cur_bin < m_bins.z; ++cur_bin)
            {
            outf<<(0.5+(double)(cur_bin))*dr.z;
            if (type_map.size() > 0)
                {
                for (unsigned int cur_type = 0; cur_type < type_map.size(); ++cur_type)
                    {
                    outf<<"\t"<<density.z[type_map[cur_type]][cur_bin]/density_norm;
                    }
                }
            else
                {
                outf<<"\t"<<density.z[0][cur_bin]/density_norm;
                }
            outf<<std::endl;
            }
        
        outf.close();
        }
    }
    
//! Export DensityProfile Analyzer to Python module
void export_DensityProfile()
    {
    using namespace boost::python;
    class_<DensityProfile, boost::shared_ptr<DensityProfile>, bases<Analyzer>, boost::noncopyable >
    ("DensityProfile", init< boost::shared_ptr<Trajectory>, const std::string&, const Vector3<unsigned int>& >())
    .def("addType",&DensityProfile::addType)
    .def("deleteType",&DensityProfile::deleteType)
    .def("useMassWeighting",&DensityProfile::useMassWeighting);
    }
    
