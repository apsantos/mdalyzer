#include "Clustering.h"
#include "Trajectory.h"
#include "Frame.h"
#include "TriclinicBox.h"

#include <fstream>
#include <algorithm>
#include <iomanip>

#include <boost/python.hpp>

Clustering::Clustering(boost::shared_ptr<Trajectory> traj, const std::string& file_name, double atom_dist)
    : Analyzer(traj), m_file_name(file_name), m_atom_dist_sq(atom_dist*atom_dist)
    {
    }

void Clustering::evaluate()
    {
    // read the frames and make sure there is a simulation box
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    if (!m_traj->hasBox())
        {
        // error! box not found
        throw std::runtime_error("Clustering needs a simulation box");
        }
    
    // open the output file and write the first line (comment line)
    std::ofstream outf(m_file_name.c_str());
    outf << "# Clustering" << std::endl
         << "#  - each column contains:" << std::endl
         << "#      1) Frame Number" << std::endl
         << "#      2) Number of Clusters" << std::endl
         << "#      3) Size of the Largest Cluster" << std::endl
         << "# -------------------------------------------------- " << std::endl
         << std::endl;
    
    // compute clustering for each frame
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); ++frame_idx)
        {
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];
        
        // need to use the trajectory box if the current frame doesn't have a box
        TriclinicBox cur_box;
        if (cur_frame->hasBox())
            {
            cur_box = cur_frame->getBox();
            }
        else
            {
            cur_box = m_traj->getBox();
            }
//         Vector3<double> cur_box_len = cur_box.getLength();
        
        // get the number of atoms
        unsigned int cur_n_atom = cur_frame->getN();

        // create a vector for all atoms with their corresponding cluster number
        // - initially, they are set all to "1"
        std::vector<unsigned int> cluster_number(cur_n_atom, 1);
            
        if (!cur_frame->hasPositions())
            {
            throw std::runtime_error("Clustering needs positions for all frames");
            }
        std::vector< Vector3<double> > pos = cur_frame->getPositions();
            
            
        for (unsigned int i=0; i < cur_n_atom; ++i)
            {
            
            Vector3<double> pos_i = pos[i];
            
            for (unsigned int j=i+1; j < cur_n_atom; ++j)
                {
                
//                 Vector3<double> pos_j = pos[j];
                
                // compute distance (minimum image convention)
//                 Vector3<double> dR = pos_i - pos_j;
//                 dR.x -= cur_box_len.x * floor( (dR.x / cur_box_len.x)+0.5 );
//                 dR.y -= cur_box_len.y * floor( (dR.y / cur_box_len.y)+0.5 );
//                 dR.z -= cur_box_len.z * floor( (dR.z / cur_box_len.z)+0.5 );
//                 double dr_sq = (dR.x*dR.x + dR.y*dR.y + dR.z*dR.z);
                
                // compute distance (minimum image convention)
                Vector3<double> dR = pos[j] - pos_i;
                cur_box.minImage(dR);
                double dr_sq = dR.dot(dR);
                
                // if two atoms are separated by distance greater than what was set
                // by the user, assign them into different clusters
                if ( dr_sq > m_atom_dist_sq ) cluster_number[j]++;
                
                }
            }
            
            // calculate cluster statics
            unsigned int n_clusters = *std::max_element(cluster_number.begin(), cluster_number.end());
            
            std::vector<unsigned int> cluster_count(n_clusters, 0);
            for (unsigned int i=0; i < cur_n_atom; ++i)
                {
                cluster_count[cluster_number[i]-1]++;
                }
            unsigned int max_size_cluster = *std::max_element(cluster_count.begin(), cluster_count.end());
            
            // output the statics
            outf << std::setw(8)  << std::right << frame_idx
                 << std::setw(10) << std::right << n_clusters
                 << std::setw(10) << std::right << max_size_cluster
                 << std::endl;
        }
    
    outf.close();
    
    }
    
void export_Clustering()
    {
    using namespace boost::python;
    class_<Clustering, boost::shared_ptr<Clustering>, bases<Analyzer>, boost::noncopyable >
    ("Clustering", init< boost::shared_ptr<Trajectory>, const std::string&, double>())
    .def("setDistance", &Clustering::setDistance);
    }
