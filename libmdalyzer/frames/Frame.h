/*! \file Frame.h
 *  \author Michael P. Howard
 *  \brief Data structure for holding particle data at a given time in a simulation
 */
#ifndef MDALYZER_FRAMES_FRAME_H_
#define MDALYZER_FRAMES_FRAME_H_
 
/*! \ingroup libmdalyzer
 * @{
 * \defgroup frames
 * \brief All implementations of the Frame data structure
 * @}
 */
 
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "TriclinicBox.h"
#include "VectorMath.h"

/*! \class Frame
 *  The Frame is the fundamental data structure for a Trajectory (which is simply an array of Frames).
 *  Each Frame stores particle position, velocity, type, diameter, and mass as a SoA to improve cache coherency
 *  when calling Compute classes need only certain parts of the data. The Frame also holds the TriclinicBox defining
 *  the boundary of the simulations at a given time.
 *
 *  Accessor methods are provided for data, to be used by Compute classes to acquire read only handles to data.
 *  Inheriting classes should access the protected data members directly to avoid a memcopy.
 *  However, these classes must ensure that the appropriate boolean flags (e.g. m_has_positions) are also set. Each
 *  inheriting class is suggested, but not required, to set the number of particles in the frame.
 *
 *  \ingroup frames
 */
class Frame : boost::noncopyable
    {
    public:
        Frame();
        virtual ~Frame() {};
        
        virtual void readFromFile();
        
        /*!
         * Get methods for frame properties
         */
        double getTime() const
            {
            return m_time;
            }
            
        //! Number of particles
        unsigned int getN();
        
        const std::vector< Vector3<double > >& getPositions() const
            {
            return m_positions;
            }
        const std::vector< Vector3<double> >& getVelocities() const
            {
            return m_velocities;
            }
        const std::vector<std::string>& getTypes() const
            {
            return m_types;
            }
        const std::vector<double>& getDiameters() const
            {
            return m_diameters;
            }
        const std::vector<double>& getMasses() const
            {
            return m_masses;
            }
        const TriclinicBox& getBox() const
            {
            return m_box;
            }

        /*!
         * Checkers for data
         */
        bool hasTime() const
            {
            return m_has_time;
            }
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
        bool hasBox() const
            {
            return m_has_box;
            }
        
        /*!
         * Set methods for frame properties, waste a copy operations for vectors though
         * so only call them if you don't mind this extra step
         */
        void setTime(double time)
            {
            m_has_time = true;
            m_time = time;
            }
        void setPositions(const std::vector< Vector3<double> >& positions)
            {
            m_has_positions = true;
            m_positions = positions;
            }
        void setVelocities(const std::vector< Vector3<double> >& velocities)
            {
            m_has_velocities = true;
            m_velocities = velocities;
            }
        void setTypes(const std::vector<std::string>& types)
            {
            m_has_types = true;
            m_types = types;
            }
        void setDiameters(const std::vector<double>& diameters)
            {
            m_has_diameters = true;
            m_diameters = diameters;
            }
        void setMasses(const std::vector<double>& masses)
            {
            m_has_masses = true;
            m_masses = masses;
            }
        void setBox(const TriclinicBox& box)
            {
            m_has_box = true;
            m_box = box;
            }
            
    protected:
        double m_time;                                  //!< Snapshot time
        unsigned int m_n_particles;                     //!< Number of particles
        std::vector< Vector3<double> > m_positions;     //!< Particle positions
        std::vector< Vector3<double> > m_velocities;    //!< Particle velocities
        std::vector<std::string> m_types;               //!< Particle types
        std::vector<double> m_diameters;                //!< Particle diameters
        std::vector<double> m_masses;                   //!< Particle masses
        
        TriclinicBox m_box;                             //!< Simulation box

        //! Boolean flags for data being set
        bool m_has_time;
        bool m_has_positions;
        bool m_has_velocities;
        bool m_has_types;
        bool m_has_diameters;
        bool m_has_masses;
        bool m_has_box;
    };

void export_Frame();    
#endif //MDALYZER_FRAMES_FRAME_H_
