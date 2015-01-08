/*!
 * \file RadialDistributionFunction.h
 * \author Elia Altabet
 * \author Michael P. Howard
 * \date 7 January 2015
 * \brief Declaration of radial distribution function g2(r) analyzer
 */

#ifndef MDALYZER_ANALYZERS_RADIALDISTRIBUTIONFUNCTION_H_
#define MDALYZER_ANALYZERS_RADIALDISTRIBUTIONFUNCTION_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

/*!
 *
 * \ingroup analyzers
 */ 
class RadialDistributionFunction : public Analyzer
    {
    public:
        //! constructor
        RadialDistributionFunction(boost::shared_ptr<Trajectory> traj,
                                   const std::string& file_name,
                                   double delR,
                                   double maxR,
                                   unsigned int nSkip);

        //! destructor
        virtual ~RadialDistributionFunction() {};

        virtual void evaluate();

    private:
        std::string m_file_name;    //!< Output file name
        double m_delR;              //!< size of bin
        double m_maxR;              //!< max radius of calculation 
        unsigned int m_nSkip;       //!< number of frames to skip when averaging
    };

void export_RadialDistributionFunction();

#endif // MDALYZER_ANALYZERS_RADIALDISTRIBUTIONFUNCTION_H_

