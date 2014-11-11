#ifndef __TRAJECTORY_H__
#define __TRAJECTORY_H__

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include "Frame.h"
#include "Compute.h"

class Trajectory
    {
    public:
        Trajectory();
        virtual ~Trajectory();
        
        /*!
         * Main method to analyze the Trajectory, calling all attached Computes for all Frames
         */
        void analyze();
        
        /*!
         * Force the whole trajectory into memory at one time
         * This could be useful for some types of Compute
         */
        void readAll();
        
        /*!
         * Add a Frame to the Trajectory. This allows us to build a Trajectory from snapshot-style data (e.g. XML, PDB).
         *
         * By adding a Frame, we don't actually load the whole file into memory yet, we just keep the minimal
         * information that we need to read it when it comes time to actually load it. The Trajectory will later
         * load all Frames and then validate that they occur at a unique time and sort them.
         */
        void addFrame(boost::shared_ptr<Frame> frame);
        
        /*!
         * Time order frames in the Trajectory
         * There is no guarantee that the user will provide the frames sorted, but we expect a Trajectory to
         * have the right time ordering when we do computations, so we must sort it ourselves once.
         */
        void sortFrames();
        
        /*!
         * Perform a sanity check on frames to ensure there is no time duplication and that they are properly ordered
         */
        void validate();
        
        /*!
         * Add a Compute that will perform a calculation every certain number of frames
         */
        void addCompute(boost::shared_ptr<Compute> compute, const std::string& name);
        
        /*!
         * Remove a Compute
         */
        void removeCompute(const std::string& name);
        
        /*!
         * Get a Compute
         */
        boost::shared_ptr<Compute> getCompute(const std::string& name);
        
        
    protected:
        bool m_read_from_file; //!< Flag if trajectory should be read from file (true) or from Frames (false)
        
    private:
        std::map< std::string, boost::shared_ptr<Compute> > m_computes; //!< Hold the Computes
        std::vector< boost::shared_ptr<Frame> > m_frames;          //!< Hold the Frames
        
        bool m_sorted;   //<! Flag if Frames require sorting
        
        //! Struct wrapper to sort Frame pointers based on time
        struct time_less_than
        {
        inline bool operator() (boost::shared_ptr<Frame> f1, boost::shared_ptr<Frame> f2)
            {
            return (f1->getTime() < f2->getTime());
            }
        };
    };

#endif //__TRAJECTORY_H__
