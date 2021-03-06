//
//  Operate.cpp
//  Logic
//
//  Created by 洪锐 李 on 11/18/16.
//  Copyright © 2016 洪锐 李. All rights reserved.
//

#include <stdio.h>
#include "Header.h"

using namespace std;
class Operate{
public:
    struct Clause
    {
        string str, pred_name;			//the original string
        vector<string> arg;	//the argument list (2 at most in this case)
        bool is_fact;		//indicates whether the clause is a fact (no variables)
        bool is_true;
        Clause(){ is_fact = true; is_true = true;}
        ~Clause(){ arg.clear(); }
    };
    
    vector<string> decompose_clause(string str)
    {
        //return value 1: predicate's name
        //return value 2: argument 1 (if any)
        //return value 3: argument 2 (if any)
        //return value 4: (if any)
        int head = 0;
        char p = '(', c = ',';
        erasetuples(str);
        str = erasespace(str);
        size_t tag = str.find(p);
        if(tag == 0){
            head ++;
        }
        size_t loc = str.find(p, tag + 1);
        vector<string> ret;
        if (tag == string::npos)
        {
            ret.push_back(str);
        }
        else
        {
            ret.push_back(str.substr(head, tag));
            size_t loc2 = tag + 1;
            loc = str.find(c, loc2);
            if (loc == string::npos)//one argument
            {
                ret.push_back(str.substr(loc2, str.find(')', loc2) - loc2));
            }
            else
            {
                ret.push_back(str.substr(loc2, loc-loc2));
                ret.push_back(str.substr(loc + 1, str.find(')', loc2) - loc - 1));
            }
        }
        return ret;
    }
    void update_str(Clause &c)
    {
        c.str = c.pred_name + "(";
        for (int i = 0; i < c.arg.size(); i++)
        {
            c.str += c.arg[i];
            if (i < c.arg.size() - 1) c.str += ",";
        }
        c.str += ")";
    }
    
    void update_clause_str(string &str, string &new_var)
    {
        bool modified = false;
        Clause tmp = str2clause(str);
        for (int i = 0; i < tmp.arg.size(); i++)
        {
            if (is_variable(tmp.arg[i]))
            {
//                if (new_var == "")
//                {
                    if (!is_locked())
                    {
                        aug_idx();
                        lock_idx();
                    }
                    new_var = "[" + tmp.arg[i] + num2str(get_idx()) + "]";
//                }
                tmp.arg[i] = new_var;
                modified = true;
                tmp.is_fact = false;
            }
        }
        if (modified)
        {
            update_str(tmp);
            str = tmp.str;
        }
    }
    vector<multimap<string, Clause>::iterator> fetch_rules_for_goal(Clause goal)
    {
        vector<multimap<string, Clause>::iterator> ret;
        //multimap<string, Clause>::iterator;
        bool match;
        pair<multimap<string, Clause>::iterator, multimap<string, Clause>::iterator> bound = kb.equal_range(goal.pred_name);
        for (multimap<string, Clause>::iterator iter = bound.first;
             iter != bound.second;
             iter++){
            match = true;
            if (iter -> second.arg.size() == goal.arg.size() && iter -> second.is_true == goal.is_true)
            {
                for (unsigned i = 0; i < goal.arg.size(); i++)
                {
                    if (!is_variable(goal.arg[i]) && !is_variable(iter->second.arg[i]))
                    {
                        if (goal.arg[i] != iter->second.arg[i])
                        {
                            match = false;
                            break;
                        }
                        else if(goal.is_true != iter -> second.is_true){
                            match = false;
                            break;
                        }
                    }
                }
            }
            else
            {
                match = false;
            }
            if (match)
            {
                ret.push_back(iter);
            }
        }
        return ret;
    }
    
