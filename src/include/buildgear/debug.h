#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
 
class CDebug
{
    std::ostream    &mStream;
    bool            mOn;
 
public:
    CDebug(std::ostream &str, bool isOn = true)
        : mStream(str)
        , mOn(isOn)
    { }
 
    template <class T>
    inline
    CDebug& operator<<(const T &inVal)
    {
        if (mOn)
            mStream << inVal;
        return *this;
    }
 
    inline
    CDebug& operator<<(std::ostream& (*inVal)(std::ostream&))
    {
        if (mOn)
            mStream << inVal;
        return *this;
    }
 
    void Reset() { mOn = true; }
    bool On() const { return mOn; }
    bool& On() { return mOn; }
};
 
extern CDebug Debug;
 
#endif // DEBUG_H
