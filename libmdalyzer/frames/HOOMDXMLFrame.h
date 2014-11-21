#ifndef MDALYZER_FRAMES_HOOMDXMLFRAME_H_
#define MDALYZER_FRAMES_HOOMDXMLFRAME_H_

#include <boost/python.hpp>

#include "Frame.h"
#include "pugixml.hpp"

class HOOMDXMLFrame : public Frame
    {
    public:
        HOOMDXMLFrame(const std::string& file);
        ~HOOMDXMLFrame();
        virtual void readFromFile();
        static float s_supported_hoomd_version;
        
    private:
        const std::string m_file;
        int m_num_particles;
        
        inline void tryParticlesFromNode(pugi::xml_node node);
    };

void export_HOOMDXMLFrame();

#endif //MDALYZER_FRAMES_HOOMDXML_FRAME_H_
