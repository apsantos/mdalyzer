#ifndef __FRAME_H__
#define __FRAME_H__

class Frame
    {
    public:
        Frame();
        ~Frame();
        
        virtual double getTime() const = 0;
    };

#endif //__FRAME_H__
