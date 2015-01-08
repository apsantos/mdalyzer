/*! \file VelocityAutocorrelation.h
 *  \author Joseph R. Vella
 *  \brief Compute for the velocity autocorrelation function
 */
#ifndef MDALYZER_ANALYZERS_VELOCITYAUTOCORRELATION_H_
#define MDALYZER_ANALYZERS_VELOCITYAUTOCORRELATION_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup Analyzer
 */
class VelocityAutocorrelation : public Analyzer
    {
    public:
        VelocityAutocorrelation(boost::shared_ptr<Trajectory> traj, const std::string& file_name, unsigned int origins);
        virtual ~VelocityAutocorrelation() {};
        
        virtual void evaluate();
        
        void addType(const std::string& name);
        void deleteType(const std::string& name);
        
    private:
        std::string m_file_name;                    //!< Output file name
        unsigned int m_origins;                     //!< Num of frames between time origins
        std::vector<std::string> m_type_names;      //!< List of type names to compute on
        
        void write( const Vector3< std::vector< std::vector<double> > >& velocauto, const std::vector<unsigned int>& ntime);
    };

void export_VelocityAutocorrelation();
#endif // MDALYZER_ANALYZERS_VELOCITYAUTOCORRELATION_H_
