#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
namespace std
{
class wordstring: public vector<string>
{
public:
    wordstring() {}
    wordstring(const_iterator b, const_iterator e): vector<string>(b, e) {}
    size_type length() const
    {
        return size();
    }
    wordstring substr(size_type b, size_type n) const
    {
        return wordstring(begin() + b, begin() + b + n);
    }
};

istream& operator>>(istream& in, wordstring& vec) // output
{
    string curr;
    char c;
    int count = 0;
    while(getline(in, curr, ' '))
    {
        if(curr.size() > 0)
        {
            ++count;
            vec.push_back(curr);
        }
    }
    if(curr.size() > 0)
    {
        vec.push_back(curr);
        ++count;
    }
    //cerr << "INTSTRING: READ IN " << count << " words" << endl;
    return in;
}
ostream& operator<<(ostream& out, const wordstring& ws)
{
    for(vector<string>::const_iterator i = ws.begin(); i != ws.end(); i++)
    {
        out << *i << " ";
    }
    return out;
}

}
