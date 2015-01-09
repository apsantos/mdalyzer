/*!
 * \file TemperatureProfile.cc
 * \author Micheal P. Howard
 * \brief Declaration of TemperatureProfile Analyzer 
 */
#include "TemperatureProfile.h"
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
 * \brief Calculate temperature profile 
 * @}
 */

/*! 
 * \brief TemperatureProfile constructor
 * \param traj Boost shared_ptr to a Trajectory object
 * \param file_name output file name .dat will be appended
 * \param bins user-defined spacing of bins which space is divided
 */
TemperatureProfile::TemperatureProfile(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const Vector3<unsigned int>& bins)
    : Analyzer(traj), m_file_name(file_name), m_bins(bins)
    {
    m_type_names.reserve(m_traj->getNumTypes());
    }

/*! 
 * \brief Add a particle type to the DensityProfile Analyzer
 * \param name Particle type name
 */
void TemperatureProfile::addType(const std::string& name)
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
void TemperatureProfile::deleteType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
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
inline void TemperatureProfile::writeHeader(const std::string& direction, std::ofstream& outf) const
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
 * \brief Main TemperatureProfile routine
 */
void TemperatureProfile::evaluate()
    {
    // read the frames 
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    //! Check if there is a simulation box; throw an exception if there is not one
    if (!m_traj->hasBox())
        {
        // error! box not found
        throw std::runtime_error("TemperatureProfile needs a simulation box in the first frame");
        }
    if (!m_traj->hasMasses())
        {
        throw std::runtime_error("TemperatureProfile: mass must be set to calculate kinetic energy");
        }
    TriclinicBox box = m_traj->getBox();
    Vector3<double> box_len = box.getLength();
    
    // reserve memory for density profile
    // density.x[type][bin]
    Vector3<double> dr(box_len.x/((double)m_bins.x),box_len.y/((double)m_bins.y),box_len.z/((double)m_bins.z));
    
    // container for averaged temperature    
    Vector3< std::vector< std::vector<double> > > temperature;
    
    // number of particles in the bin
    Vector3< std::vector< std::vector<double> > > bin_cnts;
    
    // kinetic energy of the bin
    Vector3< std::vector< std::vector<double> > > two_ke;
    
    unsigned int reserve_size = std::max((int)m_traj->getNumTypes(),1); // if no types are specified, use all particles
        
    if (m_bins.x > 0)
        {
        temperature.x.reserve(reserve_size);
        bin_cnts.x.reserve(reserve_size);
        two_ke.x.reserve(reserve_size);
        }
    if (m_bins.y > 0)
        {
        temperature.y.reserve(reserve_size);
        bin_cnts.y.reserve(reserve_size);
        two_ke.y.reserve(reserve_size);
        }
    if (m_bins.z > 0)
        {
        temperature.z.reserve(reserve_size);
        bin_cnts.z.reserve(reserve_size);
        two_ke.z.reserve(reserve_size);
        }
        
    for (unsigned int i=0; i < reserve_size; ++i)
        {
        if (m_bins.x > 0)
            {
            std::vector<double> type_temp(m_bins.x,0.0);
            temperature.x.push_back(type_temp);
            bin_cnts.x.push_back(type_temp);
            two_ke.x.push_back(type_temp);
            }
        if (m_bins.y > 0)
            {
            std::vector<double> type_temp(m_bins.y,0.0);
            temperature.y.push_back(type_temp);
            bin_cnts.y.push_back(type_temp);
            two_ke.y.push_back(type_temp);
            }
        if (m_bins.z > 0)
            {
            std::vector<double> type_temp(m_bins.z,0.0);
            temperature.z.push_back(type_temp);
            bin_cnts.z.push_back(type_temp);
            two_ke.z.push_back(type_temp);
            }
        }
    
    //! build the temperature profiles
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
        {
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
        if (cur_frame->hasBox())
            {
            TriclinicBox cur_box = cur_frame->getBox();
            if (cur_box.getLength() != box_len)
                {
                throw std::runtime_error("Average TemperatureProfile cannot be computed with variable box size");
                }
            }
        
        if (!cur_frame->hasPositions())
            {
            throw std::runtime_error("TemperatureProfile needs positions for all frames");
            }
        if (!cur_frame->hasVelocities())
            {
            throw std::runtime_error("TemperatureProfile needs velocities for all frames");
            }
        std::vector< Vector3<double> > pos = cur_frame->getPositions();
        std::vector< Vector3<double> > vel = cur_frame->getVelocities();
        
        std::vector<unsigned int> type;
        if (cur_frame->hasTypes())
            {
            type = cur_frame->getTypes();
            }
        else if (m_traj->hasTypes())
            {
            type = m_traj->getTypes();
            }
            
        std::vector<double> mass;
        if (cur_frame->hasMasses())
            {
            mass = cur_frame->getMasses();
            }
        else if (m_traj->hasMasses())
            {
            mass = m_traj->getMasses();
            }
        
        //! clear out the frame counters
        for (unsigned int i=0; i < reserve_size; ++i)
            {
            if (m_bins.x > 0)
                {
                memset(&bin_cnts.x[i][0], 0.0, sizeof(double)*m_bins.x);                
                memset(&two_ke.x[i][0], 0.0, sizeof(double)*m_bins.x);                
                }
            if (m_bins.y > 0)
                {
                memset(&bin_cnts.y[i][0], 0.0, sizeof(double)*m_bins.y);
                memset(&two_ke.y[i][0], 0.0, sizeof(double)*m_bins.y);                  
                }
            if (m_bins.z > 0)
                {
                memset(&bin_cnts.z[i][0], 0.0, sizeof(double)*m_bins.z);
                memset(&two_ke.z[i][0], 0.0, sizeof(double)*m_bins.z);                  
                }
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
            const double cur_two_ke = mass[i]*vel[i].dot(vel[i]);
            if (m_bins.x > 0)
                {
                cur_pos.x -= box_len.x*floor(cur_pos.x/box_len.x);
                const unsigned int cur_idx = (unsigned int)(cur_pos.x/dr.x);
                bin_cnts.x[type_idx_i][cur_idx] += 1.0;
                two_ke.x[type_idx_i][cur_idx] += cur_two_ke;
                }
            if (m_bins.y > 0)
                {
                cur_pos.y -= box_len.y*floor(cur_pos.y/box_len.y);
                const unsigned int cur_idx = (unsigned int)(cur_pos.y/dr.y);
                bin_cnts.y[type_idx_i][cur_idx] += 1.0;
                two_ke.y[type_idx_i][cur_idx] += cur_two_ke;
                }
            if (m_bins.z > 0)
                {
                cur_pos.z -= box_len.z*floor(cur_pos.z/box_len.z);
                const unsigned int cur_idx = (unsigned int)(cur_pos.z/dr.z);
                bin_cnts.z[type_idx_i][cur_idx] += 1.0;
                two_ke.z[type_idx_i][cur_idx] += cur_two_ke;
                }
            }
        
        //! normalize and bin for the frame
        for (unsigned int i=0; i < reserve_size; ++i)
            {
            for (unsigned int cur_bin=0; cur_bin < m_bins.x; ++cur_bin)
                {
                if (bin_cnts.x[i][cur_bin] > 1)
                    {
                    temperature.x[i][cur_bin] += two_ke.x[i][cur_bin]/(3.0*(bin_cnts.x[i][cur_bin]-1.0));
                    }
                }
            for (unsigned int cur_bin=0; cur_bin < m_bins.y; ++cur_bin)
                {
                if (bin_cnts.y[i][cur_bin] > 1)
                    {
                    temperature.y[i][cur_bin] += two_ke.y[i][cur_bin]/(3.0*(bin_cnts.y[i][cur_bin]-1.0));
                    }
                }
            for (unsigned int cur_bin=0; cur_bin < m_bins.z; ++cur_bin)
                {
                if (bin_cnts.z[i][cur_bin] > 1)
                    {
                    temperature.z[i][cur_bin] += two_ke.z[i][cur_bin]/(3.0*(bin_cnts.z[i][cur_bin]-1.0));
                    }
                }
            }
        }
    
    //! handle output to file
    
    // map the string types to indices so we know which to grab
    std::vector<unsigned int> type_map(m_type_names.size());
    for (unsigned int cur_type = 0; cur_type < m_type_names.size(); ++cur_type)
        {
        type_map[cur_type] = m_traj->getTypeByName(m_type_names[cur_type]);
        }
        
    if (m_bins.x > 0)
        {
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
                    outf<<"\t"<<temperature.x[type_map[cur_type]][cur_bin]/(double)frames.size();
                    }
                }
            else
                {
                outf<<"\t"<<temperature.x[0][cur_bin]/(double)frames.size();
                }
            outf<<std::endl;
            }
        
        outf.close();
        }
        
    if (m_bins.y > 0)
        {
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
                    outf<<"\t"<<temperature.y[type_map[cur_type]][cur_bin]/(double)frames.size();
                    }
                }
            else
                {
                outf<<"\t"<<temperature.y[0][cur_bin]/(double)frames.size();
                }
            outf<<std::endl;
            }
        
        outf.close();
        }
    
    if (m_bins.z > 0)
        {
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
                    outf<<"\t"<<temperature.z[type_map[cur_type]][cur_bin]/(double)frames.size();
                    }
                }
            else
                {
                outf<<"\t"<<temperature.z[0][cur_bin]/(double)frames.size();
                }
            outf<<std::endl;
            }
        
        outf.close();
        }
    }
    
//! Export TemperatureProfile Analyzer to Python module
void export_TemperatureProfile()
    {
    using namespace boost::python;
    class_<TemperatureProfile, boost::shared_ptr<TemperatureProfile>, bases<Analyzer>, boost::noncopyable >
    ("TemperatureProfile", init< boost::shared_ptr<Trajectory>, const std::string&, const Vector3<unsigned int>& >())
    .def("addType",&TemperatureProfile::addType)
    .def("deleteType",&TemperatureProfile::deleteType);
    }
    
