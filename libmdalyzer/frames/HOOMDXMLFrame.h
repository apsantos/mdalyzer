#ifndef __HOOMDXMLFRAME_H__
#define __HOOMDXMLFRAME_H__

#include "Frame.h"
#include "pugixml.hpp"

class HOOMDXMLFrame : public Frame
    {
    public:
        HOOMDXMLFrame(const std::string& file);
        ~HOOMDXMLFrame() {};
        virtual void readFromFile();
        static float s_supported_hoomd_version;
        
    private:
        const std::string m_file;
        int m_num_particles;
        
        inline void tryParticlesFromNode(pugi::xml_node node);
    };

#endif