#ifndef __FRAME_H__
#define __FRAME_H__

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "VectorMath.h"

class Frame
    {
    public:
        Frame();
        virtual ~Frame();
        
        virtual double getTimeFromFile();
        virtual void readFromFile();
        
        /*!
         * Get methods for frame properties
         */
        double getTime()
            {
            return (m_time_set) ? m_time : getTimeFromFile();
            }
        std::vector< Vector3<double > > getPositions()
            {
            return m_positions;
            }
        std::vector< Vector3<double> > getVelocities()
            {
            return m_velocities;
            }
        std::vector<std::string> getTypes()
            {
            return m_types;
            }
        std::vector<double> getDiameters()
            {
            return m_diameters;
            }
        std::vector<double> getMasses()
            {
            return m_masses;
            }

        /*!
         * Checkers for data
         */
        bool hasPositions()
            {
            return m_has_positions;
            }
        bool hasVelocities()
            {
            return m_has_positions;
            }
        bool hasTypes()
            {
            return m_has_positions;
            }
        bool hasDiameters()
            {
            return m_has_positions;
            }
        bool hasMasses()
            {
            return m_has_positions;
            }
        
        /*!
         * Set methods for frame properties
         */
        void setPositions(const std::vector< Vector3<double> >& positions)
            {
            m_positions = positions;
            }
        void setVelocities(const std::vector< Vector3<double> >& velocities)
            {
            m_velocities = velocities;
            }
        void setTypes(const std::vector<std::string>& types)
            {
            m_types = types;
            }
        void setDiameters(const std::vector<double>& diameters)
            {
            m_diameters = diameters;
            }
        void setMasses(const std::vector<double>& masses)
            {
            m_masses = masses;
            }
    
    protected:
        double m_time;
        std::vector< Vector3<double> > m_positions;
        std::vector< Vector3<double> > m_velocities;
        std::vector<std::string> m_types;
        std::vector<double> m_diameters;
        std::vector<double> m_masses;
        
    private:
        bool m_time_set;
        bool m_has_positions;
        bool m_has_velocities;
        bool m_has_types;
        bool m_has_diameters;
        bool m_has_masses;
    };

#endif //__FRAME_H__
