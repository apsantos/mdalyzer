/*! \file MeanSquaredDisplacement.h
 *  \author Andrew P. Santos
 *  \brief Compute for the mean squared displacement 
 */
#ifndef MDALYZER_COMPUTES_MEANSQUAREDDISPLACEMENTT_H_
#define MDALYZER_COMPUTES_MEANSQUAREDDISPLACEMENTT_H_

#include <string>
#include <vector>
#include <fstream>
#include "Compute.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup computes
 */
class MeanSquaredDisplacement : public Compute
    {
    public:
        MeanSquaredDisplacement(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const unsigned int& origins);
        virtual ~MeanSquaredDisplacement() {};
        
        virtual void evaluate();
        
        void addType(const std::string& name);
        void deleteType(const std::string& name);
        
    private:
        std::string m_file_name;                    //!< Output file name
        unsigned int m_origins;               //!< Number of frames between time origins
        std::vector<std::string> m_type_names;      //!< List of type names to compute on
        
    };

void export_MeanSquaredDisplacement();
#endif // MDALYZER_COMPUTES_MEANSQUAREDDISPLACEMENTT_H_
