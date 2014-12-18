/*! \file Frame.h
 *  \author Michael P. Howard
 *  \brief Data structure for holding particle data at a given time in a simulation
 */
#ifndef MDALYZER_DATASTRUCTURES_FRAME_H_
#define MDALYZER_DATASTRUCTURES_FRAME_H_
 
/*! \ingroup libmdalyzer
 * @{
 * \defgroup frames
 * \brief All implementations of the Frame data structure
 * @}
 */

#include <stdexcept>
 
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "TriclinicBox.h"
#include "VectorMath.h"

class Trajectory; // forward declaration

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
        Frame(unsigned int n);
        virtual ~Frame() {};

        //! setters 
        void setTime(double time)
            {
            m_time = time;
            m_has_time = true;
            }   
        void setBox(const TriclinicBox& box)
            {
            m_box = box;
            m_has_box = true;
            }
        void setPositions(const std::vector< Vector3<double> >& positions);
        void setPosition(unsigned int pid, const Vector3<double>& pos);
        
        void setVelocities(const std::vector< Vector3<double> >& velocities);
        void setVelocity(unsigned int pid, const Vector3<double>& vel);
        
        void setNames(const std::vector<std::string>& names);
        void setName(unsigned int pid, const std::string& name);
        
        void setTypes(const std::vector<unsigned int>& types);
        void setType(unsigned int pid, unsigned int type);
        
        void setDiameters(const std::vector<double>& diams);
        void setDiameter(unsigned int pid, double diam);
        
        void setMasses(const std::vector<double>& masses);
        void setMass(unsigned int pid, double mass);

        //! getters
        unsigned int getN() const
            {
            return m_n_particles;
            }
        unsigned int getNumByType(unsigned int type) const;
        unsigned int getNumByName(const std::string& name) const;
        
        double getTime() const;
        const TriclinicBox& getBox() const;
        const std::vector< Vector3<double> >& getPositions() const;
        const std::vector< Vector3<double> >& getVelocities() const;
        const std::vector<std::string>& getNames() const;
        const std::vector<unsigned int> getTypes() const;
        const std::vector<double> getDiameters() const;
        const std::vector<double> getMasses() const;
        
        //! checkers
        bool hasTime()
            {
            return m_has_time;
            }
        bool hasBox()
            {
            return m_has_box;
            }
        bool hasPositions()
            {
            return m_has_positions;
            }
        bool hasVelocities()
            {
            return m_has_velocities;
            }
        bool hasNames()
            {
            return m_has_names;
            }
        bool hasTypes()
            {
            return m_has_types;
            }
        bool hasDiameters()
            {
            return m_has_diameters;
            }
        bool hasMasses()
            {
            return m_has_masses;
            }
    
    private:
        const unsigned int m_n_particles;                   //!< Number of particles
        std::map<std::string, unsigned int> m_n_per_name;   //!< Number of particles according to name

        double m_time;                                  //!< Snapshot time
        TriclinicBox m_box;                             //!< Simulation box
        std::vector< Vector3<double> > m_positions;     //!< Particle positions
        std::vector< Vector3<double> > m_velocities;    //!< Particle velocities
        std::vector<std::string> m_names;               //!< Particle names (strings)
        std::vector<unsigned int> m_types;              //!< Particle types (ints, mapped later)
        std::vector<double> m_diameters;                //!< Particle diameters
        std::vector<double> m_masses;                   //!< Particle masses

        bool m_has_time;        
        bool m_has_box;
        bool m_has_positions;
        bool m_has_velocities;
        bool m_has_names;
        bool m_has_types;
        bool m_has_diameters;
        bool m_has_masses;
    };

void export_Frame();      
#endif //MDALYZER_DATASTRUCTURES_FRAME_H_
