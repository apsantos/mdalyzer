/*! \file GROFrame.h
 *  \author Sang Beom Kim
 */
#ifndef MDALYZER_FRAMES_GROFRAME_H_
#define MDALYZER_FRAMES_GROFRAME_H_

#include "Frame.h"

/*! \class GROFrame
 *  \brief GRO parser
 *
 *  Extension of the Frame data structure that parses GRO file
 *
 *  \ingroup frames
 */
class GROFrame : public Frame
    {
    public:
        GROFrame(const std::string& fileName);
        virtual ~GROFrame() {};
        virtual void readFromFile();
        
    private:
        const std::string m_file;
        
    };

void export_GROFrame();

#endif //MDALYZER_FRAMES_GROFRAME_H_
