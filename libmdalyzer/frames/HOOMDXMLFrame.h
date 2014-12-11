/*! \file HOOMDXMLFrame.h
 *  \author Michael P. Howard
 */
#ifndef MDALYZER_FRAMES_HOOMDXMLFRAME_H_
#define MDALYZER_FRAMES_HOOMDXMLFRAME_H_

#include "Frame.h"
#include "pugixml.hpp"

/*! \class HOOMDXMLFrame
 *  \brief HOOMD XML parser
 *
 *  Extension of the Frame data structure that parses HOOMD XML output (v. 1.0.0 or more recent). The pugixml reader
 *  is used to 
 *
 *  \ingroup frames
 */
class HOOMDXMLFrame : public Frame
    {
    public:
        HOOMDXMLFrame(const std::string& file);
        virtual ~HOOMDXMLFrame() {};
        virtual void readFromFile();
        static float s_supported_hoomd_version;
    private:
        const std::string m_file;
        
        inline void tryParticlesFromNode(pugi::xml_node node);
        inline void updateParticleCount(unsigned int cur_particle);
        
        template<typename T>
        inline void fasterPushBack(const T val, unsigned int cur_particle, std::vector<T>& vec);
    };

void export_HOOMDXMLFrame();

#endif //MDALYZER_FRAMES_HOOMDXML_FRAME_H_
