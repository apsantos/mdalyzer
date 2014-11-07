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
    std::map< std::string, boost::shared_ptr<Compute> >::iterator cur_compute;
    
    while( (cur_frame = getNextFrame()) )
        {
        for (cur_compute = m_computes.begin(); cur_compute != m_computes.end(); ++cur_compute)
            {
            if (cur_compute->second->shouldCompute())
                {
                cur_compute->second->run();
                }
            }
        }
    }
