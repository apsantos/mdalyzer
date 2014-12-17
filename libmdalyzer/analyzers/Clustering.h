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
 * \ingroup computes
 */
class Clustering : public Analyzer
    {
    public:
        Clustering(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const double& atom_dist);
        virtual ~Clustering() {};
        
        virtual void evaluate();
        

    private:
        std::string m_file_name;                    //!< Output file name
        double m_atom_dist;                           //!< distance criterion (two atoms are regarded to be in one
                                                    //   cluster if their distance is less than this value
        
    };

void export_Clustering();
#endif // MDALYZER_ANALYZERS_CLUSTERING_H_
