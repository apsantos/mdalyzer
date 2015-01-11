/*!
 * \file RadialDistributionFunction.cc
 * \author Elia Altabet
 * \author Michael P. Howard
 * \date 7 January 2015
 * \brief Implementation of radial distribution function g2(r) analyzer
 */

#include "RadialDistributionFunction.h"
#include "Trajectory.h"
#include "Frame.h"
#include "TriclinicBox.h"

#include <fstream>
#include <algorithm>
#include <cmath>

#include <boost/python.hpp>
#include <boost/math/constants/constants.hpp>

/*!
 * \param traj
 * \param file_name
 * \param delR
 * \param maxR
 * \param nSkip
 */
RadialDistributionFunction::RadialDistributionFunction(boost::shared_ptr<Trajectory> traj,
                                                       const std::string& file_name,
                                                       double delR,
                                                       double maxR,
                                                       unsigned int nSkip)
   : Analyzer(traj), m_file_name(file_name), m_delR(delR), m_maxR(maxR), m_nSkip(nSkip)
   {
   }


/*!
 *
 * \todo need to add option to do g2(r) of specific types e.g. AA AB BB
 */
void RadialDistributionFunction::evaluate()
    {
    std::vector< boost::shared_ptr<Frame> > frames = m_traj->getFrames();
    
    // check that a simulation box is set
    if (!m_traj->hasBox())
        {
        // error! box not found
        throw std::runtime_error("g2(r) needs a simulation box");
        }

    unsigned int nBins = (unsigned int)(m_maxR/m_delR) + 1; // number of bins for histogram
    std::vector<double> vHist (nBins);         

    // build histogram
    Vector3<double> BoxAvg; // for average box size
    TriclinicBox cur_box = m_traj->getBox();
    
    double numFrames(0.0);
    for (unsigned int frame_idx = 0; frame_idx < frames.size(); frame_idx += m_nSkip)
        {
        numFrames += 1.0;
        
        boost::shared_ptr<Frame> cur_frame = frames[frame_idx];

        if (!cur_frame->hasPositions())
            {
            throw std::runtime_error("RadialDistFunc needs positions for all frames");
            }
        std::vector< Vector3<double> > pos = cur_frame->getPositions();

        if (cur_frame->hasBox())
            {
            cur_box = cur_frame->getBox();
            }
        
        Vector3<double> edge_dist = cur_box.getNearestPlaneDistance();
        if (m_maxR > 0.0)
            {
            // 2*maxR must be less than the nearest distance between edges to avoid self-interaction
            if (edge_dist.x <= 2.0*m_maxR || edge_dist.y <= 2.0*m_maxR || edge_dist.z <= 2.0*m_maxR)
                {
                throw std::runtime_error("RadialDistributionFunction: maximum radius exceeds value allowed by periodicity");
                }
            }
        else
            {
            // no radius set, so use the largest value allowed in the box (minimum of edge lengths)
            // will be set only in the first frame
            m_maxR = fmin(edge_dist.x, fmin(edge_dist.y, edge_dist.z));
            }
    
        BoxAvg += cur_box.getLength();

        // the guts of the calculation is here in the particle loop
        for (unsigned int i = 0; i < m_traj->getN(); ++i)
            {
            Vector3<double> cur_pos_i = pos[i];
            for (unsigned int j = i+1; j < m_traj->getN(); ++j)
                {
                Vector3<double> dR = pos[j] - cur_pos_i;
                cur_box.minImage(dR); 
                double dRad = sqrt(dR.dot(dR));
            
                if (dRad <= m_maxR)
                    {
                    int cur_bin = int(dRad/m_delR);
                    vHist[cur_bin] += 2.0;  // update histogram if R < maxR
                    } 
                }
            }
        }

    BoxAvg.x /= numFrames;
    BoxAvg.y /= numFrames;
    BoxAvg.z /= numFrames;

    // normalize histogram
    double rho = (double)(m_traj->getN())/(BoxAvg.x * BoxAvg.y * BoxAvg.z);
    double norm_const = 4.0/3.0*boost::math::constants::pi<double>()*rho;

    for (unsigned int i = 0; i < nBins; ++i)
        {
        double dRlo = m_delR*(double)i;
        double dRhi = dRlo + m_delR;
        double dNideal = norm_const*(dRhi*dRhi*dRhi - dRlo*dRlo*dRlo);
        vHist[i] /= numFrames*dNideal*(double)(m_traj->getN());
        }

    // output to file 

    std::string outf_name = m_file_name;
    std::ofstream outf(outf_name.c_str());
    outf.precision(8);

    outf << "# radial distribution function" << std::endl;
    outf << "# r    g2(r)" << std::endl;

    // dump that output
    double R = m_delR/2.0;
    for (unsigned int i = 0; i < nBins; ++i)
        {
        outf << R << "\t" << vHist[i] << std::endl;
        R += m_delR;
        } 

    outf.close();
}

// Export to Python 

void export_RadialDistributionFunction()
    {
    using namespace boost::python;
    
    class_<RadialDistributionFunction,boost::shared_ptr<RadialDistributionFunction>,bases<Analyzer>,boost::noncopyable>
    ("RadialDistributionFunction", init< boost::shared_ptr<Trajectory>,
                                         const std::string&,
                                         double,
                                         double,
                                         unsigned int >()); 
    }
