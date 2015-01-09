/*! \file Clustering.h
 *  \author Sang Beom Kim
 *  \brief Compute for the clustering of atoms
 */
#ifndef MDALYZER_ANALYZERS_CLUSTERING_H_
#define MDALYZER_ANALYZERS_CLUSTERING_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup analyzers
 */
class Clustering : public Analyzer
    {
    public:
        Clustering(boost::shared_ptr<Trajectory> traj, const std::string& file_name, double atom_dist);
        virtual ~Clustering() {};
        
        virtual void evaluate();
        
        //! set the interatomic distance defining a cluster
        /*! \param distance the distance to use between atoms */
        void setDistance(double distance)
            {
            m_atom_dist_sq = distance*distance;
            }
    private:
        std::string m_file_name;                    //!< Output file name
        double m_atom_dist_sq;                      //!< distance sq criterion (two atoms are regarded to be in one
                                                    //   cluster if their distance is less than this value  
    };

void export_Clustering();
#endif // MDALYZER_ANALYZERS_CLUSTERING_H_
