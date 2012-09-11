#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <vector>
#include <sstream>
#include <cassert>
using namespace std;
static const string TERMINATORS_RANGE = "$";
class Node
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
    void link(Node* son, int start, int end, const char* s)
    {
        // Links the current node with the son. The edge will have substring s[start, end)
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
class SuffixTree
{
    vector<Node*> nodes;
    Node *root, *needSuffix;
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
    SuffixTree(string& str)
    {
        termi = 0;
        currentNode = 0;
        str.append(TERMINATORS_RANGE);
        length = str.size();
        root = newNode();
        build(root, str);
    }
    SuffixTree(string stra[], int n)
    {
        termi = 0;
        currentNode = 0;
        root = newNode();
        stringstream sb(stringstream::in);
        for(int i = 0; i < n; i++)
        {
            sb << stra[i];
            sb << TERMINATORS_RANGE <<  i;
        }
        generalized = sb.str();
        length = generalized.size();
        build(root, generalized);
    }
    SuffixTree()
    {
        termi = 0;
        currentNode = 0;
        root = newNode();
    }
    void addString(string str)
    {
        str.append(TERMINATORS_RANGE);
        char buffer[128];
        sprintf(buffer, "%s", termi);
        str.append(buffer);
        termi++;
        length = str.size();
        build(root, str);
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
    //walk down from j to i, starts with the Node c
    Node* walkDown(Node* c, int j, int i, const char* str)
    {
        int k = j + c->depth();
        if(i - j + 1 > 0)
        {
            while(!c->inEdge(i - j))
            {
                c = c->sons[str[k]];
                assert(c != NULL);
                k += c->edgeLength();
            }
        }
        return c;
    }
    void addSuffixLink(Node* current)
    {
        if(needSuffix != NULL)
        {
            needSuffix->suffixLink = current;
        }
        needSuffix = NULL;
    }
    void build(Node* root, string& st)
    {
        build(root, st.c_str());
    }
    void build(Node* root, const char* s)
    {
        Node* c = newNode();
        needSuffix = NULL;
        root->link(c, 0, length, s);
        // Indicates if at the beginning of the phase we need to follow the suffix link of the current node
        //and then walk down the tree using the skip and count trick.
        bool needWalk = true;
        for(int i = 0, j = 1; i < length - 1; ++i)
        {
            char nc = s[i + 1];
            while(j <= i + 1)
            {
                if(needWalk)
                {
                    if(c->suffixLink == NULL && c->parent != NULL) c = c->parent;
                    c = (c->suffixLink == NULL ? root : c->suffixLink);
                    c = walkDown(c, j, i, s);
                }
                needWalk = true;
                // Here c == the highest node below s[j...i] and we will add char s[i+1]
                int m = i - j + 1; // Length of the string s[j..i].
                if(m == c->depth())
                {
                    // String s[j...i] ends exactly at node c (explicit node).
                    addSuffixLink(c);
                    map<char, Node*>::iterator it = c->sons.find(nc);
                    if(it != c->sons.end())
                    {
                        c = it->second;
                        needWalk = false;
                        break;
                    }
                    else
                    {
                        Node* leaf = newNode();
                        c->link(leaf, i + 1, length, s);
                    }
                }
                else
                {
                    // String s[j...i] ends at some place in the edge that reaches node c.
                    int where = c->edgeStart + m - c->parentDepth;
                    // The next character in the path after string s[j...i] is s[where]
                    if(s[where] == nc)   //Either rule 3 or rule 1
                    {
                        addSuffixLink(c);
                        if(!c->isLeaf() || j != c->edgeStart - c->parentDepth)
                        {
                            // Rule 3
                            needWalk = false;
                            break;
                        }
                    }
                    else
                    {
                        Node* split = newNode();
                        c->parent->link(split, c->edgeStart, where, s);
                        split->link(c, where, c->edgeEnd, s);
                        split->link(newNode(), i + 1, length, s);
                        addSuffixLink(split);
                        if(split->depth() == 1)
                        {
                            //The suffix link is the root because we remove the
                            //only character and end with an empty string.
                            split->suffixLink = root;
                        }
                        else
                        {
                            needSuffix = split;
                        }
                        c = split;
                    }
                }
                j++;
            }
        }
    }
};

int main(int argn, char** args)
{
    string s = "aaa";
    SuffixTree t1(s);
    cout << (t1.nofnodes()) << endl;
    return 0;
}
