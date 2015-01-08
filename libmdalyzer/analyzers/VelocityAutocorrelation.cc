/*!
 * \file VelocityAutocorrelation.cc
 * \author Joseph R. Vella
 * \brief Velocity Autocorrelation Function Analyzer
 * \todo test the functions
 * \ingroup Analyzer
 */
#include "VelocityAutocorrelation.h"
#include "Trajectory.h"
#include "Frame.h"
#include "TriclinicBox.h"

#include <fstream>
#include <algorithm>

#include <boost/python.hpp>

VelocityAutocorrelation::VelocityAutocorrelation(boost::shared_ptr<Trajectory> traj, const std::string& file_name, unsigned int  origins)
    : Analyzer(traj), m_file_name(file_name), m_origins(origins)
    {
    m_type_names.reserve(m_traj->getNumTypes());
    }

/*!
 * \param name Particle type string 
 */    
void VelocityAutocorrelation::addType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
    if (name_it == m_type_names.end())
        {
        m_type_names.push_back(name);
        }
    }

/*!
 * \param name Particle type string 
 */    
void VelocityAutocorrelation::deleteType(const std::string& name)
    {
    std::vector<std::string>::iterator name_it = std::find(m_type_names.begin(), m_type_names.end(), name);
    if (name_it == m_type_names.end())
        {
        throw std::runtime_error("VelocityAutocorrelation cannot remove type that doesn't exit");
        }
    else
        {
        m_type_names.erase(name_it);
        }
    }

/*!
 * Uses time origins to calculate the velocity autocorrelation function
 */    
void VelocityAutocorrelation::evaluate()
    {

    // read the frames and make sure there is time data
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    if (!frames[0]->hasTime())
        {
        // error! there is no time data
        throw std::runtime_error("VelocityAutocorrelation needs data on time");
        }
    if (!m_traj->hasTypes())
        {
        // error! there is no time data
        throw std::runtime_error("VelocityAutocorrelation needs types");
        }
    // set up the velocity autocorrelation function
    Vector3< std::vector< std::vector<double> > > velocauto;
    // if no types are specified, use all particles
    unsigned int type_size = std::max((int)m_traj->getNumTypes(),1); 
    // zero the velocity autocorrelation function values and time counter
    velocauto.x.resize(type_size, std::vector<double>( frames.size(), 0.0 ));
    velocauto.y.resize(type_size, std::vector<double>( frames.size(), 0.0 ));
    velocauto.z.resize(type_size, std::vector<double>( frames.size(), 0.0 ));

    std::vector<unsigned int> type;
    type = m_traj->getTypes();
          
    if (m_type_names.size() != type_size)
        {
        for (unsigned int ipart = 0; ipart < type.size(); ++ipart)
            {
            addType( m_traj->getNameByType( type[ipart] ) );
            }
        }

    std::vector<unsigned int> ntime(frames.size(), 0); 
    std::vector<unsigned int> time0; // vector of time origin frame
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
        {
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
        if (!cur_frame->hasVelocities())
            {
            throw std::runtime_error(
                  "VelocityAutocorrelation needs velocities for all frames");
            }
        std::vector< Vector3<double> > vel = cur_frame->getVelocities();

        // save time origins
        if ( frame_idx % m_origins == 0 )
            {
            time0.push_back(frame_idx);
            }

        for ( unsigned int tau = 0; tau < (time0.size()); ++tau)        
            {
            // set timestep to match the time origin
            unsigned int delta_t = frame_idx - time0[tau];
            if ( delta_t < frames.size() )
                {
                // count occurances each corrected timestep is passed
                ++ntime[delta_t] ;

                boost::shared_ptr<Frame> origin_frame = frames[time0[tau]];
                std::vector< Vector3<double> > origin_vel = origin_frame->getVelocities();

                for (unsigned int iatom = 0; iatom < m_traj->getN(); ++iatom)
                    {
                    // check if this atom is one of our types
                    unsigned int type_idx_iatom = (m_type_names.size() > 0 && m_traj->hasTypes()) ? type[iatom] : 0;
                    Vector3<double> cur_vel = vel[iatom];
                    Vector3<double> cur_origin_vel = origin_vel[iatom];

                    velocauto.x[type_idx_iatom][delta_t] += ( cur_vel.x * cur_origin_vel.x );
                    velocauto.y[type_idx_iatom][delta_t] += ( cur_vel.y * cur_origin_vel.y );
                    velocauto.z[type_idx_iatom][delta_t] += ( cur_vel.z * cur_origin_vel.z );
                    } 
                } 
            }
        }
    write(velocauto, ntime);
    }

/*!
 * \breif write out the total and directional velocity autocorrelation function for each particle type
 * \param velocauto Vector3 struct of 2D vector (particle type, time)
 * \param ntime Histogram of the instances a time bin was visited in the velocity autocorrelation evaluation
 */    
void VelocityAutocorrelation::write( const Vector3< std::vector< std::vector<double> > >& velocauto, const std::vector<unsigned int>& ntime)
    {
    // read the frames and make sure there is time data
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();

    // if no types are specified, use all particles
    unsigned int type_size = std::max((int)m_traj->getNumTypes(),1); 

    std::vector<unsigned int> type;
    if (m_traj->hasTypes())
        {
        type = m_traj->getTypes();
        }
    else
        {
        throw std::runtime_error("VelocityAutocorrelation needs types");
        }

    std::vector<unsigned int> type_map(m_type_names.size());
    std::vector<unsigned int> num_particle_type( type_size, 0 );
    for (unsigned int cur_type = 0; cur_type < m_type_names.size(); ++cur_type)
        {
        // map the string types to indices so we know which to grab
        type_map[cur_type] = m_traj->getTypeByName(m_type_names[cur_type]);

        // count the number of particles in each type
        num_particle_type[cur_type] = std::count ( type.begin(), type.end(), type_map[cur_type] );
        }

    // output
    for (unsigned int cur_type = 0; cur_type < m_type_names.size(); ++cur_type)
        {
        std::string outf_name = m_file_name + "_" + m_type_names[cur_type] + ".dat";
        std::ofstream outf(outf_name.c_str());
        outf.precision(8);
        outf<<"# time total   x    y   z";
        outf<<std::endl;
        for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
            {
            boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
            double time = cur_frame->getTime(); 
            double velocauto_norm = ( ntime[frame_idx] * num_particle_type[cur_type]);
            double velocauto_tot = ( velocauto.x[type_map[cur_type]][frame_idx] + 
                               velocauto.y[type_map[cur_type]][frame_idx] + 
                               velocauto.z[type_map[cur_type]][frame_idx] ) ;
            outf<<time;
            outf<<"\t"<<velocauto_tot/velocauto_norm;
            outf<<"\t"<<velocauto.x[type_map[cur_type]][frame_idx]/velocauto_norm;
            outf<<"\t"<<velocauto.y[type_map[cur_type]][frame_idx]/velocauto_norm;
            outf<<"\t"<<velocauto.z[type_map[cur_type]][frame_idx]/velocauto_norm;
            outf<<std::endl;
            }
        outf.close();
        }
    }

void export_VelocityAutocorrelation()
    {
    using namespace boost::python;
    class_<VelocityAutocorrelation, boost::shared_ptr<VelocityAutocorrelation>, bases<Analyzer>, boost::noncopyable >
    ("VelocityAutocorrelation", init< boost::shared_ptr<Trajectory>, const std::string&, unsigned int >())
    .def("addType",&VelocityAutocorrelation::addType)
    .def("deleteType",&VelocityAutocorrelation::deleteType);
    }