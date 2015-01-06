/* \file RadialDistributionFunction.h
 * \author Elia
 * \compute for radial distribution function g2(r)
 */

#ifndef MDALYZER_ANALYZERS_RADIALDISTFUNC_H_
#define MDALYZER_ANALYZERS_RADIALDISTFUNC_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

class RadialDistFunc : public Analyzer {
  public:
   RadialDistFunc(boost::shared_ptr<Trajectory> traj, const std::string& file_name, double delR, double maxR, int nSkip);
   virtual ~RadialDistFunc() {};

   virtual void evaluate();    // the guts of the computation 

  private:
   std::string m_file_name;    // Output file name
   double m_delR;              // size of bin (length units of simulation)
   double m_maxR;              // max radius of calculation 
   int m_nSkip;                // skip nSkip frames for calculation
};

void export_RadialDistFunc();

#endif // MDALYZER_ANALYZERS_RADIALDISTFUNC_H_

