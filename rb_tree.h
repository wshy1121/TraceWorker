#ifndef __RB_TREE_H
#define __RB_TREE_H

#define RED 1  
#define BLACK 2 

struct Node{  
    Node(){  
        ch[0] = ch[1] = p = NULL;  
    }  
    Node *ch[2],*p;  
    int key,color;  
};

class Tree
{
public:
    Tree(){  
        nil = new Node();  
        head = nil;  
        nil->color = BLACK;  
    }     
    void insert(int key);
    void remove(int key);
    void out();
private:
    Node *successor(Node *t);  
    void rotate(Node *t,int c);///c=0 左旋 1 右旋  
    void insert_fixup(Node *t);
    void remove_fixup(Node *t);  
    void dfs(Node *t,int d);
private:    
    Node *head,*nil;//根结点，叶子结点  
};  

#endif

