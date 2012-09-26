#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <vector>
#include <sstream>
#include <cassert>
using namespace std;
#include <iostream>
#include <string>
#include "object_pool.hpp"
#include "hash_table.hpp"
#include "unittest.hpp"
using namespace std;
class Node;
class KSuffixTree;
typedef  HashTable<char, Node*> Map;
//typedef  map<char, Node*> Map;
class Suffix
{
public :
    Node* origin_node;
    Node* end_node;
    int first_char_index;
    int last_char_index;
    Suffix(Node* node, int start, int stop)
        : origin_node(node),end_node(NULL),
          first_char_index(start),
          last_char_index(stop)
    {
    };
    int isExplicit()
    {
        return first_char_index > last_char_index;
    }
    int isImplicit()
    {
        return last_char_index >= first_char_index;
    }
    inline void canonize(const char*, Node* child = NULL);
    void canonize_r(const char* env, Node* child = NULL);
};

#define ITEM_POOL
class Node// or an edge
{
public:
#ifdef ITEM_POOL
    DECLARE_MEMORY_POOL(Node);
#endif
    Node *parent, *suffixLink;
    const char* edgeStart, *edgeEnd;
    int parentDepth;
    int count;
    Map sons;
    Node(): parent(NULL),suffixLink(NULL),
        edgeStart(NULL), edgeEnd(NULL),parentDepth(0),
        count(0), sons()
    {
        
    }
    void printEdge(std::ostream& os)
    {
        const char*p = edgeStart;
        while(p != edgeEnd) 
        {
            os << (int)(unsigned char)(*p++) << ',';
        }
        os << (int)(unsigned char)(*p);
    }
    int children()
    {
        return valid()? sons.size(): -1;
    }
    bool valid()
    {
        return parentDepth != -2;
    }
    Node& operator [](char c)
    {
        static Node nullNode;
        nullNode.parentDepth = -2;
        Map::iterator it = sons.find(c);
        if (it != sons.end())
        {
            return *it->second;
        }
        return nullNode;
    }
    bool inEdge(int position)
    {
        return parentDepth <= position && position < depth();
    }
    int edgeLength()
    {
        return edgeStart != NULL ? 1 + (int)(edgeEnd - edgeStart):0;
    }
    int depth()
    {
        return parentDepth + edgeLength();
    }
    Node* find(char start)
    {
        Map::iterator it = sons.find(start);
        if (it != sons.end())
        {
            return it->second;
        }
        return NULL;
    }
    void link(Node* son, const char* start, const char* end, const char* s)
    {
        son->parent = this;
        son->parentDepth = depth();
        son->edgeStart = start;
        son->edgeEnd = end;
        sons[*start] = son;
    }
    bool isLeaf()
    {
        return sons.empty();
    }
};
void Suffix::canonize_r(const char* T, Node* child)
{
    Node* node = child == NULL?origin_node->find(T[first_char_index]):child;
    int  edge_span = node->edgeEnd - node->edgeStart;
    while(edge_span <= (last_char_index - first_char_index))
    {
        first_char_index = first_char_index + edge_span + 1;
        origin_node = node;
        if(first_char_index <= last_char_index)
        {
            node = node->find(T[first_char_index]);
            edge_span = node->edgeEnd - node->edgeStart;
        };
    }
    end_node = node;
}
inline void Suffix::canonize(const char* T, Node* child)
{
    if(!isExplicit())
    {
        return canonize_r(T, child);
    }
}
class KSuffixTree
{
public:
    vector<Node*> nodes;
    Node *root;
    int maxDepth;
    int currentNode;
    static const char DEFAULT_TERM = '$';
    char terminator; 
public:
    ~KSuffixTree()
    {
#ifndef ITEM_POOL
        for(unsigned int i = 0; i <  nodes.size(); ++i)
        {
            delete nodes[i];
        }
#endif
    }
    KSuffixTree(const char* str, int strlen, int mx, char t=DEFAULT_TERM):
        maxDepth(mx),currentNode(0), terminator(t)
    {
        root = newNode();
        build(root, str, strlen, mx);
    }
    KSuffixTree(int max_depth,char t=DEFAULT_TERM):
        maxDepth(max_depth),currentNode(0), terminator(t)
    {
        root = newNode();
    }
    void setTerminator(char c)
    {
        terminator = c;
    }
    int nofnodes()
    {
        return currentNode;
    }
    Node* newNode()
    {
        Node* s = new Node();
#ifndef ITEM_POOL
        nodes.push_back(s);
#endif
        currentNode++;
        return s;
    }
    inline void updatePathCount(Node* from)
    {
        from->count++;
    }
    Node* splitEdge(Node* node, const char* where, const char* T)
    {
        Node* split = newNode();
        split->suffixLink = node->parent;
        node->parent->link(split, node->edgeStart, where, T);
        split->link(node, where + 1, node->edgeEnd, T);
        return split;
    }
    void build(Node* root, const char* str, int length, int max_depth)
    {
        Suffix active(root, 0, -1);
        for(int i = 0; i < length; ++i)
            update(root, active, i, str, length, max_depth);
    }
    void add(const char* str, int str_len)
    {
        Suffix active(root, 0, -1);
        int i = 0;
        for(i = 0; i < str_len ; ++i)
            update(root, active, i, str, str_len, maxDepth);
    }
    void update(Node* root, Suffix& active, int last_char_index, const char* T, int length, int max_depth)
    {
        Node* parent_node = NULL;
        Node* last_parent_node = NULL;
        int breakRule = 4;
        Node* edge = NULL;
        while(active.first_char_index <= last_char_index)
        {
            edge = NULL;
            parent_node = active.origin_node;
            bool exceed = false;
            // Step 1 is to try and find a matching edge for the given node.
            // If a matching edge exists, we are done adding edges, so we break
            // out of this big loop.
            //
            if(active.isExplicit())
            {
                //empty char
                if (last_char_index == length - 1 && active.origin_node == root)
                {
                    return;
                }
                if (active.origin_node->depth() >= max_depth)
                {
                    exceed = true;
                    updatePathCount(active.origin_node);
                }
                else 
                {
                    edge = active.origin_node->find(T[last_char_index]);
                    if(edge != NULL)
                    {
                        //ab$  abab$
                        if (*edge->edgeStart == terminator)
                        {
                            exceed = true;
                            updatePathCount(edge);
                        }
                        else
                        {
                            breakRule = 1;
                            break;
                        }
                    }
                }
            }
            else //implicit node, a little more complicated
            {
                int span = active.last_char_index - active.first_char_index;
                if (active.origin_node->depth() + span + 1>= max_depth)
                {
                    exceed = true;
                    updatePathCount(active.end_node);
                }
                else
                {
                    if (active.end_node && active.end_node->parent == active.origin_node)
                    {
                        edge = active.end_node;
                    }
                    else
                    {
                        edge = active.origin_node->find(T[active.first_char_index]);
                    }
                    //edge = active.origin_node->find(T[active.first_char_index]);
                    const char* where = edge->edgeStart + span;
                    if(*(where + 1) == T[last_char_index]) 
                    {
                        if(T[last_char_index] == terminator)
                        {
                            exceed = true;
                            updatePathCount(edge);
                        }
                        else
                        {
                            breakRule = 2; 
                            break;
                        }
                    }
                    else
                    {
                        parent_node = splitEdge(edge, where, T);
                    }
                }
            }
            //
            // We didn't find a matching edge, so we create a new one, add
            // it to the tree at the parent node position, and insert it
            // into the hash table.  When we create a new node, it also
            // means we need to create a suffix link to the new node from
            // the last node we visited.
            if(last_parent_node != NULL)
            {
                last_parent_node->suffixLink = parent_node;
                last_parent_node = parent_node;//test5
            }
            if (!exceed)
            {
                //leaf should end with an extra node
                Node* nNode = newNode();
                parent_node->link(nNode, T + last_char_index, T + length, T);
                last_parent_node = parent_node;
                updatePathCount(nNode);
            }
            //
            // This final step is where we move to the next smaller suffix
            if(active.origin_node == root)
            {
                active.first_char_index++;
            }
            else
            {
                active.origin_node = active.origin_node->suffixLink;
            }
            active.canonize(T);
        }
        if(last_parent_node)
            last_parent_node->suffixLink = parent_node;
        active.last_char_index++;  
        if (breakRule < 3)
        {
            active.canonize(T,edge);
        }
        else
        {
            active.canonize(T, active.end_node);
        }
        //or just active.canonize(T);
    }
    //e.x  ab*ab
    int similarCount(const char* param, int from, int to, char any)
    {
        return similarCount(root, param, from, to , any);
    }
    //[from to]
    int similarCount(Node* node, const char* param, int from , int to, char any)
    {
        int result = 0;
        if (node == NULL) return 0;
        if (from > to)
        {
            return 0;
        }
        if(param[from] == any)
        {
            Map& child = node->sons;
            for(Map::iterator it = child.begin(); it != child.end(); ++it)
            {
                if (terminator == *it->second->edgeStart)
                {
                    continue;
                }
                if(from == to)
                {
                    result += it->second->count;
                }
                else
                {
                    int nfrom = from + 1;
                    const char* p = it->second->edgeStart + 1;
                    bool match = true;
                    while(p != it->second->edgeEnd + 1 && nfrom <= to)
                    {
                        if (*(param + nfrom) != *p && *(param + nfrom) != any)  
                        {
                            match = false;
                            break;
                        }
                        if (*p == terminator)
                        {
                            match = false;
                            break;
                        }
                        ++nfrom;
                        ++p;
                    }
                    if (match)
                    {
                        if(nfrom > to)
                        {
                            result += it->second->count;
                        }
                        else
                        {
                            result += similarCount(it->second, param, nfrom, to, any);
                        }
                    }
                }
            }
        }
        else
        {
            Map::iterator it = node->sons.find(param[from]);
            if(it == node->sons.end())
            {
                result = 0;
            }
            else  if(from == to)
            {
                result = it->second->count;
            }
            else
            {
                from++;
                const char* p = it->second->edgeStart + 1;
                bool match = true;
                while(p != it->second->edgeEnd + 1 && from <= to)
                {
                    if (*(param + from) != *p && *(param + from) != any) 
                    {
                        match = false;
                        break;
                    }
                    if (*p == terminator) 
                    {
                        match = false;
                        break;
                    }
                    ++from;
                    ++p;
                }
                if (match)
                {
                    if(from > to)
                    {
                        result += it->second->count;
                    }
                    else
                    {
                        result += similarCount(it->second, param, from, to, any);
                    }
                }
            }
        }
        return result;
    }
    int exactCount(const char* start, int len)
    {
        int i = 0;
        Node* node = root;
        if (len > maxDepth) return 0;
        while(i < len && node != NULL)
        {
            node = node->find(*(start + i));
            if (node != NULL)
            {
                const char* p = node->edgeStart;
                while(p != node->edgeEnd + 1 && i < len)
                {
                    if (*(start + i) != *p)
                    {
                        return 0;
                    }
                    ++i;
                    ++p;
                }
            }
        }
        if (node != NULL)
        {
            return node->count; 
        }
        return 0;
    }
    int calcCount()
    {
        return calcCount(root);
    }
    static int calcCount(Node* from)
    {
        if(from->isLeaf())
        {
            return from->count;
        }
        from->count = 0;
        Map::iterator it = from->sons.begin();
        for (;it != from->sons.end();++it)
        {
            from->count += calcCount(it->second);
        }
        return from->count;
    }
    static void dfsd(Node* c, int&  stacktrack, int& count)
    {
        if(c->isLeaf())
        {
            return;
        }
        Node* a;
        Map::iterator it = c->sons.begin();
        for (;it != c->sons.end();++it)
        {
            a = it->second;
            for(int i = 0; i < stacktrack; i++)
            {
                cout << "--";
            }
            if (a->edgeEnd - a->edgeStart < 10)
                cout << string(a->edgeStart,a->edgeEnd + 1) <<":" << a->count << endl;
            else
                cout << string(a->edgeStart,a->edgeStart + 10) <<":" << a->count << endl;
            stacktrack+=1;
            count++;
            dfsd(it->second, stacktrack, count);
            stacktrack-=1;
        }
    }
    bool exist(const char* start, int len)
    {
        int i = 0;
        Node* node = root;
        while(i < len && node != NULL)
        {
            node = node->find(*(start + i));
            if (node != NULL)
            {
                const char* p = node->edgeStart;
                while(p != node->edgeEnd + 1 && i < len)
                {
                    if (*(start + i) != *p)
                    {
                        cerr << "len:" << len << " offset:" << i << "-" << int(p - node->edgeStart)<< endl;
                        while(node != root)
                        {
                            node->printEdge(std::cerr);
                            cerr << endl;
                            node = node->parent;
                        }
                        for (int m = 0; m <len; ++m)
                        {
                            cerr << (int)(unsigned char)(*(start + m))<<",";
                        }
                        cerr << endl;
                        return false;
                    }
                    ++i;
                    ++p;
                }
            }
        }
        return i>=len;
    }
    bool self_test(const char* start, int len)
    {
        bool res = true;
        for(int i = 0; i < len; i++) // begin of substring
        {
            int m = min(maxDepth+1, len - i);
            for(int j = 1; j < m; j++) // end of substring
            {
                bool errid = exist(start + i, j);
                if (!errid)
                {
                    cerr << "Test Results: Fail in string ("; 
                    for (int m = 0; m <j; ++m)
                    {
                        cerr << (int)(unsigned char)(*(start + i + m))<<",";
                    }
                    cerr << ")" << endl;
                    res = false;
                }
            }
        }
        return res;
    }
    void dfsd()
    {
        int st = 0;
        int c = 0;
        cout << "r:" << root->count << endl;
        dfsd(root, st, c);
    }
};

