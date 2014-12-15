/*! \file DensityProfile.h
 *  \author Michael P. Howard
 *  \brief Compute for the average density profile
 */
#ifndef MDALYZER_ANALYZERS_DENSITYPROFILE_H_
#define MDALYZER_ANALYZERS_DENSITYPROFILE_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup computes
 */
class DensityProfile : public Analyzer
    {
    public:
        DensityProfile(boost::shared_ptr<Trajectory> traj, const std::string& file_name,const Vector3<unsigned int>& bins);
        virtual ~DensityProfile() {};
        
        virtual void evaluate();
        
        void addType(const std::string& name);
        void deleteType(const std::string& name);
        
        void useMassWeighting(bool mass_weighted)
            {
            m_mass_weighted = mass_weighted;
            }
        
    private:
        std::string m_file_name;                    //!< Output file name
        Vector3<unsigned int> m_bins;               //!< Number of slices in each direction
        std::vector<std::string> m_type_names;      //!< List of type names to compute on
        
        bool m_mass_weighted;                       //!< Mass (true) or number (false) averaged density
        bool m_total_density;                       //!< Compute density of all components
        
        inline void writeHeader(const std::string& direction, std::ofstream& outf) const;
    };

void export_DensityProfile();
#endif // MDALYZER_ANALYZERS_DENSITYPROFILE_H_
