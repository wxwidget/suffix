#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <vector>
#include <sstream>
#include <cassert>
using namespace std;
class Node;
class SuffixTree;
class Suffix
{
public :
    Node* origin_node;
    int first_char_index;
    int last_char_index;
    Suffix(Node* node, int start, int stop)
        : origin_node(node),
          first_char_index(start),
          last_char_index(stop) {};
    int isExplicit()
    {
        return first_char_index > last_char_index;
    }
    int isImplicit()
    {
        return last_char_index >= first_char_index;
    }
    void canonize(const char*);
};
class Node// or an edge
{
public:
    Node *parent, *suffixLink;
    int edgeStart, edgeEnd, parentDepth;
    map<char, Node*> sons;
    Node()
    {
        parent = suffixLink = NULL;
        edgeStart = edgeEnd = parentDepth = 0;
    }
    bool inEdge(int position)
    {
        return parentDepth <= position && position < depth();
    }
    int edgeLength()
    {
        return edgeEnd - edgeStart;
    }
    int depth()
    {
        return parentDepth + edgeLength();
    }
    Node* find(char start)
    {
        map<char,Node*>::iterator it = sons.find(start);
        if (it != sons.end())
        {
            return it->second;
        }
        return NULL;
    }
    void link(Node* son, int start, int end, const char* s)
    {
        son->parent = this;
        son->parentDepth = depth();
        son->edgeStart = start;
        son->edgeEnd = end;
        sons[s[start]] = son;
    }
    bool isLeaf()
    {
        return sons.empty();
    }
};
void Suffix::canonize(const char* T)
{
    if(!isExplicit())
    {
        Node* node = origin_node->find(T[first_char_index]);
        int edge_span = node->edgeEnd - node->edgeStart;
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
    }
}
class SuffixTree
{
    vector<Node*> nodes;
    Node *root;
    int currentNode;
    int length;
    int termi;
    string generalized;
public:
    ~SuffixTree()
    {
        for(int i = 0; i <  currentNode; ++i)
        {
            delete nodes[i];
        }
    }
    SuffixTree(const string& str)
    {
        termi = 0;
        currentNode = 0;
        length = str.size();
        root = newNode();
        build(root, str.c_str());
    }
    SuffixTree()
    {
        termi = 0;
        currentNode = 0;
        root = newNode();
    }
    int nofnodes()
    {
        return currentNode;
    }
    Node* newNode()
    {
        Node* s = new Node();
        nodes.push_back(s);
        currentNode++;
        return s;
    }
    Node* splitEdge(Node* node, Suffix* s, const char* T)
    {
        int span = s->last_char_index - s->first_char_index;
        int where = node->edgeStart + span;
        Node* split = newNode();
        split->suffixLink = s->origin_node;
        node->parent->link(split, node->edgeStart, where, T);
        split->link(node, where + 1, node->edgeEnd, T);
        return split;
    }
    Node* splitEdge(Node* node, int where, const char* T)
    {
        Node* split = newNode();
        split->suffixLink = node->parent;
        node->parent->link(split, node->edgeStart, where, T);
        split->link(node, where + 1, node->edgeEnd, T);
        return split;
    }
    void build(Node* root, const char* str)
    {
        Suffix active(root, 0, -1);
        for(int i = 0; i < length; ++i)
            update(root, active, i, str);
    }
    void update(Node* root, Suffix& active, int last_char_index, const char* T)
    {
        Node* parent_node = NULL;
        Node* last_parent_node = NULL;
        while(active.first_char_index <= last_char_index)
        {
            Node* edge;
            parent_node = active.origin_node;
            //
            // Step 1 is to try and find a matching edge for the given node.
            // If a matching edge exists, we are done adding edges, so we break
            // out of this big loop.
            //
            if(active.isExplicit())
            {
                edge = active.origin_node->find(T[last_char_index]);
                if(edge != NULL)
                    break;
            }
            else //implicit node, a little more complicated
            {
                edge = active.origin_node->find(T[active.first_char_index]);
                int span = active.last_char_index - active.first_char_index;
                int where = edge->edgeStart + span;
                if(T[where + 1] == T[last_char_index])
                    break;
                //parent_node = splitEdge(edge, &active, T);
                parent_node = splitEdge(edge, where, T);
            }
            //
            // We didn't find a matching edge, so we create a new one, add
            // it to the tree at the parent node position, and insert it
            // into the hash table.  When we create a new node, it also
            // means we need to create a suffix link to the new node from
            // the last node we visited.
            parent_node->link(newNode(), last_char_index, length-1, T);
            if(last_parent_node != NULL)
                last_parent_node->suffixLink = parent_node;
            last_parent_node = parent_node;
            //
            // This final step is where we move to the next smaller suffix
            //
            if(active.origin_node == root)
                active.first_char_index++;
            else
                active.origin_node = active.origin_node->suffixLink;
            active.canonize(T);
        }
        if(last_parent_node)
            last_parent_node->suffixLink = parent_node;
        active.last_char_index++;  //Now the endpoint is the next active point
        active.canonize(T);
    }
    static void dfsd(Node* c, int&  stacktrack, int& count)
    {
        if(c->isLeaf())
        {
            return;
        }
        Node* a;
        map<char, Node*>::iterator it = c->sons.begin();
        for (;it != c->sons.end();++it)
        {
            a = it->second;
            for(int i = 0; i < stacktrack; i++)
            {
                cout << "--";
            }
            char c = it->first;
            cout << c << ":" << a->edgeStart << " to " <<  a->edgeEnd << endl;
            stacktrack+=1;
            count++;
            dfsd(it->second, stacktrack, count);
            stacktrack-=1;
        }
    }
    void dfsd()
    {
        int st = 0;
        int c = 0;
        dfsd(root, st, c);
    }
};
int main(int argn, char** args)
{
    string s = "abacabc$";
    SuffixTree t1(s);
    t1.dfsd();
    return 0;
}
