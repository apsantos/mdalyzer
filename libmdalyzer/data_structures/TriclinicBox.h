#ifndef MDALYZER_FRAMES_TRICLINIC_BOX_H_
#define MDALYZER_FRAMES_TRICLINIC_BOX_H_

#include "VectorMath.h"

class TriclinicBox
    {
    public:
        TriclinicBox();
        TriclinicBox(Vector3<double> length);
        TriclinicBox(Vector3<double> length, Vector3<double> tilt);
        TriclinicBox(Vector3<double> v1, Vector3<double> v2, Vector3<double> v3);
        ~TriclinicBox() {};
        
        const Vector3<double>& getLength() const
            {
            return m_length;
            }
        const Vector3<double>& getTilt() const
            {
            return m_tilt;
            }
        
        void shiftImage(const Vector3<double>& image, Vector3<double> pos) const
            {
            pos.x += image.x * m_length.x + m_tilt.x * image.y * m_length.y + m_tilt.y * image.z * m_length.z;
            pos.y += image.y * m_length.y + m_tilt.z * image.z * m_length.z;
            pos.z += image.z * m_length.z;
            }
    
    private:
        Vector3<double> m_length;
        Vector3<double> m_tilt;
    };

void export_TriclinicBox();

#endif // MDALYZER_FRAMES_TRICLINIC_BOX_H_
