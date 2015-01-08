/*!
 * \file DensityProfile.h
 * \author Michael P. Howard
 * \date 5 January 2014
 * \brief Analyzer for the average temperature profile
 */
#ifndef MDALYZER_ANALYZERS_TEMPERATUREPROFILE_H_
#define MDALYZER_ANALYZERS_TEMPERATUREPROFILE_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup analyzers
 */
class TemperatureProfile : public Analyzer
    {
    public:
        TemperatureProfile(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const Vector3<unsigned int>& bins);
        virtual ~TemperatureProfile() {};
        
        virtual void evaluate();
        
        void addType(const std::string& name);
        void deleteType(const std::string& name);
        
    private:
        std::string m_file_name;                    //!< Output file name
        Vector3<unsigned int> m_bins;               //!< Number of slices in each direction
        std::vector<std::string> m_type_names;      //!< List of type names to compute on
        
        inline void writeHeader(const std::string& direction, std::ofstream& outf) const;
    };

void export_TemperatureProfile();
#endif // MDALYZER_ANALYZERS_TEMPERATUREPROFILE_H_
