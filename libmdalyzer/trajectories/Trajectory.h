/*!
 * \file Trajectory.h
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Declaration of Trajectory data structure
 */
#ifndef MDALYZER_TRAJECTORY_TRAJECTORY_H_
#define MDALYZER_TRAJECTORY_TRAJECTORY_H_

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "Frame.h"
#include "Analyzer.h"

/*! \ingroup libmdalyzer
 * @{
 * \defgroup trajectories
 * \brief All implementations of the Trajectory
 * @}
 */

//! Data structure containing particle data from Molecular Dynamics simulations
/*!
 * A Trajectory is the fundamental data structure in molecular simulation tracking the positions, momenta, and
 * other properties of particles as a time series. A Trajectory is decomposed into multiple Frames containing this
 * information. Analyzer objects are attached to the Trajectory, and then called on the data with analyze().
 * 
 * Each Trajectory supplies a read() method that loads Frame objects into memory. Frames are then time
 * sorted and validated to ensure ordering, and relevant Trajectory level parameters are extracted.
 * Currently, the number of particles in a Trajectory must remain constant throughout the simulation.
 *
 * \ingroup trajectories
 * \sa Frame
 * \sa Analyzer
 */
class Trajectory : boost::noncopyable
    {
    public:
        //! default constructor
        Trajectory();
        
        //! default destructor
        virtual ~Trajectory() {};
        
        //! Run analysis on MD simulation data
        /*!
         * Calls evaluate() for all attached Analyzer methods
         */
        void analyze();
        
        //! Attach a file to parse to the Trajectory
        /*!
         * Nearly all Trajectories require a list of files to add. If an inheriting class does not
         * require a list of files, this method may be overridden to throw an exception, or simply ignored.
         */
        virtual void addFile(const std::string& file);
        
        //! Force the whole trajectory into memory at one time.
        /*! 
         * By default, read() is empty. Inheriting classes should supply file format dependent implementations.
         */
        virtual void read();
        
        //! Time order frames in the Trajectory
        /*!
         * There is no guarantee that inheriting classes will provide Frame objects sorted,
         * but each Analyzer should expect a Trajectory to have the right time ordering.
         */
        void sortFrames();
        
        //! get a reference to the time series of Frames
        const std::vector< boost::shared_ptr<Frame> >& getFrames() const
            {
            return m_frames;
            }
        //! get a Frame at a specific time
        boost::shared_ptr<Frame> getFrame(unsigned int frame_id) const
            {
            if (frame_id >= m_frames.size())
                throw std::runtime_error("Frame: frame_id out of range");
            return m_frames[frame_id];
            }
        
        //! Sanity check Frame data
        void validate();
        
        /* Analyzer manipulation */
        //! Attach an Analyzer to the Trajectory by name
        void addAnalyzer(boost::shared_ptr<Analyzer> analyzer, const std::string& name);
        
        //! Remove an Analyzer from the Trajectory by name
        void removeAnalyzer(const std::string& name);
        
        //! Get an Analyzer by name
        boost::shared_ptr<Analyzer> getAnalyzer(const std::string& name);
        
        /* Particle data manipulation */
        //! add a particle name to the type map
        unsigned int addName(const std::string& name);
        
        //! remove a particle name from the type map
        void removeName(const std::string& name);
        
        //! get a type id by name
        unsigned int getTypeByName(const std::string& name);
        
        //! get a particle name by type id
        std::string getNameByType(unsigned int type);
        
        //! get total number of particle types in the map
        unsigned int getNumTypes() const
            {
            return m_type_map.size();
            }
            
        //! get the total number of particles in the Trajectory
        unsigned int getN() const
            {
            return m_n_particles;
            }
            
        /* Checkers */
        //! check if Trajectory has simulation box set
        bool hasBox() const
            {
            return (m_loc_box != NONE);
            }
        //! check if Trajectory has particle names set
        bool hasNames() const
            {
            return (m_loc_names != NONE);
            }
        //! check if Trajectory has particle types set
        bool hasTypes() const
            {
            return (m_loc_types != NONE);
            }
        //! check if Trajectory has particle diameters set
        bool hasDiameters() const
            {
            return (m_loc_diameters != NONE);
            }
        //! check if Trajectory has particle masses set
        bool hasMasses() const
            {
            return (m_loc_masses != NONE);
            }
        
        /* Getters */
        /*! \return simulation box */
        const TriclinicBox& getBox() const
            {
            return m_box;
            }
        /*! \return particle type map */
        const std::map<std::string, unsigned int>& getTypeMap() const
            {
            return m_type_map;
            }
        /*! \return reference to vector of particle names for Trajectory */
        const std::vector<std::string>& getNames() const
            {
            return m_names;
            }
        /*! \return reference to vector of particle types for Trajectory */
        const std::vector<unsigned int>& getTypes() const
            {
            return m_types;
            }
        /*! \return reference to vector of particle masses for Trajectory */
        const std::vector<double>& getMasses() const
            {
            return m_masses;
            }
        /*! \return reference to vector particle diameters for Trajectory */
        const std::vector<double>& getDiameters() const
            {
            return m_diameters;
            }
        
        /* Setters */
        //! set the Trajectory box
        /*! \param box the box to set */
        void setBox(const TriclinicBox& box)
            {
            m_box = box;
            m_loc_box = OWN;
            }
            
        //! set the Trajectory particle names
        /*! 
         * \param names the vector of particle names 
         * \note names will be mapped to types by parse()
         */
        void setNames(const std::vector<std::string>& names)
            {
            m_names = names;
            m_loc_names = OWN;
            }
        
        //! set the Trajectory particle diameters
        /*! \param diameters the vector of particle diameters */
        void setDiameters(const std::vector<double>& diameters)
            {
            m_diameters = diameters;
            m_loc_diameters = OWN;
            }
        
        //! set the Trajectory particle masses
        /*! \param masses the vector of particle masses */
        void setMasses(const std::vector<double>& masses)
            {
            m_masses = masses;
            m_loc_masses = OWN;
            }
        
    protected:
        bool m_must_read_from_file;                         //!< Flag if Trajectory should be (re-)read from file
        std::vector<std::string> m_files;                   //!< List of files to parse
        std::vector< boost::shared_ptr<Frame> > m_frames;   //!< Vector of Frame pointers for the Trajectory
        
    private:
        std::map< std::string, boost::shared_ptr<Analyzer> > m_analyzers;   //!< Hashed list of Analyzers to call
        
        unsigned int m_n_particles;                                         //!< Total number of particles
        std::map<std::string, unsigned int> m_type_map;                     //!< Map string names from all Frames to IDs
        
        bool m_sorted;                                                      //<! Flag if Frames require sorting
        
        //! location of Trajectory data
        /*!
         * Trajectories keep copies of information either specified by the user directly or extracted from the first
         * Frame. Calling Analyzer objects then have the option to use this data, or to read from each Frame
         * individually. This is especially useful for file formats that do not specify information like type, since
         * this information usually does not change during a simulation and can be specified once.
         *
         * Currently, both OWN (user-input) and FRAME (extracted) data is treated equivalently, and this enum might
         * be simplified in favor a bool. However, in the future, we might consider using pointers to the first Frame
         * to avoid a second copy of the data, so we will track the source.
         */
        enum LOCATION
            {
            NONE,   //!< No Trajectory cache of data
            OWN,    //!< User-inputed data
            FRAME   //!< Frame-extracted data
            };
        
        //! extract Frame info to the Trajectory if needed
        void parse();
        
        /* Trajectory level data */
        TriclinicBox m_box;                 //!< Trajectory level copy of simulation box
        std::vector<std::string> m_names;   //!< Trajectory level copy of particle names
        std::vector<unsigned int> m_types;  //!< Trajectory level copy of particle ids
        std::vector<double> m_diameters;    //!< Trajectory level copy of particle diameters
        std::vector<double> m_masses;       //!< Trajectory level copy of particle masses
        
        /* Locations of Trajectory level data */
        LOCATION m_loc_box;         //!< Source of Trajectory box
        LOCATION m_loc_names;       //!< Source of Trajectory particle names
        LOCATION m_loc_types;       //!< Source of Trajectory particle types
        LOCATION m_loc_diameters;   //!< Source of Trajectory particle diameters
        LOCATION m_loc_masses;      //!< Source of Trajectory particle masses
    };

//! Export Trajectory to Python module
void export_Trajectory();

#endif // MDALYZER_TRAJECTORY_TRAJECTORY_H_
