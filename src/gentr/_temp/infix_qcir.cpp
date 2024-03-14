#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
using namespace std;



void write_quantifiers (vector<char> const &quantifier, map<string, int> const &var_map, ofstream &my_file) {
    int len = quantifier.size();
    char letter = 'A';
    stringstream ss;
    for (int i=0; i<len; i++) {
        if (quantifier.at(i)=='A') {
            ss << "forall(" ;
        } else {
            ss << "exists(" ;
        }
        string identifier = "_";
        identifier.push_back(letter);
        for (auto x : var_map ) {
            if (x.first.find(identifier) != string::npos){
                ss << x.second << ",";
            }

        }
        string string_ss = ss.str();
        my_file << string_ss.substr(0,string_ss.length()-1) << ")" << endl;
        ss.str(std::string());
        letter++;
    }
}

pair<int, int> find_last_vars(string formula) {
    int index1 = formula.find_last_of("(");
    int index2 = formula.substr(0,index1-1).find_last_of("(");
    pair<int, int> index_pair(index2, index1);
    return index_pair;
}

string write_to_file (ofstream &my_file, int &count, stack<string> &s, string &prefix, pair<int, int> &index_pair){
    stringstream ss;
    //my_file << to_string(count) << " = ";
    ss << to_string(count) << " = ";
    if (s.top() == "/\\") { // or operator
        //my_file << "or(";
        ss << "or(";
    } else if (s.top() == "\\/") { // and operator
        //my_file << "and(";
        ss << "and(";
    }
    index_pair = find_last_vars(prefix);

    //my_file << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    //<< "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
    ss << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    << "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
    prefix = prefix.substr(0,index_pair.first) + '(' + to_string(count) + ')';
    count++;

    return ss.str();
}

bool isOperator(string c)
{
    if (c == "/\\" || c == "\\/") {
        return true;
    }
    else {
        return false;
    }
}

bool isOperator2(string c)
{
    if (c == "~") {
        return true;
    } else {
        return false;
    }
}

bool isPar(string c) {
    if (c == ")" || c == "(") {
        return true;
    } else {
        return false;
    }
}

int precedence(string c)
{
    if (c == "\\/") // acts as /\ operator since its flipped
        return 3;
    else if (c == "/\\") //acts as \/ operator since its flipped
        return 2;
    else if (c == "~")
        return 1;
    else
        return -1;
}

void InfixToQCIR(stack<string> s, string infix, map<string,int> &var_map, vector<char> const & quantifier)
{
    int count = 1;
    pair<int, int> index_pair;
    ofstream my_file;
    stringstream ss;
    my_file.open("output1.qcir");
    string prefix;
    string variable;
    reverse(infix.begin(), infix.end());

    for (int i = 0; i < infix.length(); i++) {
        if (infix[i] == '(') {
            infix[i] = ')';
        }
        else if (infix[i] == ')') {
            infix[i] = '(';
        }
    }
    for (int i = 0; i < infix.length(); i++) {
        if (infix[i]=='~') {
            //account for negation
            prefix +=infix[i];
            size_t pos;
            while ((pos = prefix.find(")~")) != std::string::npos) {
                size_t pos2 = prefix.substr(0,pos).find_last_of("(");
                prefix.replace(pos, 2, ")");
                prefix.insert(pos2+1, "-");
            }
        // adding variable to prefix
        } else if (infix[i] != '/' && infix[i] != '\\' && infix[i] != '~' && !isPar(infix.substr(i,1))){
            // assign numbers to variables here
            variable += infix[i];
            // end of variable due to operator or parentheses
            if ((infix.length() >= (i+1) && (infix[i+1] == '/' || infix[i+1] == '\\' || infix[i+1] == '~' || isPar(infix.substr(i+1,1))))||(infix.length()-1) == i) {
                reverse(variable.begin(), variable.end());
                // check if variable already in map or not
                if (var_map.find(variable) == var_map.end()) {
                    prefix = prefix +  "(" + std::to_string(count) + ')';
                    var_map [variable] = count;
                    count++;
                } else {
                    prefix = prefix +  "(" + to_string(var_map[variable]) + ')';
                }
                variable = "";
            }
        }
        else if (infix[i] == '(') {
            string s1;
            s1 += infix[i];
            s.push(s1);
        }
        else if (infix[i] == ')') {
            while ((s.top() != "(") && (!s.empty())) {
                // assign number to formula and write to file
                ss << write_to_file (my_file, count, s, prefix, index_pair);
                s.pop();
            }
            if (s.top() == "(") {
                s.pop();
            }
        }
        else if (isOperator(infix.substr(i,2)) || isOperator2(infix.substr(i,1))) {
            int index1;
            if (isOperator(infix.substr(i,2))) {
                index1 = 2;
            } else {
                index1 = 1;
            }
            string prec = infix.substr(i,index1);
            if (s.empty()) {
                s.push(infix.substr(i,index1));
            }
            else {
                if (precedence(prec) > precedence(s.top())) {
                    s.push(prec);
                }
                else if (precedence(prec) == precedence(s.top())) {
                    s.push(prec);
                }
                else {
                    while ((!s.empty()) && (precedence(prec) < precedence(s.top()))) {
                        // assign number to formula and write to file
                        ss << write_to_file (my_file, count, s, prefix, index_pair);
                        s.pop();
                    }
                    s.push(prec);
                }
            }
        }
    }
    while (!s.empty()) {
        // last formula
        // assign number to formula and write to file
        ss << write_to_file (my_file, count, s, prefix, index_pair);

        s.pop();
    }
    // quantifiers
    write_quantifiers (quantifier, var_map, my_file);
    // output and variables
    my_file << "output" << prefix << '\n' << endl;
    // write formulas to file
    my_file << ss.str() << endl;
    // new map so variables can be sorted by number in qcir file
    map<int, string> new_map;
    for (auto x: var_map) {
        new_map[x.second] = x.first;
    }
    my_file << "# variables" << endl;
    for (auto x: new_map) {
        my_file << "#  " << x.first << " " << x.second << endl;
    }
    my_file.close();
}

int main() {
    map<string, int> var_map;
    stack<string> stack;
    string line, input_file;

    //change this to change the quantifiers
    vector<char> quantifier {'A','E'};
    // takes an input file
    cout << "Enter a file name: " << endl;
    cin >> input_file;
    ifstream myfile (input_file);
    stringstream infix;

    // removes any white space from file and puts into stringstream
    if (myfile.is_open())
    {
    while ( getline (myfile,line) )
    {
      line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
      infix << line;
    }
    myfile.close();
    }
    else cout << "Unable to open file";
    InfixToQCIR(stack, infix.str() , var_map, quantifier);
    cout << "\nConversion complete\n" << endl;
    return 0;
}