    bool unify(Clause &x, Clause &y, map<string, string> &theta)
    {
        map<string, string>::iterator iter;
        for (int i = 0; i < x.arg.size(); i++)
        {
            if (is_variable(x.arg[i]))
            {
                if (!is_variable(y.arg[i]))
                {
                    iter = theta.find(x.arg[i]);
                    if (iter == theta.end())
                        theta[x.arg[i]] = y.arg[i];
                    else
                    {
                        if (iter->second != y.arg[i])
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    if (x.arg[i] != y.arg[i])
                    {
                        x.arg[i] = y.arg[i];
                        update_str(x);
                    }
                }
            }
            else if (is_variable(y.arg[i]))
            {
                if (!is_variable(x.arg[i]))
                {
                    iter = theta.find(y.arg[i]);
                    if (iter == theta.end())
                        theta[y.arg[i]] = x.arg[i];
                    else
                    {
                        if (iter->second != x.arg[i])
                        {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
    
    vector<string> preprocess(string &str){
        erasetuples(str);
        str = erasespace(str);
        size_t i = str.find('|');
        size_t k;
        size_t j = str.find("=>");
        int imply = 0;
        int lock = 0;
        vector<int> ad;
        string premise, conclusion;
        string compre, t1;
        vector<string> pre, t;
        for(int count = 0; count < str.length(); count++){
            if(str[count] == '('){
                lock++;
            }
            else if(str[count] == ')'){
                lock--;
            }
            else if (str[count] == '&' && !lock) {
                ad.push_back(count);
            }
            else if(str[count] == '|' && !lock){
                imply = count;
            }
        }
        if(ad.size() > 0){
            int tmp = 0;
            for(int m = 0; m < ad.size(); m++){
                t1 = str.substr(tmp, ad[m] - tmp);
                t = preprocess(t1);
                tmp = ad[m] + 1;
                for(int x = 0; x < t.size(); x++){
                    pre.push_back(t[x]);
                }
                t.clear();
            }
            t1 = str.substr(tmp, str.length() - 1);
            t = preprocess(t1);
            for(int x = 0; x < t.size(); x++){
                pre.push_back(t[x]);
            }
        }
        else if(str.find("~") != string::npos && imply){
            str[str.find("~")] = ' ';
            str[imply] = '=';
            str.insert(str.begin() + imply + 1, '>');
            return preprocess(str);
            
        }
        else if(i != string::npos && j != string::npos){
            premise = erasespace(str.substr(0, j));
            conclusion = erasespace(str.substr(j + 2, str.length() - 1));
            
            erasetuples(premise);
            pre.push_back(premise.substr(0, i - 1) + "=>" + conclusion);
            while(premise.find("|", i)!=string::npos)
            {
                k = premise.find("|", i);
                compre = premise.substr(i + 1, k - i - 1) + "=>" + conclusion;
                pre.push_back(compre);
                
                i = k+SIZE("|");
            }
            pre.push_back(premise.substr(i,premise.length() - 1) + "=>" + conclusion);
        }
        else{
            pre.push_back(str);
        }
        
        
        return pre;
        
        
    }
    
    Clause subst(Clause x, map<string, string> &theta, bool &flag)
    {
        //temporary
        map<string, string>::iterator iter;
        Clause new_x = x;
        if (theta.size() == 0) return new_x;
        new_x.str = x.pred_name + "(";
        for (int i = 0; i < x.arg.size(); i++)
        {
            if (is_variable(x.arg[i]))
            {
                iter = theta.find(x.arg[i]);
                if (iter == theta.end())
                {
                    flag = false;
                    return x;
                }
                new_x.arg[i] = theta[x.arg[i]];
            }
            new_x.str += new_x.arg[i];
            if (i < x.arg.size() - 1) new_x.str += ",";
        }
        new_x.str += ")";
        flag = true;
        return new_x;
    }
    
    bool ask(Clause query, map<string, int> &checkcircle)
    {
        vector<multimap<string, Clause>::iterator> bvec;
        multimap<string, Clause>::iterator iter;
        stack<Clause> stk;
        map<string, string> the;
        stk.push(query);
        bool flag, second, result;
        second = false;
        int count = 0;
        vector<int> countset;
        result = false;
        
        while (!stk.empty())
        {
            
            flag = false;
            Clause cur = stk.top();
            cur = subst(cur, the, flag);
            
            if (checkcircle.count(cur.pred_name)) {
                stk.pop();
                continue;
            }

//            if (flag)
//            {
//                stk.pop();
//                continue;
//            }
            
            bvec = fetch_rules_for_goal(cur);
            //
//            if (bvec.size() == 0){
//                return false;
//            }
            //
            //stk.pop();
            for (int i = 0; i < bvec.size(); i++)
            {
                iter = bvec[i];
                if (!unify(cur, iter->second, the))
                {
                    return false;
                }
                
//                stk.push(cur);
                stk.pop();
                if (!iter->second.is_fact)
                {
                    count = 0;
                    map<string, vector<string> >::iterator iiter = prem.find(iter->second.str);
                    for (size_t j = 0; j < iiter->second.size(); j++)
                    {
                        Clause p = str2clause(iiter->second[j]);
                        //p = subst(p, the);
                        stk.push(p);
                        count ++;
                    }
                    countset.push_back(count);
                    
                }
                else if (!iter -> second.pred_name.compare(cur.pred_name)){
                    result = true;
                    if(stk.size() == 0){
                        return result;
                    }
                    
                    for(int i = 0; i < countset[countset.size() - 1]; i++){
                        result = result && ask(stk.top(), checkcircle);
                        stk.pop();
                    }
                    
                    if(stk.size() == 0 && result == true){
                        return result;
                    }
                }
  //              stk.push(cur);
            }
            //stk.pop();
            checkcircle[cur.pred_name] ++;
        }
//        checkcircle.clear();
        return result;
    }
    
    Clause str2clause(string str)
    {
        Clause clus;
        vector<string> arg_list = decompose_clause(str);
        //bool flag = true;
        clus.str = str;
        
        int flag = 0;
        while(arg_list[0][0] == '~'){
            arg_list[0].erase(arg_list[0].begin());
            flag++;
        }
        
        clus.pred_name = arg_list[0];
        
        if(flag % 2 != 0){
            clus.is_true = false;
        }
        
        for (int i = 1; i < arg_list.size(); i++)
        {
            //if (is_variable(arg_list[i])) flag = false;
            clus.arg.push_back(arg_list[i]);
        }
        //clus.is_fact = flag;
        return clus;
    }
    
    string erasespace(string str){
        while(str[0] == ' '){
            str.erase(str.begin());
        }
        while(str[str.length() - 1] == ' '){
            str.erase(--str.end());
        }
        
        return str;
    }
    
    string eraseneg(string str){
        while(str[0] == '~'){
            str.erase(str.begin());
        }
        
        return str;
    }
    
    void erasetuples(string &str){
        if(str[0] == '(' && str[str.length() - 1] == ')'){
            str.erase(str.begin());
            str.erase(--str.end());
        }
    }
    
    void tell(string str)
    {
        str = erasespace(str);
        erasetuples(str);
        size_t loc1 = str.find("=>");
        string premise = "", conclusion = "", clause = "";
        Clause clus;
        vector<string> arg_list;
        
        if(loc1!=string::npos)
        {
            premise = erasespace(str.substr(0, loc1));
            conclusion = erasespace(str.substr(loc1 + 2, str.size() - loc1 - 2));
        }
        else
        {
            conclusion = str;
        }
        
        clus = str2clause(conclusion);
        
        //st"&"ardize
        string new_variable;
        for (int i = 0; i < clus.arg.size(); i++)
        {
            if (is_variable(clus.arg[i]))
            { 
                if (!is_locked())
                {
                    aug_idx();
                    lock_idx();
                }
                
                new_variable = "[" + clus.arg[i] + num2str(get_idx()) + "]";
                clus.arg[i] = new_variable;
            }
        }
        update_str(clus);
        
        if(SIZE(premise)>0)
        {
            erasetuples(premise);
            loc1 = 0;
            clus.is_fact = false;
            size_t loc2 = string::npos;
            while(premise.find("&", loc1)!=string::npos)
            {
                loc2 = premise.find("&", loc1);
                clause = premise.substr(loc1, loc2 - loc1);
                //
                update_clause_str(clause, new_variable);
                //
                prem[clus.str].push_back(clause);
                loc1 = loc2+SIZE("&");
            }
            clause = premise.substr(loc1, premise.size() - loc1);
            //
            update_clause_str(clause, new_variable);
            //
            prem[clus.str].push_back(clause);
        }
        if (is_locked()) unlock_idx();
        kb.insert(pair<string, Clause>(clus.pred_name, clus));
    }
    
private:
    multimap<string, Clause> kb;	//knowledge base: the key is the predicate's name of concolusion of
    map<string, int> constant;
    map<string, vector<string> > prem;		//the key is original string (member str in Clause)
 

    int variable_idx = 0;
    bool lock = false;

    inline bool is_locked()
    {
        return lock;
    }
    
    inline int get_idx()
    {
        return variable_idx;
    }
    
    inline void aug_idx()
    {
        if (!is_locked()) variable_idx++;
    }
    
    inline void lock_idx()
    {
        lock = true;
    }
    
    inline void unlock_idx()
    {
        lock = false;
    }
    
    inline bool is_variable(const string str)
    {
        if (str.length() < 2) return true;
        if (str[0] == '[') return true;
        return false;
    }
    
    inline string num2str(int i)
    {
        string ret;
        while (i > 0)
        {
            int a = i % 10;
            ret = ret + (char)('0' + a);
            i /= 10;
        }
        return ret;
    }
    
    inline unsigned SIZE(const string str)
    {
        return str.size();
    }

    
};

