/* \file g2r.cc
 * \author Elia (much copied from Mike's Density Profile stuff)
 * \compute for radial distribution function g2(r)
 */

#include "g2r.h"
#include "Trajectory.h"
#include "Frame.h"

#include <fstream>
#include <algorithm>
#include <cmath>

#include <boost/python.hpp>

g2r::g2r(boost::shared_ptr<Trajectory> traj, const std::string& file_name, const double delR, const double maxR)
   : Compute(traj), m_file_name(file_name), m_delR(delR), m_maxR(maxR)
   {
   }


inline Vector3<double>  g2r::wrapCoords(Vector3<double> cur_pos, const Vector3<double> box_len, const Vector3<double> inv_box_len) {
   // we do a wrap of the positions back into an orthorhombic box that ensures they lie on [0,L)
   cur_pos.x -= box_len.x*round(cur_pos.x*inv_box_len.x);
   cur_pos.y -= box_len.y*round(cur_pos.y*inv_box_len.y);
   cur_pos.z -= box_len.z*round(cur_pos.z*inv_box_len.z);
   return cur_pos;
}

void g2r::evaluate(unsigned int nSkip) {  // need to add option to do g2(r) of specific types e.g. AA AB BB
    // skip nSkip frames for calculation
    // read the frames and make sure there is a simulation box
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    if (!frames[0]->hasBox()) {
        // error! box not found
        throw std::runtime_error("g2(r) needs a simulation box in the first frame");
    if (nSkip >= frames.size()) {
        // error! nSkip is negative
        throw std::runtime_error("g2(r) needs a value for nSkip less than the number of frames");
    }


    }

    int nBins = (int)(m_maxR/m_delR) + 1; // number of bins for histogram
    std::vector<double> vHist (nBins);         

    // build histogram
    unsigned int nSkipP1 = nSkip + 1;
    unsigned int numFrames = frames.size()/nSkipP1;          // number of snapshots used in calculation
    unsigned int maxFrame = (frames.size()/nSkipP1)*nSkipP1; 
    Vector3<double> BoxAvg; // for average box size
    for (unsigned int frame_idx = 0; frame_idx <= maxFrame; frame_idx += nSkipP1) {
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];

	if (!cur_frame->hasPositions()) {
	   throw std::runtime_error("g2(r) needs positions for all frames");
	}
        std::vector< Vector3<double> > pos = cur_frame->getPositions();

	if (!cur_frame->hasBox()) {
        // error! box not found
        throw std::runtime_error("g2(r) needs a simulation box in all frames");
	}
        TriclinicBox cur_box = cur_frame->getBox();
	Vector3<double> cur_box_len = cur_box.getLength();
	Vector3<double> inv_box_len;
	BoxAvg.x += cur_box_len.x;
	BoxAvg.y += cur_box_len.y;
	BoxAvg.z += cur_box_len.z;

	inv_box_len.x = 1.0/cur_box_len.x;
	inv_box_len.y = 1.0/cur_box_len.y;
	inv_box_len.z = 1.0/cur_box_len.z;


/*  For if AA BA AA type g2r is needed/ written        
        std::vector<std::string> type;
        if (cur_frame->hasTypes()) {
	   type = cur_frame->getTypes();
	}
        else if (m_traj->hasTypes()) {
	   type = m_traj->getTypes();
	}
*/
        for (unsigned int i=0; i < m_traj->getN(); ++i) {
	   Vector3<double> cur_pos_i = wrapCoords(pos[i], cur_box_len, inv_box_len); 
	   for (unsigned int j=i+1; j < m_traj->getN(); ++j) {
	      Vector3<double> cur_pos_j = wrapCoords(pos[j], cur_box_len, inv_box_len); 
	      Vector3<double> dR;
	      dR.x = cur_pos_i.x - cur_pos_j.x;
	      dR.y = cur_pos_i.y - cur_pos_j.y;
	      dR.y = cur_pos_i.z - cur_pos_j.z;
              // min images
	      dR.x -= cur_box_len.x*round(dR.x * inv_box_len.x); 
	      dR.y -= cur_box_len.y*round(dR.y * inv_box_len.y); 
	      dR.z -= cur_box_len.z*round(dR.z * inv_box_len.z);
	      double dRad = sqrt(dR.x*dR.x + dR.y*dR.y + dR.z*dR.z);
	      if (dRad < m_maxR) {
		 int cur_bin = int(dRad/m_delR);
		 vHist[cur_bin] += 2.0;  // update histogram if R < maxR
	      } 
	   }
	}
    }
    BoxAvg.x /= (double)numFrames;
    BoxAvg.y /= (double)numFrames;
    BoxAvg.z /= (double)numFrames;

    // normalize histrogram
    double dPi = 3.14159265;
    double dRho = (double)(m_traj->getN())/(BoxAvg.x * BoxAvg.y * BoxAvg.z);
    double dConst = 4.0/3.0*dPi*dRho;
    
    for (int i = 0; i <= nBins; ++i) {
       double dRlo = m_delR*(double)i;
       double dRhi = dRlo + m_delR;
       double dNideal = dConst*(dRhi*dRhi*dRhi - dRlo*dRlo*dRlo);
       vHist[i] /= (double)(m_traj->getN()*numFrames)*dNideal;
    }

    // output to file 
    std::string outf_name = m_file_name + ".g2r"; // eventually ammend to reflect types used
    std::ofstream outf(outf_name.c_str());
    outf.precision(8); // just copied from mike... 

    outf << "radial distribution function\n";
    outf << "r    g2(r)\n";

    // dump that output
    double R = m_delR/2.0;
    for (int i = 0; i < nBins; ++i) {
       outf << R << "\t" << vHist[i] << "\n";
       R += m_delR;
    } 
    outf.close();
}


// Export to Python 

void export_g2r() {
    // nope
}
