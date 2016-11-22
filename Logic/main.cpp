//
//  main.cpp
//  Logic
//
//  Created by 洪锐 李 on 11/17/16.
//  Copyright © 2016 洪锐 李. All rights reserved.
//

#include "Header.h"
#include "String2Int.cpp"
#include "Operate.cpp"
using namespace std;

int main(int argc, const char * argv[]) {

    string2int toint = *new string2int();
    vector<Operate::Clause> query;
    string q, knowledge, num;
    int i = 0;
    ifstream iinfo;
    Operate::Clause known;
    Operate OP = *new Operate();
    
    //iinfo.open("/Users/kouruiri/Documents/Logic/Logic/TestCasesHW2/Test10/input.txt");
    iinfo.open("/Users/kouruiri/Documents/Logic/Logic/input.txt");
    //iinfo.open(argv[1]);
    //iinfo.open("/Users/kouruiri/Documents/Logic/Logic/TestCase_New/Test2/input.txt");
    
    
    if (! iinfo.is_open())
    { cout << "Error opening file"; exit (1); }
    
    
    
    //////////////////////////////////////////////////////////
    //           Read all the query  <Head>                 //
    //////////////////////////////////////////////////////////
    getline(iinfo, num);
    int n = toint.Toint(num);
    
    for(i = 0;i < n; i++)
    {
        getline(iinfo, q);
        query.push_back(OP.str2clause(q));
    }
    
    num.clear();
    
    //////////////////////////////////////////////////////////
    //            Read all the query                <End>   //
    //////////////////////////////////////////////////////////
    
    
    //////////////////////////////////////////////////////////
    //              Read the knowledge  <Head>              //
    //////////////////////////////////////////////////////////
    getline(iinfo, num);
    n = toint.Toint(num);
    
    for(i = 0;i < n; i++)
    {
        getline(iinfo, knowledge);
        OP.tell(knowledge);
    }
    iinfo.close();
    
    //////////////////////////////////////////////////////////
    //               Read the knowledge         <End>       //
    //////////////////////////////////////////////////////////
    ofstream oinfo("/Users/kouruiri/Documents/Logic/Logic/output.txt");
    for(i = 0; i < query.size(); i++){
        if (OP.ask(query[i])) {
            oinfo << "TRUE" << endl;
        }
        else{
            oinfo << "FALSE" << endl;
        }
    }
    oinfo.close();
    
    return 0;
    
    return 0;
}

