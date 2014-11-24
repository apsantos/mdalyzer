#ifndef MDALYZER_TRAJECTORY_TRAJECTORY_H_
#define MDALYZER_TRAJECTORY_TRAJECTORY_H_

#include <vector>
#include <string>
#include <map>
#include <boost/python.hpp>
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
         * Force the whole trajectory into memory at one time. This method defaults to reading all the added Frames,
         * but can be overriden by derived classes to read a single input file at once into those Frames. We call
         * read() at the start of each analysis().
         */
        virtual void read();
        
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
        
        std::vector< boost::shared_ptr<Frame> > getFrames()
            {
            return m_frames;
            }
        
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
        
        /*!
         * Particle type converters
         */
        unsigned int getTypeByName(const std::string& name) const;
        std::string getNameByType(unsigned int type) const;
        unsigned int getNumTypes() const
            {
            return m_type_map.size();
            }
        unsigned int getN() const
            {
            return m_n_particles;
            }
            
        /*!
         * Checkers for data
         */
        bool hasPositions() const
            {
            return m_has_positions;
            }
        bool hasVelocities() const
            {
            return m_has_velocities;
            }
        bool hasTypes() const
            {
            return m_has_types;
            }
        bool hasDiameters() const
            {
            return m_has_diameters;
            }
        bool hasMasses() const
            {
            return m_has_masses;
            }
        
        std::vector<std::string> getTypes() const
            {
            if (m_frames.size() == 0)
                throw std::runtime_error("Can't get types before reading!");
            return m_frames[0]->getTypes();
            }
        std::vector<double> getDiameters() const
            {
            if (m_frames.size() == 0)
                throw std::runtime_error("Can't get diameters before reading!");
            return m_frames[0]->getDiameters();
            }
        std::vector<double> getMasses() const
            {
            if (m_frames.size() == 0)
                throw std::runtime_error("Can't get masses before reading!");
            return m_frames[0]->getMasses();
            }
        
    protected:
        bool m_must_read_from_file; //!< Flag if trajectory should be (re-)read from file

        bool m_has_positions;
        bool m_has_velocities;
        bool m_has_types;
        bool m_has_diameters;
        bool m_has_masses;
        
    private:
        std::map< std::string, boost::shared_ptr<Compute> > m_computes; //!< Hold the Computes
        std::vector< boost::shared_ptr<Frame> > m_frames;          //!< Hold the Frames
        
        std::vector<std::string> m_type_map;
        unsigned int m_n_particles;
        
        bool m_sorted;   //<! Flag if Frames require sorting
    };

void export_Trajectory();

#endif // MDALYZER_TRAJECTORY_TRAJECTORY_H_
