#include "suffix_array.h"
#include "suffix_array_app.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sys/time.h>
#include "wordstring.h"
#include "intstring.h"
using namespace std;
void test1(wordstring& ws, intstring& ids)
{
   struct timeval start;
   struct timeval end;
   SuffixArray sa;
   gettimeofday(&start,NULL);
   sa.DA(ids); 
   gettimeofday(&end,NULL);
   double dur = 0;
   dur += (end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
   cout<<dur/1000000<<endl;
   cerr << endl;
   vector<RepeatSubString> repeat;
   CaculateRepeatSubString(sa, repeat, 5); 
   for (int i = 0; i < repeat.size(); ++i)
   {
       cout << repeat[i] << repeat[i].ToString(ws) << endl;
   }
}

int main(int argn, char** argv)
{
   wordstring ws;
   while(cin >> ws);
   intstring ids;
   int s = SuffixArray::Word2Id(ws, ids);
   test1(ws, ids);
}
