#include "Frame.h"

Frame::Frame()
    : m_time(-1.0), m_has_time(false), m_has_positions(false), m_has_velocities(false), m_has_types(false),
    m_has_diameters(false), m_has_masses(false)
    {
    }
Frame::~Frame()
    {
    }

void Frame::readFromFile()
    {
    }
    
void export_Frame()
    {
    using namespace boost::python;
    class_<Frame, boost::shared_ptr<Frame> >("Frame", init<>())
    .def("readFromFile",&Frame::readFromFile)
    .def("getTime",&Frame::getTime)
    .def("hasTime",&Frame::hasTime)
    .def("hasPositions",&Frame::hasPositions)
    .def("hasVelocities",&Frame::hasVelocities)
    .def("hasTypes",&Frame::hasTypes)
    .def("hasDiameters",&Frame::hasDiameters)
    .def("hasMasses",&Frame::hasMasses)
    .def("hasBox",&Frame::hasBox);
    }
