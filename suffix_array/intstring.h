#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
namespace std
{
class intstring: public vector<int>
{
public:
    intstring() {}
    intstring(const_iterator b, const_iterator e): vector<int>(b, e) {}
    size_type
    length() const
    {
        return size();
    }
    intstring substr(size_type b, size_type n) const
    {
        return intstring(begin() + b, begin() + b + n);
    }
};

istream& operator>>(istream& in, intstring& vec) // output
{
    string curr;
    char c;
    int count = 0;
    while(in.good())
    {
        c = in.get();
        if('0' <= c and c <= '9' or (c == '-' and curr.size() == 0))
        {
            curr.push_back(c);
        }
        else
        {
            if(curr.size() > 0)
            {
                ++count;
                vec.push_back(atoi(curr.c_str()));
            }
            curr = string();
            if(c != ' ' and c != '\t' and c != '\n')
            {
                //cerr << "INSTRING READIN found strange character: **" << c << "**" << endl;
            }
        }
    }
    if(curr.size() > 0)
    {
        vec.push_back(atoi(curr.c_str()));
        ++count;
    }
    //cerr << "INTSTRING: READ IN " << count << " integers" << endl;
    return in;
}

ostream& operator<<(ostream& out, const intstring& ws)
{
    for(vector<int>::const_iterator i = ws.begin(); i != ws.end(); i++)
    {
        out << *i << " ";
    }
    return out;
}
}
