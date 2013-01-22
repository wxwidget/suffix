#include <algorithm>
#include "suffix_array.h"

SuffixArray::SuffixArray(unsigned defaultBuff)
{
    mRank.reserve(defaultBuff);
    mSa.reserve(defaultBuff);
    mHeight.reserve(defaultBuff);
    mNSa.reserve(defaultBuff);
    mNRank.reserve(defaultBuff);
    mCount.reserve(defaultBuff);
}
void SuffixArray::DA(const std::vector<int>& wordids)
{
    return DA(wordids.data(), wordids.size());
}
void  SuffixArray::DA(const int* wordids, unsigned int n)
{
    mCount.clear();
    mCount.resize(n, 0);
    for (int i = 0; i < n; ++i) ++(mCount[wordids[i]]);
    for (int i = 1; i < n; ++i) mCount[i] += mCount[i-1];
    mSa.resize(n);
    for (int i = 0; i < n; ++i) mSa[--mCount[wordids[i]]] = i;

    mRank.resize(n);
    mRank[mSa[0]] = 0;
    for (int i = 1, k = 0; i < n; ++i)
    {
        k += wordids[mSa[i]] != wordids[mSa[i-1]];
        mRank[mSa[i]] = k;
    }
    mNSa.resize(n);
    mNRank.resize(n);
    for (int x = 1; mRank[mSa[n-1]] < n - 1; x *= 2)
    {
        //mCount.clear();
        //mCount.resize(n, 0);
        memset(mCount.data(), 0, n << 2);
        for (int i = 0; i < n; ++i) ++(mCount[mRank[i]]);
        for (int i = 1; i < n; ++i) mCount[i] += mCount[i-1];
        for (int i = n - 1; i >= 0; --i)
        {
            if (mSa[i] >= x)
            {
                mNSa[--mCount[mRank[mSa[i] - x]]] = mSa[i] - x;
            }
        }
        for (int i = n - x; i < n; ++i)
        {
            mNSa[--mCount[mRank[i]]] = i;
        }
        mNRank[mNSa[0]] = 0;
        for (int i = 1, k = 0; i < n; ++i)
        {
            k += mRank[mNSa[i]] != mRank[mNSa[i-1]] || mRank[mNSa[i] + x] != mRank[mNSa[i-1] + x];
            mNRank[mNSa[i]] = k;
        }
        mSa.swap(mNSa);
        mRank.swap(mNRank);
    }
    CaculateHeight(wordids, n);
}
void SuffixArray::CaculateHeight(const int* wordids, int n)
{
    mHeight.resize(n);
    for (int i = 0, k = 0; i < n; ++i)
    {
        if (mRank[i] == n - 1) mHeight[n-1] = k = 0;
        else
        {
            if (k) --k;
            int j = mSa[mRank[i] + 1];
            while (wordids[i+k] == wordids[j+k]) ++k;
            mHeight[mRank[i]] = k;
        }
    }
}
