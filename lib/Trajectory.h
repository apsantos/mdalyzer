#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#inndef __TRAJECTORY_H__
#define __TRAJECTORY_H__

class Trajectory
    {
    public:
        Trajectory();
        ~Trajectory();
        
        /*!
         * Analyze the Trajectory
         */
        void analyze();
        
        /*!
         * Add a Frame to the Trajectory, asserting it occurs at a unique time
         */
        void addFrame(boost::shared_ptr<Frame> frame, double time=-1.0);
        
        /*!
         * Remove a Frame at time
         */
        void removeFrame(double time);
        
        /*!
         * Get a Frame at time
         */
        boost::shared_ptr<Frame> getFrame(double time) const;
        
        /*!
         * Time order frames in the Trajectory
         */
        void sortFrames();
        
        
        /*!
         * Add a Compute that calculates every period in time
         */
        void addCompute(boost::shared_ptr<Compute> compute, const std::string& name, double period);
        
        /*!
         * Remove a Compute
         */
        void removeCompute(const std::string& name);
        
        /*!
         * Get a Compute
         */
        boost::shared_ptr<Compute> getCompute(const std::string& name) const;
        
        /*!
         * Update period for Compute
         */
        void setComputePeriod(const std::string& name, double period);
        
        
    private:
        std::vector< boost::shared_ptr<Compute> > m_computes;
        std::vector< boost::shared_ptr<Frame> > m_frames;
    };

#endif