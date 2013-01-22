#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H
#include <vector>
#include <tr1/unordered_map>
class SuffixArray
{
public:
    template<typename T>
    static int Word2Id(const T& seq, std::vector<int>& outIds);
    SuffixArray(unsigned defaultBuff = 2048);
    void DA(const std::vector<int>& words);
    void DA(const int* words, unsigned int len);
    void CaculateHeight(const int* words, int n);
    void Reset(int size);
    std::vector<int> mRank;
    std::vector<int> mSa;
    std::vector<int> mHeight;
    std::vector<int> mNSa;
    std::vector<int> mNRank;
    std::vector<int> mCount;
};
template<typename T>
int SuffixArray::Word2Id(const T& seq, std::vector<int>& outIds)
{
    std::tr1::unordered_map<typename T::value_type, uint64_t> reRangeMap;
    uint64_t newid = 0;
    outIds.clear();
    size_t sequenceLen = seq.size();
    outIds.resize(sequenceLen + 1);
    int i = 0;
    typename T::const_iterator iter = seq.begin();
    for (;iter != seq.end() ; ++iter, ++i)
    {
        uint64_t& x = reRangeMap[*iter];
        if (x == 0)
        {
            x = ++newid;
        }
        outIds[i] = x;
    }
    return reRangeMap.size() + 1;
}
#endif //SUFFIX_ARRAY_H
