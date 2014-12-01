/*! \file XYZFrame.h
 *  \author Joseph R. Vella
 */
#ifndef MDALYZER_FRAMES_XYZFRAME_H_
#define MDALYZER_FRAMES_XYZFRAME_H_

#include "Frame.h"

/*! \class XYZFrame
 *  \brief XYZ parser
 *
 *  Extension of the Frame data structure that parses XYZ file
 *
 *  \ingroup frames
 */
class XYZFrame : public Frame
    {
    public:
        XYZFrame(const std::string& fileName);
        virtual ~XYZFrame() {};
        virtual void readFromFile();
        
    private:
        const std::string m_file;
        
    };

void export_XYZFrame();

#endif //MDALYZER_FRAMES_XYZFRAME_H_
