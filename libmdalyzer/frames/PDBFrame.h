/*! \file PDBFrame.h
 *  \author Sang Beom Kim
 */
#ifndef MDALYZER_FRAMES_PDBFRAME_H_
#define MDALYZER_FRAMES_PDBFRAME_H_

#include "Frame.h"

/*! \class PDBFrame
 *  \brief PDB parser
 *
 *  Extension of the Frame data structure that parses PDB file
 *
 *  \ingroup frames
 */
class PDBFrame : public Frame
    {
    public:
        PDBFrame(const std::string& fileName);
        virtual ~PDBFrame() {};
        virtual void readFromFile();
        
    private:
        const std::string m_file;
        
    };

void export_PDBFrame();

#endif //MDALYZER_FRAMES_PDBFRAME_H_
