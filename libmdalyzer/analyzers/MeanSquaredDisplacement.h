/*! \file MeanSquaredDisplacement.h
 *  \author Andrew P. Santos
 *  \brief Compute for the mean squared displacement 
 */
#ifndef MDALYZER_ANALYZERS_MEANSQUAREDDISPLACEMENTT_H_
#define MDALYZER_ANALYZERS_MEANSQUAREDDISPLACEMENTT_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup analyzers
 */
class MeanSquaredDisplacement : public Analyzer
    {
    public:
        MeanSquaredDisplacement(boost::shared_ptr<Trajectory> traj, const std::string& file_name, unsigned int origins);
        virtual ~MeanSquaredDisplacement() {};
        
        virtual void evaluate();
        
        void addType(const std::string& name);
        void deleteType(const std::string& name);
        
    private:
        std::string m_file_name;                    //!< Output file name
        unsigned int m_origins;                     //!< Num of frames between time origins
        std::vector<std::string> m_type_names;      //!< List of type names to compute on
        
        void write( const Vector3< std::vector< std::vector<double> > >& msd, const std::vector<unsigned int>& ntime);
    };

void export_MeanSquaredDisplacement();
#endif // MDALYZER_ANALYZERS_MEANSQUAREDDISPLACEMENTT_H_
