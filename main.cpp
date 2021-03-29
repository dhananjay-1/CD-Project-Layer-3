#include <bits/stdc++.h>
using namespace std;

void filereader(char* ipFile, char* opFile) {
#ifndef ONLINE_JUDGE
  // for getting input from input.txt
  freopen(ipFile, "r", stdin);
  // for writing output to output.txt
  freopen(opFile, "w", stdout);
#endif
}

string tokens[] = {"make", "assign"};
const int tokens_size = 2;

struct Command
{
    int codeNo;
    string dataType;
    string variableName;
    string value;
};

class AlgoInterpreter
{
    const string cppCodeHeader =
    "#include<bits/stdc++.h>\nusing namespace std;\nint main(){\n";

    const string cppCodeFooter =
    "\nreturn 0;\n}";

    string cppCodeBody = "";

    Command command;

    map<string, int> commandCodeMap;
    map<string, string> declaredVariables; // key=>variable name, value=>data type


    void analyzer(string& s);

    void declareVariable();
    void assignValue();

    void cppCodeGenerator();

public:
    AlgoInterpreter()
    {
        for(int i=0; i<tokens_size; ++i)
        {
            commandCodeMap[tokens[i]] = i;
        }

        command.codeNo = -1;
    }

    void interpret(string& s);
    void showCppCode();
};

void AlgoInterpreter::interpret(string& s)
{
    analyzer(s);
    cppCodeGenerator();
}

void AlgoInterpreter::showCppCode()
{
    string op = cppCodeHeader + cppCodeBody + cppCodeFooter;
    cout<<op<<endl;
}

void AlgoInterpreter::analyzer(string& ipStr)
{
    int tokenNo = 0;

    int n = ipStr.size();

    for(int i=0; i<n; ++i, ++tokenNo)
    {
        string token = "";

        while(i<n && ipStr[i]!=' ')
        {
            token += ipStr[i];
            ++i;
        }

        if(tokenNo==0)
        {
            command.codeNo = commandCodeMap[token];
            continue;
        }

        if(command.codeNo==0)
        {
            switch(tokenNo)
            {
                case 1: command.dataType = token;
                    break;
                case 2: command.variableName = token;
                    break;
                case 3: command.value = token;
                    break;
            }
        }
        else if(command.codeNo==1)
        {
            switch(tokenNo)
            {
                case 1: command.variableName = token;
                    break;
                case 2: command.value = token;
                    break;
            }
        }
    }

}

void AlgoInterpreter::declareVariable()
{
    if(declaredVariables.count(command.variableName))
    {
        cout<<"Error : varibale already declared"<<endl;
        //TODO: Handle error

        return;
    }

    declaredVariables[command.variableName] = command.dataType;

    string& s = cppCodeBody;

    s += "\n" + command.dataType + " " + command.variableName;

    if(!command.value.empty())
    {
        if(command.dataType=="char")
        {
            command.value = "'"+command.value+"'";
        }

        s += " = " + command.value;
    }

    s += ";";
}

void AlgoInterpreter::assignValue()
{
    if(!declaredVariables.count(command.variableName))
    {
        cout<<"Error : variable not declared yet"<<endl;
        //TODO: Handle error

        return;
    }

    string& s = cppCodeBody;

    string& dataType = declaredVariables[command.variableName];

    if(dataType=="char")
    {
        command.value = "\'"+command.value+"\'";
    }

    s += "\n"+command.variableName+" = "+command.value+";";

}

void AlgoInterpreter::cppCodeGenerator()
{
    switch(command.codeNo)
    {
    case 0: declareVariable();
        break;
    case 1: assignValue();
    }

}



int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    //filereader("ip.txt", "op.txt");

    AlgoInterpreter ai;

    string s; getline(cin, s);
    while(s!="end")
    {
        if(s=="show")
        {
            ai.showCppCode();
            cout<<endl;

            getline(cin, s);
            continue;
        }

        if(!s.empty())
            ai.interpret(s);

        getline(cin, s);
    }

    return 0;
}
