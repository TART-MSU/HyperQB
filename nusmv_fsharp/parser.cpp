#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
using namespace std;

// test comment 



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

// void skip_comments (string line, ifstream myfile) {
//     if (line[0] == '-' && line[1] == '-') {
//         getline (myfile, line);
//         line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
//     }
// }

// set initial conditions
void init_state (set<string> &vars, map<string, string> &var_type, map<string, string> &init, string init_str) {
    
    string var_str;
    size_t first_paren = init_str.find("(");
    size_t second_paren = init_str.find(")", first_paren + 1);
    var_str = init_str.substr(first_paren, second_paren - first_paren);
    init[var_str] = "";
}

// store vars in maps and sets
void init_var (set<string> &vars, map<string, string> &var_type, map<string, string> &init, string init_str) {
    int i = 0;
    string var_str;
    while (init_str[i] != ':') {
        cout << init_str[i] << endl;
        var_str += init_str[i];
        i++;
    }
    // add var to set
    vars.insert(var_str);
    init_str.pop_back();

    // add var and var type to var_type map
    string type_info = init_str.substr(i+1);
    var_type[var_str] = type_info;
    cout << "var: " << type_info << endl;
}

void next_state () {

}

int main() {
    set<string> vars;
    map<string, string> var_type;
    map<string, string> init;
    map<string, string> next;
    map<string, string> define;

    string line, input_file;
    
    // cout << "Enter a file name: " << endl;
    // cin >> input_file;
    ifstream myfile ("smv.txt");
    
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
            
            if (line == "VAR") {
                getline (myfile, line);
                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                while (line != "ASSIGN") {
                    // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    cout << line << endl;
                    init_var(vars, var_type, init, line);
                    
                    getline(myfile, line);
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    // skip comments
                    while (line[0] == '-' && line[1] == '-') {
                        getline (myfile, line);
                        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    }
                }
            }
            if (line == "ASSIGN") {
                getline(myfile, line);
                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                // skip comments
                while (line[0] == '-' && line[1] == '-') {
                    getline (myfile, line);
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                }
                init_state(vars, var_type, init, line);
            }
            if (line.find("next(")) {
                
            }
        }

        myfile.close();
    }
    else cout << "Unable to open file";
    return 0;
}