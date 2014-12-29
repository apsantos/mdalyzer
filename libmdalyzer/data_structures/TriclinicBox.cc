/*!
 * \file TriclinicBox.cc
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Implementation of TriclinicBox data structure
 */
#include "TriclinicBox.h"
#include <cmath>
#include <boost/python.hpp>

TriclinicBox::TriclinicBox()
    : m_length(Vector3<double>(0.0,0.0,0.0)), m_tilt(Vector3<double>(0.0,0.0,0.0))
    {}

/*!
 * \param length box edge lengths
 */   
TriclinicBox::TriclinicBox(Vector3<double> length)
    : m_length(length), m_tilt(Vector3<double>(0.0,0.0,0.0))
    {}

/*!
 * \param length box edge lengths
 * \param tilt box tilt factors (dimensionless)
 */
TriclinicBox::TriclinicBox(Vector3<double> length, Vector3<double> tilt)
    : m_length(length), m_tilt(tilt)
    {}

/*!
 * \param v1 a lattice vector
 * \param v2 b lattice vector
 * \param v3 c lattice vector
 *
 * Formulae come from the
 * <a href="https://codeblue.umich.edu/hoomd-blue/doc/page_box.html">HOOMD-blue periodic boundary documentation</a>.
 */
TriclinicBox::TriclinicBox(Vector3<double> v1, Vector3<double> v2, Vector3<double> v3)
    {
    // start by normalizing along x
    m_length.x = sqrt(v1.dot(v1));
    
    // a factor we need...
    double a2x = v1.dot(v2)/m_length.x;
    
    // ...to get length along y
    m_length.y = sqrt(v2.dot(v2) - a2x*a2x);
    
    // ...and the first tilt factor
    m_tilt.x = a2x/m_length.y;
    
    // v1 x v2 is needed to get the box length in z
    Vector3<double> v1xv2 = v1.cross(v2);
    m_length.z = v3.dot(v1xv2)/sqrt(v1xv2.dot(v1xv2));
    
    double a3x = v1.dot(v3)/m_length.x;
    m_tilt.y = a3x/m_length.z; //xz
    m_tilt.z = (v2.dot(v3)-a2x*a3x)/(m_length.y*m_length.z);
    }

/*!
 * \param image 3d vector with counts of times the particle moved through the box
 * \param pos periodically wrapped image
 *
 * Formula taken from
 * <a href="https://codeblue.umich.edu/hoomd-blue/doc/page_xml_file_format.html#sec_xml_image">HOOMD-blue documentation for images</a>.
 */   
void TriclinicBox::shiftImage(const Vector3<double>& image, Vector3<double>& pos) const
    {
    pos.x += image.x * m_length.x + m_tilt.x * image.y * m_length.y + m_tilt.y * image.z * m_length.z;
    pos.y += image.y * m_length.y + m_tilt.z * image.z * m_length.z;
    pos.z += image.z * m_length.z;
    }

void export_TriclinicBox()
    {
    using namespace boost::python;
    
    class_<TriclinicBox, boost::shared_ptr<TriclinicBox> >("TriclinicBox")
    .def(init< Vector3<double> >())
    .def(init< Vector3<double>, Vector3<double> >())
    .def(init< Vector3<double>, Vector3<double>, Vector3<double> >());
    }
