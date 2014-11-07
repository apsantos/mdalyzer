#include "Trajectory.h"

Trajectory::Trajectory()
    {
    }
    
Trajectory::~Trajectory()
    {
    }

void Trajectory::analyze()
    {
    boost::shared_ptr<Frame> cur_frame;
    std::vector< boost::shared_ptr<Compute> >::iterator cur_compute;
    
    while ( (cur_frame = getNextFrame()) ) // for each frame
        {
        for (cur_compute = m_computes.begin(); cur_compute != m_computes.end(); ++cur_compute)
            {
            if (cur_compute->shouldCompute()) // if the update period of this compute says we should
                {
                // do the compute
                cur_compute->compute();
                }
            }
        }
    }
