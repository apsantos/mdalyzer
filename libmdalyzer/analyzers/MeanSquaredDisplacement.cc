/*!
 * \file MeanSquaredDisplacement.cc
 * \author Andrew P. Santos
 * \brief Mean Squared Displacement of Particles Analyzer
 * \todo test the functions
 * \ingroup Analyzer
 */
#include "MeanSquaredDisplacement.h"
#include "Trajectory.h"
#include "Frame.h"
#include "TriclinicBox.h"

#include <fstream>
#include <algorithm>

#include <boost/python.hpp>

MeanSquaredDisplacement::MeanSquaredDisplacement(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const unsigned int&  origins)
    : Analyzer(traj), m_file_name(file_name), m_origins(origins)
    {
    m_type_names.reserve(m_traj->getNumTypes());
    }

/*!
 * \param name Particle type string 
 */    
void MeanSquaredDisplacement::addType(const std::string& name)
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

/*!
 * Uses time origins to calculate the mean squared disp. of particles of like types
 * using user-defined time origins.  Algorithm is based on Frenkel and Smit's 
 * "Understanding Molecular Simulation". 
 */    
void MeanSquaredDisplacement::evaluate()
    {

    // read the frames and make sure there is time data
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    if (!frames[0]->hasTime())
        {
        // error! there is no time data
        throw std::runtime_error("MeanSquaredDisplacement needs data on time");
        }
    // set up the msd
    Vector3< std::vector< std::vector<float> > > msd;
    // if no types are specified, use all particles
    unsigned int type_size = std::max((int)m_traj->getNumTypes(),1); 
    // zero the msd values and time counter
    msd.x.resize(type_size, std::vector<float>( frames.size(), 0.0 ));
    msd.y.resize(type_size, std::vector<float>( frames.size(), 0.0 ));
    msd.z.resize(type_size, std::vector<float>( frames.size(), 0.0 ));
    std::string outf_name = "msdstuff.dat";
    std::ofstream outf(outf_name.c_str());
    outf.precision(4);
    outf<<m_type_names.size()<<"\n";

    std::vector<unsigned int> type;
    if (frames[0]->hasTypes())
        {
        type = frames[0]->getTypes();
        }
    else if (m_traj->hasTypes())
        {
        type = m_traj->getTypes();
        }
          
    if (m_type_names.size() != type_size)
        {
        for (unsigned int ipart = 0; ipart < type.size(); ++ipart)
            {
            addType( m_traj->getNameByType( type[ipart] ) );
            }
        }

    outf<<m_type_names.size()<<"\n";
    outf.close();

    std::vector<unsigned int> ntime(frames.size(), 0); 
    std::vector<unsigned int> time0; // vector of time origin frame
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
        {
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
        if (!cur_frame->hasPositions())
            {
            throw std::runtime_error(
                  "MeanSquaredDisplacement needs positions for all frames");
            }
        std::vector< Vector3<double> > pos = cur_frame->getPositions();

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
                std::vector< Vector3<double> > origin_pos = origin_frame->getPositions();

                for (unsigned int iatom = 0; iatom < m_traj->getN(); ++iatom)
                    {
                    // check if this atom is one of our types
                    unsigned int type_idx_iatom = (m_type_names.size() > 0 && m_traj->hasTypes()) ? type[iatom] : 0;
                    Vector3<double> cur_pos = pos[iatom];
                    Vector3<double> cur_origin_pos = origin_pos[iatom];
                    Vector3<double> diff_pos = cur_pos - cur_origin_pos;

                    msd.x[type_idx_iatom][delta_t] += ( diff_pos.x * diff_pos.x );
                    msd.y[type_idx_iatom][delta_t] += ( diff_pos.y * diff_pos.y );
                    msd.z[type_idx_iatom][delta_t] += ( diff_pos.z * diff_pos.z );
                    } 
                } 
            }
        }
    write(msd, ntime);
    }

/*!
 * \breif write out the total and directional MSD for each particle type
 * \param msd Vector3 struct of 2D vector (particle type, time)
 * \param ntime Histogram of the instances a time bin was visited in the MSD evaluation
 */    
void MeanSquaredDisplacement::write( const Vector3< std::vector< std::vector<float> > >& msd, const std::vector<unsigned int>& ntime)
    {
    // read the frames and make sure there is time data
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();

    // if no types are specified, use all particles
    unsigned int type_size = std::max((int)m_traj->getNumTypes(),1); 
    std::string outf_name = "msdstuff2.dat";
    std::ofstream outf(outf_name.c_str());
    outf.precision(4);
    outf<<type_size<<"\n";
    outf.close();

    std::vector<unsigned int> type;
    if (frames[0]->hasTypes())
        {
        type = frames[0]->getTypes();
        }
    else if (m_traj->hasTypes())
        {
        type = m_traj->getTypes();
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
        outf.precision(4);
        outf<<"time msd-total   -x    -y   -z";
        outf<<std::endl;
        for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
            {
            boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
            double time = cur_frame->getTime(); 
            double msd_norm = ( ntime[frame_idx] * num_particle_type[cur_type]);
            double msd_tot = ( msd.x[type_map[cur_type]][frame_idx] + 
                               msd.y[type_map[cur_type]][frame_idx] + 
                               msd.z[type_map[cur_type]][frame_idx] ) ;
            outf<<time;
            outf<<"\t"<<msd_tot/msd_norm;
            outf<<"\t"<<msd.x[type_map[cur_type]][frame_idx]/msd_norm;
            outf<<"\t"<<msd.y[type_map[cur_type]][frame_idx]/msd_norm;
            outf<<"\t"<<msd.z[type_map[cur_type]][frame_idx]/msd_norm;
            outf<<std::endl;
            }
        outf.close();
        }
    }

void export_MeanSquaredDisplacement()
    {
    using namespace boost::python;
    class_<MeanSquaredDisplacement, boost::shared_ptr<MeanSquaredDisplacement>, bases<Analyzer>, boost::noncopyable >
    ("MeanSquaredDisplacement", init< boost::shared_ptr<Trajectory>, const std::string&, const unsigned int& >())
    .def("addType",&MeanSquaredDisplacement::addType)
    .def("deleteType",&MeanSquaredDisplacement::deleteType);
    }
    
