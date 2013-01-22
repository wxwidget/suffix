#ifndef SUFFIX_ARRAY_APP_H
#define SUFFIX_ARRAY_APP_H
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include "suffix_array.h"
class RepeatSubString
{
public:
    uint64_t times;
    uint64_t begin;
    uint64_t end;
    //all the locations where the repeat string occurs
    std::vector<uint64_t> locations;
    RepeatSubString(int times = 0, int begin = 0, int end = 0)
            : times(times), begin(begin), end(end) 
    {

    }
    bool Contains(const RepeatSubString& other)
    {
        return times == other.times
               && end >= other.end
               && begin <= other.begin;
    }
    bool operator < (const RepeatSubString& rhs) const
    {
        if (times == rhs.times)
        {
            if (end == rhs.end)
            {
                return begin < rhs.begin;
            }
            return end > rhs.end;
        }
        return times < rhs.times;
    }
    template<typename T>
    inline std::string ToString(const T& idSequence);
    inline uint64_t GetLastIndex() const
    {
        return end - 1;
    }
    inline size_t GetSize()
    {
        return end - begin;
    }
};
inline std::ostream& operator << (std::ostream& out, const RepeatSubString& x)
{
    out << "times:" << x.times
    << " begin:" << x.begin 
    << " end:" << x.end
    << " length: " << x.end - x.begin
    << " locations:";
    for (int i = 0;i < x.locations.size(); ++i)
    {
        out << x.locations[i]<< " ";
    }
    out << std::endl;
    return out;
}
template<typename T>
inline std::string RepeatSubString::ToString(const T& words)
{
    std::string str;
    for (int i = begin; i < end; ++i)
    {
        str += words[i] + ' ';
    }
    return str;
}
void CaculateRepeatSubString(SuffixArray& suffix, std::vector<RepeatSubString>& repeat, 
    int minFreq = 5, int minLen = 2);

#endif 
