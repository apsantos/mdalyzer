/* \file g2r.cc
 * \author Elia
 * \compute for radial distribution function g2(r)
 */

#ifndef MDALYZER_COMPUTES_G2R_H_
#define MDALYZER_COMPUTES_G2R_H_

#include <string>
#include <vector>
#include <fstream>
#include "Analyzer.h"
#include "VectorMath.h"

class g2r : public Analyzer {
  public:
   g2r(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const double delR, const double maxR);
   virtual ~g2r() {};

   virtual void evaluate(unsigned int nSkip);    // the guts of the computation 

  private:
   std::string m_file_name;    // Output file name
   double m_delR;              // size of bin (length units of simulation)
   double m_maxR;              // max radius of calculation 
   inline Vector3<double> wrapCoords(Vector3<double> cur_pos, const Vector3<double> box_len, const Vector3<double> inv_box_len);
};

void export_g2r();

#endif // MDALYZER_COMPUTES_G2R_H_
