#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
using namespace std;


// type SmvProgram = 
//     {
//         Vars : Set<String>
//         VarTypes : Map<String, VariableType>;
//         Init : Map<String, VariableValue>
//         Next : Map<String, Expression>
//         Define : Map<String, Expression>
//     }

// let keywords = 
//     [
//         "MODULE"
//         "ASSIGN"
//         "VAR"
//         "DEFINE"
//         "init"
//         "next"
//         "TRUE"
//         "FALSE"
//         "case"
//         "esac"
//     ]

// MODULE, VAR, ASSIGN, ":=" , next() , case , init() 

// MODULE main
//  VAR
//     a: boolean;
//     b: boolean;
//  ASSIGN
//     -- Initial Conditions
//     init(a):= FALSE;
//     init(b):= TRUE;

//     -- Transition Relations
//     next(a) :=
//       case
//         TRUE: a;
//       esac;
//     next(b) :=
//       case
//         TRUE: b;
//       esac;
// 
// 
// [~a /\ b] -> [~a' /\ b']
//    -----------------
//    init(a):= FALSE;
//    init(b):= TRUE;
//    -- Transition Relations
//    next(a) :=
//      case
//        TRUE: {TRUE, FALSE};
//      esac;
//    next(b) :=
//      case
//        TRUE: b;
//      esac;
// 
//   [~a /\ b] -> ( [a' /\ b']   \/ [~a' /\ b'] )

// if var, add to var set
// add var to vartype map
// add to init map
// 

// struct SMVProgram {
//     set<string> vars;
//     map<string, string> var_type;
//     map<string, string> init;
//     map<string, string> next;
//     map<string, string> define;
// }

void initialize(set<string> &vars, map<string, string> &var_type, map<string, string> &init, string init_str) {
    int i = 0;
    string var_str;
    while (init_str[i] != ':') {
        cout << init_str[i] << endl;
        var_str += init_str[i];
        i++;
    }
    cout << "var: " << var_str << endl;
}


int main() {
    set<string> vars;
    map<string, string> var_type;
    map<string, string> init;
    map<string, string> next;
    map<string, string> define;

    string line, input_file;
    
    cout << "Enter a file name: " << endl;
    cin >> input_file;
    ifstream myfile (input_file);
    stringstream infix;
    
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
            if (line == "VAR") {
                getline (myfile, line);
                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                while (line != "ASSIGN") {
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    cout << line << endl;
                    initialize(vars, var_type, init, line);
                    
                    getline(myfile, line);
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    
                }
            }
            if (line == "ASSIGN") {
                getline (myfile, line);
                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                while (line != "DEFINE") {
                    if (line.substr(0,3) == "init") {
                        string var_init = "";
                        for (int i=5; line[i] != ')'; i++) {
                            var_init+= line[i];
                        }
                        de[var_init] =
                        
                    }
                }

            }
        }

        myfile.close();
    }
    else cout << "Unable to open file";
    return 0;
}