/*
 * Copyright (C) 2011-2012  Martin Lund
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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
