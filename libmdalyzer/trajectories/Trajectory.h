#ifndef MDALYZER_TRAJECTORY_TRAJECTORY_H_
#define MDALYZER_TRAJECTORY_TRAJECTORY_H_

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "Frame.h"
#include "Analyzer.h"

class Trajectory : boost::noncopyable
    {
    public:
        Trajectory();
        virtual ~Trajectory() {};
        
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
         * Time order frames in the Trajectory
         * There is no guarantee that the user will provide the frames sorted, but we expect a Trajectory to
         * have the right time ordering when we do computations, so we must sort it ourselves once.
         */
        void sortFrames();
        
        const std::vector< boost::shared_ptr<Frame> >& getFrames() const
            {
            return m_frames;
            }
        boost::shared_ptr<Frame> getFrame(unsigned int frame_id) const
            {
            if (frame_id >= m_frames.size())
                throw std::runtime_error("Frame: frame_id out of range");
            return m_frames[frame_id];
            }
        
        /*!
         * Perform a sanity check on frames to ensure there is no time duplication and that they are properly ordered
         */
        void validate();
        
        /*!
         * Add a Compute that will perform a calculation every certain number of frames
         */
        void addCompute(boost::shared_ptr<Analyzer> compute, const std::string& name);
        
        /*!
         * Remove a Compute
         */
        void removeCompute(const std::string& name);
        
        /*!
         * Get a Compute
         */
        boost::shared_ptr<Analyzer> getCompute(const std::string& name);
        
        /*!
         * Particle type converters
         */
        unsigned int addName(const std::string& name);
        void removeName(const std::string& name);
        unsigned int getTypeByName(const std::string& name);
        std::string getNameByType(unsigned int type);
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
        bool hasTypes() const
            {
            return (m_loc_types != NONE);
            }
        bool hasDiameters() const
            {
            return (m_loc_diameters != NONE);
            }
        bool hasMasses() const
            {
            return (m_loc_masses != NONE);
            }
        
        const std::map<std::string, unsigned int>& getTypeMap() const
            {
            return m_type_map;
            }
//         const std::vector<unsigned int>& getTypes(unsigned int frame_id = 0xffffff) const;
//         const std::vector<double>& getDiameters(unsigned int frame_id = 0xffffff) const;
//         const std::vector<double>& getMasses(unsigned int frame_id = 0xffffff) const;
        
    protected:
        bool m_must_read_from_file; //!< Flag if trajectory should be (re-)read from file
        std::vector< boost::shared_ptr<Frame> > m_frames;                   //!< Hold the Frames
        
    private:
        enum PROP_LOCATION {NONE, OWN, FRAME};
        //! parse the trajectory info from the frames
        void parse();
        
        //! for storing information trajectory wide
        TriclinicBox m_box;
        std::vector<std::string> m_names;
        std::vector<unsigned int> m_types;
        std::vector<double> m_diameters;
        std::vector<double> m_masses;
        PROP_LOCATION m_loc_box;
        PROP_LOCATION m_loc_names;
        PROP_LOCATION m_loc_types;
        PROP_LOCATION m_loc_diameters;
        PROP_LOCATION m_loc_masses;
        
        std::map< std::string, boost::shared_ptr<Analyzer> > m_computes;     //!< Hold the Computes
        
        std::map<std::string, unsigned int> m_type_map;                     //!< Map string names to IDs
        unsigned int m_n_particles;                                         //!< Number of particles
        
        bool m_sorted;                                                      //<! Flag if Frames require sorting
    };

void export_Trajectory();

#endif // MDALYZER_TRAJECTORY_TRAJECTORY_H_
