#ifndef MDALYZER_FRAMES_FRAME_H_
#define MDALYZER_FRAMES_FRAME_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

#include "VectorMath.h"
#include "TriclinicBox.h"

class Frame
    {
    public:
        Frame();
        virtual ~Frame();
        
        virtual void readFromFile();
        
        /*!
         * Get methods for frame properties
         */
        double getTime()
            {
            return m_time;
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
        TriclinicBox getBox()
            {
            return m_box;
            }

        /*!
         * Checkers for data
         */
        bool hasTime()
            {
            return m_has_time;
            }
        bool hasPositions()
            {
            return m_has_positions;
            }
        bool hasVelocities()
            {
            return m_has_velocities;
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
        bool hasBox()
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
        
        //! Frame comparisons are done using time (only variable that makes sense), which we need for sorting.
        inline bool operator< (boost::shared_ptr<Frame> f)
            {
            return (getTime() < f->getTime());
            }
    
    protected:
        double m_time;
        std::vector< Vector3<double> > m_positions;
        std::vector< Vector3<double> > m_velocities;
        std::vector<std::string> m_types;
        std::vector<double> m_diameters;
        std::vector<double> m_masses;
        
        TriclinicBox m_box;

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
