/*!
 * \file TriclinicBox.h
 * \author Michael P. Howard
 * \date 29 December 2014
 * \brief Declaration of TriclinicBox data structure
 */
#ifndef MDALYZER_FRAMES_TRICLINIC_BOX_H_
#define MDALYZER_FRAMES_TRICLINIC_BOX_H_

#include "VectorMath.h"

//! TriclinicBox
/*!
 * A simulation box with periodic boundaries is defined by three arbitrary 3d lattice vectors oriented in space.
 * These vectors may be simplified to 6 variables: three edge lengths and three angles between them. We convert
 * the angles to tilt factors because it simplifies periodic wrapping. An orthorhombic box has all tilt factors
 * set to zero (90 degree angles). Most Trajectory implementations will extract the TriclinicBox from the data files,
 * but some formats (\sa XYZTrajectory) do not include the box size. These formats must have the TriclinicBox set
 * explicitly on the Python level, and so we export the class constructors.
 */
class TriclinicBox
    {
    public:
        //! default constructor
        TriclinicBox();
        
        //! constructor for orthorhombic box
        TriclinicBox(const Vector3<double>& length);
        
        //! constructor for triclinic box with known tilts
        TriclinicBox(const Vector3<double>& length, const Vector3<double>& tilt);
        
        //! construct a box from three arbitrarily oriented vectors
        TriclinicBox(const Vector3<double>& v1, const Vector3<double>& v2, const Vector3<double>& v3);
        
        //! default destructor
        ~TriclinicBox() {};
        
        //! get the box edge lengths
        const Vector3<double>& getLength() const
            {
            return m_length;
            }
            
        //! get the box tilt factors
        const Vector3<double>& getTilt() const
            {
            return m_tilt;
            }
        
        //! translates positions in place by their image counters
        void shiftImage(const Vector3<double>& image, Vector3<double>& pos) const;
        
        //! computes the minimum image of a vector between particles in the box
        void minImage(Vector3<double>& vec) const;
        
        //! gets the shortest distance between planes in the box
        Vector3<double> getNearestPlaneDistance() const;
    
    private:
        Vector3<double> m_length;   //!< box edge lengths
        Vector3<double> m_tilt;     //!< tilt factors x=xy, y=xz, z=yz in HOOMD nomenclature
    };

//! Python export for TriclinicBox
void export_TriclinicBox();

#endif // MDALYZER_FRAMES_TRICLINIC_BOX_H_
