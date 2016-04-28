#include <stdio.h>  
#include <iostream>  
#include <stdlib.h>  
#include <cmath>  
#include <iostream>  
#include "rb_tree.h"

using namespace std;  

void Tree::dfs(Node *t,int d){  
    if(t==nil)  
    {  
        //cout<<d<<" ";//��Ҷ�ӽ���·�����м�����ɫ���  
        return ;  
    }  
    cout<<" "<<t->key<<" "<<(t->color==BLACK?'B':'R')<<" "<<endl;  
    //cout<<"L";if(t->ch[0]!=nil&&t->ch[0]->p!=t) cout<<"bug"<<endl;  
    dfs(t->ch[0],d+(t->color==BLACK));  
    //cout<<"U";cout<<"R";if(t->ch[1]!=nil&&t->ch[1]->p!=t) cout<<"bug"<<endl;  
    dfs(t->ch[1],d+(t->color==BLACK));  
    //cout<<"U";  
}  
void Tree::out(){  
       dfs(head,0);cout<<endl;  
   }  

void Tree::rotate(Node *t,int c){  
    Node *&pt = (t->p==nil) ? head : t->p->ch[t->p->ch[1]==t];// t�ĸ���ָ��t��ָ��  
    Node *y = t->ch[c^1];  
    t->ch[c^1] = y->ch[c];  
    y->ch[c]->p = t;  
    y->ch[c] = t;  
    y->p = t->p;  
    t->p = y;  
    pt = y;  
}  
Node *Tree::successor(Node *t){//����t�ĺ�̽��  
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
void Tree::insert(int key){  
    Node *t = new Node();  
    t->key = key;  
    t->ch[0] = t->ch[1] = nil;  
    t->color = RED;  
    Node *x=head,*y=nil;  
    while(x!=nil)  //Ѱ��Ҷ����Ϊ����ĵط�  
    {  
        y = x;  
        x = x->ch[key > x->key];  
    }  
    t->p = y;  
    if(y==nil) head = t;  
    else y->ch[key > y->key] = t;  
    insert_fixup(t);  
}  
void Tree::insert_fixup(Node *t){  
    while(t->p->color==RED){  
        Node *p = t->p;//����R  
        Node *g = p->p;//�游B  
        int pa = g->ch[1]==p;//���������Һ���  
        int ta = p->ch[1]==t;//�Լ������Һ���  
        Node *u = g->ch[pa^1];//����  
        if(u->color==RED){//case1������Ҳ�Ǻ�ɫ���ı�p,u,t����ɫ  
            g->color = RED;  
            p->color = u->color = BLACK;  
            t = g;  
        }else{  
            if(ta!=pa)//case2������һ��ֱ����  
            {  
                rotate(p,ta^1);//ת����ͬһ��ֱ����  
                swap(t,p);  
                ta = pa;  
            }  
            g->color = RED;//case3���ı�p��g����ɫ  
            p->color = BLACK;  
            rotate(g,pa^1);//���游��ת  
        }  
    }  
    head->color = BLACK;  
}  
void Tree::remove(int key)  
{  
    Node *t= head,*y;  
    while(t!=nil&&t->key!=key) t = t->ch[key>t->key]; //���ұ�ɾ���Ľ��  
    if(t==nil) return ;  
    if(t->ch[0]==nil||t->ch[1]==nil) y = t;//y������Ҫɾ���Ľ��  
    else y = successor(t);  
    Node *x = y->ch[y->ch[0]==nil];  
    x->p = y->p;  
    if(y->p==nil)//y�Ǹ�  
        head = x;  
    else  
        y->p->ch[y->p->ch[1]==y] = x; //��y�������Ƴ�  
    if(y!=t)  
        t->key = y->key;  
    if(y->color==BLACK)//y�Ǻ�ɫ��Ҫ���к������ά��  
        remove_fixup(x);  
    delete(y);  
}  
void Tree::remove_fixup(Node *t)  
{  
    while(t!=head && t->color==BLACK){  
        Node *p = t->p;  
        int ta = p->ch[1]==t;  
        Node *w = p->ch[ta^1];//�ֵ�  
        if(w->color==RED)//case1���ֵ��Ǻ�ɫ���ֵ�Ⱦ�ɺ�ɫ������Ⱦ�ɺ�ɫ��  
        {  
            w->color = BLACK;  
            p->color = RED;  
            rotate(p,ta);  
            w = p->ch[ta^1];  
        }  
        if(w->ch[0]->color==BLACK&&w->ch[1]->color==BLACK){ //case2������ֶ���Ǻ�ɫ�����ֵ�Ⱦ�죬��ȱһ����ɫ����״̬�Ƹ�����  
            w->color = RED;  
            t = p;  
        }else{  
            if(w->ch[ta^1]->color == BLACK){ //case3����һ���Ƚ�Զ��ֶ���Ǻ�ɫ������ֶ���Ǻ�ɫ���Ѹ�����ֶ��Ⱦ�ڣ��ֵ�Ⱦ�죬�ֵ���ת��  
                w->ch[ta]->color = BLACK;    //ת��֮���״̬�ǣ��ֵܺڣ���Զ��ֶ�Ӻ졣  
                w->color = RED;  
                rotate(w,ta^1);  
                w = p->ch[ta^1];  
            }  
            //case4���ֵ��Ǻ�ɫ����Զ��ֶ���Ǻ�ɫ���ֵ�Ⱦ�ɸ��׵���ɫ������Ⱦ�ڣ���Զ��ֶ��Ⱦ�ڣ���ת���׽�㡣  
            w->color = p->color;  
            p->color = BLACK;  
            w->ch[ta^1]->color = BLACK;  
            rotate(p,ta);  
            t = head;  
        }  
    }  
    t->color = BLACK;  
}  


