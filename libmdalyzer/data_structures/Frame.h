/*!
 * \file Frame.h
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Declaration of Frame data structure
 */
#ifndef MDALYZER_DATASTRUCTURES_FRAME_H_
#define MDALYZER_DATASTRUCTURES_FRAME_H_

#include <stdexcept>
 
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "TriclinicBox.h"
#include "VectorMath.h"

class Trajectory; // forward declaration

//! Frame data structure
/*!
 *  The Frame is the fundamental data structure for a Trajectory (which is simply an array of Frames).
 *  Each Frame stores particle position, velocity, type, diameter, and mass as a SoA to improve cache coherency
 *  when calling Analyzer classes need only certain parts of the data. The Frame also holds the TriclinicBox defining
 *  the boundary of the simulations at a given time.
 */
class Frame : boost::noncopyable
    {
    public:
        //! constructor for a frame of n particles
        Frame(unsigned int n);
        
        //! default destructor
        virtual ~Frame() {};

        /* Setters */
        //! set the snapshot time
        void setTime(double time)
            {
            m_time = time;
            m_has_time = true;
            }   
        //! set the simulation box
        void setBox(const TriclinicBox& box)
            {
            m_box = box;
            m_has_box = true;
            }
        //! set the positions from a list
        void setPositions(const std::vector< Vector3<double> >& positions);
        //! set the position of a single particle
        void setPosition(unsigned int pid, const Vector3<double>& pos);
        
        //! set the velocities from a list
        void setVelocities(const std::vector< Vector3<double> >& velocities);
        //! set the velocity of a single particle
        void setVelocity(unsigned int pid, const Vector3<double>& vel);
        
        //! set the names from a list
        void setNames(const std::vector<std::string>& names);
        //! set the name of a single particle
        void setName(unsigned int pid, const std::string& name);
        
        //! set the types from a list
        void setTypes(const std::vector<unsigned int>& types);
        //! set the type of a single particle
        void setType(unsigned int pid, unsigned int type);
        
        //! set the diameters from a list
        void setDiameters(const std::vector<double>& diams);
        //! set the diameter of a single particle
        void setDiameter(unsigned int pid, double diam);
        
        //! set the masses from a list
        void setMasses(const std::vector<double>& masses);
        //! set the mass of a single particle
        void setMass(unsigned int pid, double mass);

        /* Getters */
        //! get the number of particles
        unsigned int getN() const
            {
            return m_n_particles;
            }
//         unsigned int getNumByType(unsigned int type) const;
//         unsigned int getNumByName(const std::string& name) const;
        
        //! get the snapshot time
        double getTime() const;
        //! get the snapshot box
        const TriclinicBox& getBox() const;
        //! get the list of positions
        const std::vector< Vector3<double> >& getPositions() const;
        //! get the list of velocities
        const std::vector< Vector3<double> >& getVelocities() const;
        //! get the list of names
        const std::vector<std::string>& getNames() const;
        //! get the list of types
        const std::vector<unsigned int>& getTypes() const;
        //! get the list of diameters
        const std::vector<double>& getDiameters() const;
        //! get the list of masses
        const std::vector<double>& getMasses() const;
        
        /* Checkers */
        //! check if snapshot time is set
        bool hasTime()
            {
            return m_has_time;
            }
        //! check if the snapshot box is set
        bool hasBox()
            {
            return m_has_box;
            }
        //! check if the particle positions are set
        bool hasPositions()
            {
            return m_has_positions;
            }
        //! check if the particle velocities are set
        bool hasVelocities()
            {
            return m_has_velocities;
            }
        //! check if the particle names are set
        bool hasNames()
            {
            return m_has_names;
            }
        //! check if the particle types are set
        bool hasTypes()
            {
            return m_has_types;
            }
        //! check if the particle diameters are set
        bool hasDiameters()
            {
            return m_has_diameters;
            }
        //! check if the particle masses are set
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

        bool m_has_time;        //!< Flag for time set
        bool m_has_box;         //!< Flag for box set
        bool m_has_positions;   //!< Flag for positions set
        bool m_has_velocities;  //!< Flag for velocities set
        bool m_has_names;       //!< Flag for names set
        bool m_has_types;       //!< Flag for types set
        bool m_has_diameters;   //!< Flag for diameters set
        bool m_has_masses;      //!< Flag for masses set
    };

//! Python export for Frame
void export_Frame(); 
     
#endif //MDALYZER_DATASTRUCTURES_FRAME_H_
