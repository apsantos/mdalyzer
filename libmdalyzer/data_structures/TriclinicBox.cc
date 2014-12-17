#include "TriclinicBox.h"

TriclinicBox::TriclinicBox()
    : m_length(Vector3<double>()), m_tilt(Vector3<double>())
    {}
    
TriclinicBox::TriclinicBox(Vector3<double> length)
    : m_length(length), m_tilt(Vector3<double>())
    {}

TriclinicBox::TriclinicBox(Vector3<double> length, Vector3<double> tilt)
    : m_length(length), m_tilt(tilt)
    {}