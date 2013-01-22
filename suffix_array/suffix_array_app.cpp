#include "suffix_array_app.h"
void CaculateRepeatSubString(SuffixArray& suffix, std::vector<RepeatSubString>& repeat,
    int minFreq, int minLen)
{
    using namespace std;
    static const uint64_t MAX_REPEATEINFO_MEM = 100000000;//100,000,000 \times 8 = 800M
    static const uint64_t MAX_REPEATEINFO_NUM = 1500;
    vector<int> istack;
    istack.reserve(100);
    repeat.clear();
    int n = suffix.mHeight.size();
    istack.push_back(0);
    uint64_t totalRepeatInfoSize = 0;
    for (int i = 1; i < n; ++i)
    {
        /*  
         * In case there's too many repeate info:
         * the memory occupation:
         * number of repeat info \times average number of location of each repeat info \times 8(bytes)
         * for example, "abcde" repeat 50,k times will occupy:
         * 50,k \times 25,k \times 8 = 10G
         */
        if (repeat.size() > MAX_REPEATEINFO_NUM || totalRepeatInfoSize > MAX_REPEATEINFO_MEM)
        {
            break;
        }
        int h = suffix.mHeight[i];
        while (suffix.mHeight[istack.back()] > h)
        {
            int j = istack.back();
            int hh = suffix.mHeight[j];
            if (hh)
            {

                istack.pop_back();
                while (hh == suffix.mHeight[istack.back()])
                {
                    istack.pop_back();
                }
                int begin = suffix.mSa[istack.back() + 1];
                int end = begin + hh;
                int times = i - istack.back();
                if (times >= minFreq && (end - begin) >= minLen)
                {
                    RepeatSubString repInfo = RepeatSubString(times, begin, end);
                    //record all the locations where the lfs occurs
                    for (int x=istack.back()+1; x<=i; x++)
                    {
                        //int temp = suffix.mSa[x];
                        repInfo.locations.push_back(suffix.mSa[x]);
                    }
                    repeat.push_back(repInfo);
                    totalRepeatInfoSize += repInfo.locations.size(); 
                    if (repeat.size() > MAX_REPEATEINFO_NUM || totalRepeatInfoSize > MAX_REPEATEINFO_MEM)
                    {
                        break;
                    }
                }
            }
        }
        istack.push_back(i);
    }
    if (repeat.empty()) return;
    sort(repeat.begin(), repeat.end());
    n = repeat.size();
    int j = 0;
    for (int i = 1; i < n; ++i)
    {
        if (repeat[j].Contains(repeat[i])) continue;
        repeat[++j] = repeat[i];
    }
    repeat.resize(++j);
}