#ifndef NO_LOCAL_TEST
#include "unittest.hpp"
using namespace unittest;
int test1()
{
    string s = "abacabc$";
    KSuffixTree t1(s.c_str(), s.size() ,2);
    UnitTest unittest(std::cerr, verbose);
    Node& root = *t1.root;
    ASSERT_EQUAL(t1.currentNode,10);
    ASSERT_EQUAL(root.children(),3);
    ASSERT_EQUAL(root['a'].children(),2);
    ASSERT_EQUAL(root['b'].children(),2);
    ASSERT_EQUAL(root['c'].children(),2);
    ASSERT_EQUAL(root['$'].children(),-1);
    ASSERT_EQUAL(root['a']['b'].children(),0);
    ASSERT_EQUAL(root['a']['c'].children(),0);
    ASSERT_FALSE(root['a']['c']['d'].valid());
    ASSERT_TRUE(root['b']['a'].isLeaf());
    ASSERT_TRUE(root['b']['c'].isLeaf());
    ASSERT_TRUE(root['c']['a'].isLeaf());
    ASSERT_TRUE(root['c']['$'].isLeaf());
    ASSERT_TRUE(root['a'].suffixLink == t1.root);
    ASSERT_TRUE(root['b'].suffixLink == t1.root);
    ASSERT_TRUE(root['c'].suffixLink == t1.root);
    return unittest.errors();
}
int test2()
{
    string s1("a$");
    string s2("aba$");
    string s3("abcd$");
    string s4("ef$");
    string s5("efef$");
    string s6("gg$");
    string s7("gggggg$");
    string s8("gggggg$");
    //requre input char* stoped with '\0'
    KSuffixTree t1(s1.c_str(), s1.size() ,3);
    t1.add(s2.c_str(), s2.size());
    t1.add(s3.c_str(), s3.size());
    t1.add(s4.c_str(), s4.size());
    t1.add(s5.c_str(), s5.size());
    t1.add(s6.c_str(), s6.size());
    t1.add(s7.c_str(), s7.size());
    t1.add(s8.c_str(), s8.size());
    UnitTest unittest(std::cerr, verbose);
    Node* root = t1.root;
    ASSERT_EQUAL(t1.calcCount(), 36 - 8);
    //t1.dfsd();
    ASSERT_TRUE(root != NULL);
    ASSERT_TRUE(t1.exist("a", 1));
    ASSERT_EQUAL(t1.exactCount("f", 1), 3);
    ASSERT_EQUAL(t1.exactCount("a", 1), 4);
    ASSERT_EQUAL(t1.exactCount("a$", 2), 2);
    ASSERT_EQUAL(t1.exactCount("b", 1), 2);
    ASSERT_EQUAL(t1.exactCount("c", 1), 1);
    ASSERT_EQUAL(t1.exactCount("ab", 2), 2);
    ASSERT_EQUAL(t1.exactCount("ba", 2), 1);
    ASSERT_EQUAL(t1.exactCount("bc", 2), 1);
    ASSERT_EQUAL(t1.exactCount("ac", 2), 0);
    ASSERT_EQUAL(t1.exactCount("abc", 3), 1);
    ASSERT_EQUAL(t1.exactCount("aba", 3), 1);
    ASSERT_EQUAL(t1.exactCount("g", 1), 14);
    return unittest.errors();
}
int test3()
{
    char base[] ={4,7,4,0};
    char str[] ={15,25,24,4,4,4,0};
    char str1[] ={24,4,4,4,8,25,9,4,26,4,9,16,4,6,26,0};
    char str2[] ={24,4,4,4,8,25,26,0};
    KSuffixTree t(base, 4 ,10, 0);
    t.add(str, 7);
    t.add(str1, 16);
    t.add(str2, 8);
    UnitTest unittest(std::cerr, verbose);
    Node& root = *t.root;
    ASSERT_TRUE(t.exist(str, 5));
    ASSERT_TRUE(t.exist(str, 6));
    ASSERT_TRUE(t.exist(str, 7));
    ASSERT_TRUE(t.exist(str2, 7));
    ASSERT_TRUE(t.exist(str2, 7));
    return unittest.errors();
}
int test4()
{
    HashTable<char, int> v(17);
    UnitTest unittest(std::cerr, verbose);
    v[9] = 1;
    HashTable<char, int>::iterator it = v.find(26);
    ASSERT_TRUE(it == v.end());
    it = v.find(9);
    ASSERT_EQUAL(it->second, 1);
}
int test5()
{
    string s1("abaaca$");
    string s2("aa$");
    string s3("aa$");
    KSuffixTree t1(s1.c_str(), s1.size() ,3);
    t1.add(s2.c_str(), s2.size());
    t1.add(s3.c_str(), s3.size());
    UnitTest unittest(std::cerr, verbose);
    Node* root = t1.root;
    ASSERT_EQUAL(t1.calcCount(), 13 - 3);
    return unittest.errors();
}
int main(int argn, char** args)
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
#endif
