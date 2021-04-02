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

// Just for testing Layer 3
pair<string, int> tokens[] = {{"make", 0}, {"assign", 1}, {"arith", 2}, {"condition", 3}, {"comeout", 4}, {"while", 5}};
const int tokens_size = 6;

struct Command
{
    int codeNo;
    string dataType;
    string variableName;
    string value;

    string operation;
    string operand1;
    string operand2;
    string result;

    string conditionalKeyword;
    string expression;

    void clearCommand()
    {
        codeNo = -1;
        dataType = "";
        variableName = "";
        value = "";

        operation = "";
        operand1 = "";
        operand2 = "";
        result = "";

        conditionalKeyword = "";
        expression = "";
    }
};

class AlgoInterpreter
{
    const string cppCodeHeader =
    "#include<bits/stdc++.h>\nusing namespace std;\nint main(){\n";

    const string cppCodeFooter =
    "\nreturn 0;\n}";

    string cppCodeBody = "";

    Command command;

    int nestedBlocksCounter;

    map<string, int> commandCodeMap;
    map<string, string> declaredVariables; // key=>variable name, value=>data type


    void analyzer(string& s);

    bool isInteger(string& s);
    void declareVariable();
    void assignValue();
    void performArithmeticOperation();
    bool isExpressionEmpty(const string& s);
    void createConditionalBlock();
    void comeOutOfBlock();
    void createWhileBlock();

    void cppCodeGenerator();

public:
    AlgoInterpreter()
    {
        for(int i=0; i<tokens_size; ++i)
        {
            commandCodeMap[tokens[i].first] = tokens[i].second;
        }

        command.codeNo = -1;
        nestedBlocksCounter = 0;
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
    string inBlock = "";

    for(int i=0; i<nestedBlocksCounter; ++i)
    {
        inBlock += "\n}";
    }

    string op = cppCodeHeader + cppCodeBody + inBlock + cppCodeFooter;
    cout<<op<<endl;
}

// This function is just to test whether the layer 3 is working properly or not when the tokenized command is received from layer 2, that is NLP layer
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
            if(!commandCodeMap.count(token))
            {
                cout<<"Error : Initial Command identifying token not found"<<endl;
                return;
            }

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
        else if(command.codeNo==2)
        {
            switch(tokenNo)
            {
                case 1: command.operation = token;
                    break;
                case 2: command.operand1 = token;
                    break;
                case 3: command.operand2 = token;
                    break;
                case 4: command.result = token;
                    break;
            }
        }
        else if(command.codeNo==3)
        {
            switch(tokenNo)
            {
                case 1: command.conditionalKeyword = token;
                    break;
                default: command.expression += " "+token;
            }
        }
        else if(command.codeNo==4)
        {
            // nothing required to be done here
        }
        else if(command.codeNo==5)
        {
            command.expression += " "+token;
        }
    }

}

bool AlgoInterpreter::isInteger(string& s)
{
    int n = s.size();
    for(int i=0; i<n; ++i)
    {
        if(s[i]<'0'||s[i]>'9')
        {
            if(i==0 && s[i]=='-')
            {
                continue;
            }

            return false;
        }
    }

    return true;
}

void AlgoInterpreter::declareVariable()
{
    if(declaredVariables.count(command.variableName))
    {
        cout<<"Error : "<<command.variableName<<" varibale already declared"<<endl;

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
        cout<<"Error : "<<command.variableName<<" variable not declared yet"<<endl;

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

void AlgoInterpreter::performArithmeticOperation()
{
    string var[3] = {command.operand1, command.operand2, command.result};

    bool flag = 0;

    for(int i=0; i<3; ++i)
    {
        if(!declaredVariables.count(var[i]))
        {
            //TODO : Handle error

            if(!isInteger(var[i]))
            {
                cout<<"Error : "<<var[i]<<" variable not declared yet"<<endl;
                flag = 1;
            }
        }
    }

    if(flag)
    {
        return;
    }

    string& s = cppCodeBody;

    s += "\n"+command.result+" = "+command.operand1+command.operation+command.operand2+";";
}

bool AlgoInterpreter::isExpressionEmpty(const string& str)
{
    int n = str.size();

    for(int i=0; i<n; ++i)
    {
        if(str[i]!=' ' && str[i]!='\n' && str[i]!='\t')
        {
            return false;
        }
    }

    return true;
}

void AlgoInterpreter::createConditionalBlock()
{
    if(command.conditionalKeyword!="else")
    {
        if(isExpressionEmpty(command.expression))
        {
            cout<<"Error : Conditional expression cannot be empty"<<endl;
            return;
        }

        command.expression = "("+command.expression+")";
    }

    string& s = cppCodeBody;

    s += "\n"+command.conditionalKeyword+command.expression+"{\n";

    ++nestedBlocksCounter;

}

void AlgoInterpreter::comeOutOfBlock()
{
    if(nestedBlocksCounter==0)
    {
        return;
    }

    cppCodeBody += "\n}";

    --nestedBlocksCounter;
}

void AlgoInterpreter::createWhileBlock()
{
    if(isExpressionEmpty(command.expression))
    {
        cout<<"Error : Conditional expression cannot be empty"<<endl;
        return;
    }

    command.expression = "("+command.expression+")";

    string& s = cppCodeBody;

    s += "\nwhile"+command.expression+"{\n";

    ++nestedBlocksCounter;
}

void AlgoInterpreter::cppCodeGenerator()
{
    switch(command.codeNo)
    {
        case 0: declareVariable();
            break;
        case 1: assignValue();
            break;
        case 2: performArithmeticOperation();
            break;
        case 3: createConditionalBlock();
            break;
        case 4: comeOutOfBlock();
            break;
        case 5: createWhileBlock();
            break;
        default: cout<<"Error : command codeNo : "<<command.codeNo<<" not available"<<endl;
    }

    command.clearCommand();
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
