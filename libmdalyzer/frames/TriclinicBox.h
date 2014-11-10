#ifndef __TRICLINIC_BOX_H__
#define __TRICLINIC_BOX_H__

#include "VectorMath.h"

class TriclinicBox
    {
    public:
        TriclinicBox();
        TriclinicBox(Vector3<double> length);
        TriclinicBox(Vector3<double> length, Vector3<double> tilt);
        ~TriclinicBox() {};
        
        Vector3<double> getLength()
            {
            return m_length;
            }
        Vector3<double> getTilt()
            {
            return m_tilt;
            }
        
        void shiftImage(const Vector3<double>& image, Vector3<double> pos)
            {
            pos.x += image.x * m_length.x + m_tilt.x * image.y * m_length.y + m_tilt.y * image.z * m_length.z;
            pos.y += image.y * m_length.y + m_tilt.z * image.z * m_length.z;
            pos.z += image.z * m_length.z;
            }
    
    private:
        Vector3<double> m_length;
        Vector3<double> m_tilt;
    };

#endif