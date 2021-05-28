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
pair<string, int> tokens[] = {{"make", 0}, {"assign", 1}, {"arith", 2}, {"condition", 3}, {"comeout", 4}, {"while", 5}, {"list", 6}, {"input", 7}};
const int tokens_size = 8;

struct Command
{
    int codeNo;
    map<string, string> entityMap;

    void prepareCommand()
    {
        cin>>codeNo;

        string entity; cin>>entity;

        while(entity!="end")
        {
            string val; getline(cin, val);
            entityMap[entity] = val;

            cin>>entity;
        }

    }

    void clearCommand()
    {
        codeNo = -1;
        entityMap.clear();
    }

    Command()
    {
        clearCommand();
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

    bool isInteger(const string& s);
    int convertToInteger(const string& s);
    string convertToString(int n);

    void declareVariable();
    void assignValue();
    void performArithmeticOperation();
    bool isExpressionEmpty(const string& s);
    void createConditionalBlock();
    void comeOutOfBlock();
    void createWhileBlock();
    void createList();
    void takeInput();
    void performArithmeticOperation2();
    void giveOutput();

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

    void interpret();
    void showCppCode();
};

void AlgoInterpreter::interpret()
{
    //analyzer(s);
    command.prepareCommand();
    cppCodeGenerator();

    cout<<cppCodeBody;
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
/*
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
        else if(command.codeNo==6)
        {
            switch(tokenNo)
            {
                case 1: command.dataType = token;
                    break;
                case 2: command.variableName = token;
                    break;
                case 3: command.initialSize = convertToInteger(token);
                    break;
            }
        }
        else if(command.codeNo==7)
        {
            command.variableName = token;
        }
    }

}
*/

bool AlgoInterpreter::isInteger(const string& s)
{
    if(s.size()==0) return false;

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

int AlgoInterpreter::convertToInteger(const string& s)
{
    assert(isInteger(s));

    int n = s.size(), res = 0;
    for(int i=0; i<n; ++i)
    {
        res = res*10+(s[i]-'0');
    }

    return res;
}

string AlgoInterpreter::convertToString(int n)
{
    string s = "";

    bool isNeg = false;

    if(n<0)
    {
        isNeg = true;
        n = -n;
    }
    else if(n==0)
    {
        s="0";
    }
    else
    {
        while(n>0)
        {
            char c = (n%10)+'0';
            s = c+s;
            n/=10;
        }
    }

    if(isNeg) s = "-"+s;

    return s;

}

void AlgoInterpreter::declareVariable()
{
    string variableName = command.entityMap["variableName"];
    string dataType = command.entityMap["dataType"];
    string value = command.entityMap["value"];

    /*
    if(declaredVariables.count(variableName))
    {
        cout<<"Error : "<<variableName<<" varibale already declared"<<endl;

        //TODO: Handle error

        return;
    }
    */

    declaredVariables[variableName] = dataType;

    string& s = cppCodeBody;

    s += "\n" + dataType + " " + variableName;

    if(!value.empty())
    {
        if(dataType=="char")
        {
            value = "'"+value+"'";
        }

        s += " = " + value;
    }

    s += ";";
}

void AlgoInterpreter::assignValue()
{
    string variableName = command.entityMap["variableName"];
    string value = command.entityMap["value"];

    /*
    if(!declaredVariables.count(variableName))
    {
        cout<<"Error : "<<variableName<<" variable not declared yet"<<endl;

        //TODO: Handle error

        return;
    }
    */

    string& s = cppCodeBody;

    string& dataType = declaredVariables[variableName];

    if(dataType=="char")
    {
        value = "\'"+value+"\'";
    }

    s += "\n"+variableName+" = "+value+";";

}

void AlgoInterpreter::performArithmeticOperation()
{
    string operation = command.entityMap["operation"];
    string operand1 = command.entityMap["operand1"];
    string operand2 = command.entityMap["operand2"];
    string result = command.entityMap["result"];

    string var[3] = {operand1, operand2, result};

    bool flag = 0;

    /*
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
    */

    if(flag)
    {
        return;
    }

    string& s = cppCodeBody;

    s += "\n"+result+" = "+operand1+operation+operand2+";";
}

void AlgoInterpreter::performArithmeticOperation2()
{
    string operation = command.entityMap["operation"];
    string value = command.entityMap["value"];
    string variableName = command.entityMap["variableName"];

    string& s = cppCodeBody;

    s += "\n"+variableName+" "+operation+"= "+value+";";
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
    string conditionalKeyword = command.entityMap["conditionalKeyword"];
    string expression = command.entityMap["expression"];

    if(conditionalKeyword!="else" && conditionalKeyword!=" else")
    {
        if(isExpressionEmpty(expression))
        {
            cout<<"Error : Conditional expression cannot be empty"<<endl;
            return;
        }

        expression = "("+expression+")";
    }

    string& s = cppCodeBody;

    s += "\n"+conditionalKeyword+expression+"{\n";

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
    string expression = command.entityMap["expression"];

    if(isExpressionEmpty(expression))
    {
        cout<<"Error : Conditional expression cannot be empty"<<endl;
        return;
    }

    expression = "("+expression+")";

    string& s = cppCodeBody;

    s += "\nwhile"+expression+"{\n";

    ++nestedBlocksCounter;
}

void AlgoInterpreter::createList()
{
    string variableName = command.entityMap["variableName"];
    string dataType = command.entityMap["dataType"];
    string initialSize = command.entityMap["initialSize"];

    /*
    if(declaredVariables.count(variableName))
    {
        cout<<"Error : "<<variableName<<" varibale already declared"<<endl;

        //TODO: Handle error

        return;
    }
    */

    declaredVariables[variableName] = "vector<"+dataType+">";

    string& s = cppCodeBody;

    s += "\nvector<"+dataType+"> "+variableName+"("+initialSize+");";
}

void AlgoInterpreter::takeInput()
{
    string variableName = command.entityMap["variableName"];

    /*
    if(!declaredVariables.count(variableName))
    {
        cout<<"Error : "<<variableName<<" varibale not declared yet"<<endl;

        //TODO: Handle error

        return;
    }
    */

    string dataType = declaredVariables[variableName];

    string& s = cppCodeBody;

    if(dataType[0]=='v')// vector
    {
        s += "\nfor(int i=0; i<"+variableName+".size(); ++i){\n\tcin>>"+variableName+"[i];\n}";
    }
    else
    {
        s += "\ncin>>"+variableName+";";
    }
}

void AlgoInterpreter::giveOutput()
{
    string variableName = command.entityMap["variableName"];

    /*
    if(!declaredVariables.count(variableName))
    {
        cout<<"Error : "<<variableName<<" varibale not declared yet"<<endl;

        //TODO: Handle error

        return;
    }
    */

    string& s = cppCodeBody;

    s += "\n cout << "+variableName+";";
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
        case 6: createList();
            break;
        case 7: takeInput();
            break;
        case 8: performArithmeticOperation2();
            break;
        case 9: giveOutput();
            break;
        default: cout<<"Error : command codeNo : "<<command.codeNo<<" not available"<<endl;
    }

    command.clearCommand();
}



int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    AlgoInterpreter ai;

    ai.interpret();

    //filereader("ip.txt", "op.txt");
    /*
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
    */

    return 0;
}
