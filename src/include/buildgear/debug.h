#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
 
class Debug
{
    std::ostream    &mStream;
    bool            mOn;
 
public:
    Debug(std::ostream &str, bool isOn = true)
        : mStream(str)
        , mOn(isOn)
    { }
 
    template <class T>
    inline
    Debug& operator<<(const T &inVal)
    {
        if (mOn)
            mStream << inVal;
        return *this;
    }
 
    inline
    Debug& operator<<(std::ostream& (*inVal)(std::ostream&))
    {
        if (mOn)
            mStream << inVal;
        return *this;
    }
 
    void Reset() { mOn = true; }
    bool On() const { return mOn; }
    bool& On() { return mOn; }
};
 
extern Debug debug;
 
#endif // DEBUG_H
