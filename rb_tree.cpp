#include <stdio.h>  
#include <iostream>  
#include <stdlib.h>  
#include <cmath>  
#include <iostream>  
#include "rb_tree.h"

using namespace std;  

CRbTree::CRbTree()
{  
    nil = new Node();  
    head = nil;  
    nil->color = BLACK;  
}   

void CRbTree::dfs(Node *t,int d){  
    if(t==nil)  
    {  
        //cout<<d<<" ";//到叶子结点的路径上有几个黑色结点  
        return ;  
    }  
    cout<<" "<<t->key<<" "<<(t->color==BLACK?'B':'R')<<" "<<endl;  
    //cout<<"L";if(t->ch[0]!=nil&&t->ch[0]->p!=t) cout<<"bug"<<endl;  
    dfs(t->ch[0],d+(t->color==BLACK));  
    //cout<<"U";cout<<"R";if(t->ch[1]!=nil&&t->ch[1]->p!=t) cout<<"bug"<<endl;  
    dfs(t->ch[1],d+(t->color==BLACK));  
    //cout<<"U";  
}  
void CRbTree::out(){  
       dfs(head,0);cout<<endl;  
   }  

void CRbTree::rotate(Node *t,int c){  
    Node *&pt = (t->p==nil) ? head : t->p->ch[t->p->ch[1]==t];// t的父亲指向t的指针  
    Node *y = t->ch[c^1];  
    t->ch[c^1] = y->ch[c];  
    y->ch[c]->p = t;  
    y->ch[c] = t;  
    y->p = t->p;  
    t->p = y;  
    pt = y;  
}  
Node *CRbTree::successor(Node *t){//返回t的后继结点  
    Node *p;  
    if(t->ch[1]!=nil){  
        p = t->ch[1];  
        while(p->ch[0]!=nil) p = p->ch[0];  
        return p;  
    }else{  
        p = t->p;  
        while(p!=nil&&p->p->ch[1]==p) p = p->p;  
        return p;  
    }  
}  
void CRbTree::insert(int key){  
    Node *t = new Node();  
    t->key = key;  
    t->ch[0] = t->ch[1] = nil;  
    t->color = RED;  
    Node *x=head,*y=nil;  
    while(x!=nil)  //寻找叶子作为插入的地方  
    {  
        y = x;  
        x = x->ch[key > x->key];  
    }  
    t->p = y;  
    if(y==nil) head = t;  
    else y->ch[key > y->key] = t;  
    insert_fixup(t);  
}  
void CRbTree::insert_fixup(Node *t){  
    while(t->p->color==RED){  
        Node *p = t->p;//父亲R  
        Node *g = p->p;//祖父B  
        int pa = g->ch[1]==p;//父亲是左右孩子  
        int ta = p->ch[1]==t;//自己是左右孩子  
        Node *u = g->ch[pa^1];//叔叔  
        if(u->color==RED){//case1：叔叔也是红色，改变p,u,t的颜色  
            g->color = RED;  
            p->color = u->color = BLACK;  
            t = g;  
        }else{  
            if(ta!=pa)//case2：不在一条直线上  
            {  
                rotate(p,ta^1);//转成在同一条直线上  
                swap(t,p);  
                ta = pa;  
            }  
            g->color = RED;//case3：改变p，g的颜色  
            p->color = BLACK;  
            rotate(g,pa^1);//把祖父旋转  
        }  
    }  
    head->color = BLACK;  
}  
void CRbTree::remove(int key) 
{  
    Node *t= head,*y;  
    while(t!=nil&&t->key!=key) t = t->ch[key>t->key]; //查找被删除的结点  
    if(t==nil) return ;  
    if(t->ch[0]==nil||t->ch[1]==nil) y = t;//y是真正要删除的结点  
    else y = successor(t);  
    Node *x = y->ch[y->ch[0]==nil];  
    x->p = y->p;  
    if(y->p==nil)//y是根  
        head = x;  
    else  
        y->p->ch[y->p->ch[1]==y] = x; //把y从树中移除  
    if(y!=t)  
        t->key = y->key;  
    if(y->color==BLACK)//y是黑色则要进行红黑树的维护  
        remove_fixup(x);  
    delete(y);  
}  
void CRbTree::remove_fixup(Node *t)  
{  
    while(t!=head && t->color==BLACK){  
        Node *p = t->p;  
        int ta = p->ch[1]==t;  
        Node *w = p->ch[ta^1];//兄弟  
        if(w->color==RED)//case1：兄弟是红色，兄弟染成黑色，父亲染成红色，  
        {  
            w->color = BLACK;  
            p->color = RED;  
            rotate(p,ta);  
            w = p->ch[ta^1];  
        }  
        if(w->ch[0]->color==BLACK&&w->ch[1]->color==BLACK){ //case2：两个侄子是黑色：把兄弟染红，把缺一个黑色结点的状态推给父亲  
            w->color = RED;  
            t = p;  
        }else{  
            if(w->ch[ta^1]->color == BLACK){ //case3：有一个比较远的侄子是黑色更近的侄子是红色：把更近是侄子染黑，兄弟染红，兄弟旋转；  
                w->ch[ta]->color = BLACK;    //转完之后的状态是，兄弟黑，更远的侄子红。  
                w->color = RED;  
                rotate(w,ta^1);  
                w = p->ch[ta^1];  
            }  
            //case4：兄弟是黑色，较远的侄子是红色：兄弟染成父亲的颜色，父亲染黑，较远的侄子染黑，旋转父亲结点。  
            w->color = p->color;  
            p->color = BLACK;  
            w->ch[ta^1]->color = BLACK;  
            rotate(p,ta);  
            t = head;  
        }  
    }  
    t->color = BLACK;  
}  


