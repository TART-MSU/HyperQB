#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
using namespace std;

// need to fix, lines 159, 259

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

vector<vector<string> > cart_product (const vector<vector<string> >& v) {
    vector<vector<string>> s = {{}};
    for (const auto& u : v) {
        vector<vector<string>> r;
        for (const auto& x : s) {
            for (const auto& y : u) {
                r.push_back(x);
                r.back().push_back(y);
            }
        }
        s = move(r);
    }
    return s;
}

// create state map of initial conditions
vector<vector<string>> create_init_map (map<string, vector<string>> &init) {
    map<string, vector<string>> init_state_map;
    int state;

    vector<vector<string>> state_vec;

    for (auto x: init) {
        vector<string> possible_states;
        for (auto y: x.second) {
            if (y == "FALSE") {
                string var = "~" + x.first;
                possible_states.push_back(var);
            } else if (y == "TRUE") {
                string var = x.first;
                possible_states.push_back(var);
            }
        }

        state_vec.push_back(possible_states);
    }

    state_vec = cart_product(state_vec);

    return state_vec;

}


// set initial conditions
void init_state (set<string> &vars, map<string, string> &var_type, map<string, vector<string>> &init, string init_str) {
    vector<string> init_state_list;
    string var_str;
    size_t first_paren = init_str.find("(");
    size_t second_paren = init_str.find(")", first_paren + 1);
    var_str = init_str.substr(first_paren + 1, second_paren - (first_paren + 1));
    init_str.pop_back();
    // check for set of init states eg. {TRUE, FALSE}
    size_t found_bracket = init_str.find("{");
    if (found_bracket != string::npos) {
        string lst = init_str.substr(found_bracket+1);
        lst.pop_back();
        stringstream ss (lst); //create string stream from the string
        while(ss.good()) {
            string substring;
            getline(ss, substring, ','); //get first string delimited by comma
            init_state_list.push_back(substring);
        }
    } else {
        string init_cond = init_str.substr(second_paren + 3);
        init_state_list.push_back(init_cond);
    }
    
    // debugging
    // std::cout << "init_cond:  " << init_cond << endl;
    init[var_str] = init_state_list;
}

// store vars in maps and sets
void init_var (set<string> &vars, map<string, string> &var_type, map<string, vector<string>> &init, string init_str) {
    int i = 0;
    string var_str;
    while (init_str[i] != ':') {
        // debugging
        // std::cout << init_str[i] << endl;
        var_str += init_str[i];
        i++;
    }
    // add var to set
    
    vars.insert(var_str);
    init_str.pop_back();

    // add var and var type to var_type map
    string type_info = init_str.substr(i+1);
    var_type[var_str] = type_info;
    // debugging
    // std::cout << "var: " << type_info << endl;
}

// finding all possible next states, no matter init conditions
void next_state (set<string> &vars, map<string, vector<string>> &init, map<string, vector<string>> &next, string next_var, string line) {
    line.pop_back();
    // cout << "line: " << line << endl;
    vector<string> next_state_list;
    if (line.find("TRUE") == 0) {
        size_t found_colon = line.find(":");
        // debugging
        // std::cout << "next function:  "<< line.substr(found_colon + 1) << endl;
        // if state does not change

        // uncomment
        if (line.substr(found_colon + 1) == next_var) {
            // find state in init map
            // cout << "next_var: " << next_var << endl; 
            if (next_var[0] == '~') {
                next_state_list.push_back("FALSE");
            } else {
                next_state_list.push_back("TRUE");
            }
        }

        size_t found_bracket = line.find("{");

        if (found_bracket != string::npos) {
            string lst = line.substr(found_bracket+1);
            lst.pop_back();
            stringstream ss (lst); //create string stream from the string
            while(ss.good()) {
                string substring;
                getline(ss, substring, ','); //get first string delimited by comma
                next_state_list.push_back(substring);
            }
        }
    // conditional
    } else {
        size_t open_paren = line.find("(");
        size_t closed_paren = line.find(")");
        string condition = line.substr(open_paren + 1, closed_paren - (open_paren + 1));
        // cout << "condition: " << condition << endl;
        // cout << "next_var: " << next_var << endl;
        
        // fix later on, prob need a separate function / parser
        size_t found_colon = line.find(":");
        // debugging
        // std::cout << "next function:  "<< line.substr(found_colon + 1) << endl;
        // if state does not change

        // uncomment
        if (line.substr(found_colon + 1) == next_var) {
            // find state in init map
            // cout << "next_var: " << next_var << endl; 
            if (next_var[0] == '~') {
                next_state_list.push_back("FALSE");
            } else {
                next_state_list.push_back("TRUE");
            }
        }

        size_t found_bracket = line.find("{");

        if (found_bracket != string::npos) {
            string lst = line.substr(found_bracket+1);
            lst.pop_back();
            stringstream ss (lst); //create string stream from the string
            while(ss.good()) {
                string substring;
                getline(ss, substring, ','); //get first string delimited by comma
                next_state_list.push_back(substring);
            }
        } else {
            next_state_list.push_back(line.substr(found_colon + 1));
        }
        // if (condition[0] == '~') {
        //     condition == "FALSE";
        // } else {
        //     condition == "TRUE";
        // }

        // if (vars.find(condition) != vars.end()) {
        //     // next_state_list.push_back();
        // }
        next_var = condition;
    }
    // debugging
    // for (auto x: next_state_list) {
    //     cout << "list: " << x << endl;
    // }

    next[next_var] = next_state_list;
}

// finding the correct next states

stringstream transition_relation (map<string, string> var_type, vector<string> init_state, map<string, vector<string>> all_next) {
    stringstream ss;
    string init_str;
    ss << "[" ;

    map<string, vector<string>> next;
    // uncomment

    // for (auto x: init_state) {
    //     if (x.second == "FALSE") {
    //         init_str += "~" + x.first + "/\\";
    //     } else {
    //         init_str += x.first + "/\\";
    //     } 
    // }

    for (auto x: init_state) {
        init_str += x + "/\\";
        for (auto y: all_next) {
            // cout << "x: " << x << endl;
            // cout << "y: " << y.first << endl;
            if (y.first == x) {
                next[y.first] = y.second;
            }
        }
    }
    

    // for(const auto& elem : next)
    // {
    //     std::cout << elem.first << " ";
    //     for (auto x : elem.second) {
    //         std::cout << x << " ";
    //     }
    //     std::cout << "\n";
    // }

    ss << init_str.substr(0, init_str.length() - 2) << "] -> [" ;
    string next_str;
    vector<vector<string>> next_vec_pairs;
    // int combinations = 1;

    // clean this
    // trying to figure out how to find all possible pairs
    for (auto x: next) {
        vector<string> possible_states;
        for (auto y: x.second) {
            if (y == "FALSE") {
                string var = x.first + "'";
                possible_states.push_back(var);
            } else if (y == "TRUE") {
                string var = x.first + "'";
                possible_states.push_back(var);
            }
        }
        // combinations *= possible_states.size();
        next_vec_pairs.push_back(possible_states);
    }
    

    // cout << "combinations: " << combinations << endl;
    // for (auto x: next_vec_pairs) {
    //     for (auto y: x) {
    //         cout << "y: " << y << endl;
    //     }
    // }

    next_vec_pairs = cart_product(next_vec_pairs);

    for (int i =0; i < next_vec_pairs.size(); i++) {
        string formula;
        for (auto state: next_vec_pairs[i]) {
            formula += state + "/\\";
        }
        ss << "(" << formula.substr(0, formula.length()-2) << ")";
        if ((i+1) < next_vec_pairs.size()) {
            ss << "\\/";
        }
    }
    ss << "]";
    // for (auto x: next_vec_pairs) {
    //     cout << "________" << endl;
    //     for (auto y: x) {
    //         cout << y << endl;
    //     }
    // }

    // for (int i = 0; i < next_vec_pairs.size(); i++) {
    //     for (int j = 0; j < 2; j++) {
    //         string formula = next_vec_pairs[i][0] + "/\\" + next_vec_pairs[i][1];
    //         cout << formula << endl;
    //     }
    // }
    
    return ss;
}

int main() {
    set<string> vars;
    map<string, string> var_type;
    map<string, vector<string> > init;
    map<string, vector<string> > next;
    vector<vector<string> > all_init_states;
    //int state_num = 0;
    // map<string, string> define;

    string line, input_file;
    
    // std::cout << "Enter a file name: " << endl;
    // cin >> input_file;
    ifstream myfile ("smv_3.txt");
    
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
                    // debugging
                    // std::cout << line << endl;
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
                while (line.find("next(") == string::npos){
                    //skip comments and blank lines
                    if ((line[0] == '-' && line[1] == '-') || (line == "")) {
                        getline (myfile, line);
                        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                        continue;
                    }
                    // if (line == "") {
                    //     getline (myfile, line);
                    //     line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    //     continue;
                    // }
                    init_state(vars, var_type, init, line);
                    getline (myfile, line);
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                }
                all_init_states = create_init_map(init);
            }
            size_t found_next = line.find("next(");
            if (found_next != string::npos) {
                found_next += 5;
                char ch = line[found_next];
                string next_var;
                while (ch != ')') {
                    next_var += ch;
                    found_next += 1;
                    ch = line[found_next];
                }
                // debugging
                // std::cout << "next var chars: " << next_var << endl;
                getline (myfile, line);
                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                if (line == "case") {
                            
                    getline(myfile, line);
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    while (line.find("esac") == string::npos){

                        // skip over blank lines and comments
                        while (line == "" or line.find("--") != string::npos) {
                            getline(myfile, line);
                            line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                        }
                        // debugging
                        // cout << "next line: " << line << endl;
                        next_state(vars, init, next, next_var, line);
                                
                        getline(myfile, line);
                        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    
                    }
                }
            }
        }
        myfile.close();
    }


    else std::cout << "Unable to open file";
    
    // debugging

    // std::cout << "\nVAR TYPE MAP:" << endl;
    // for(const auto& elem : var_type)
    // {
    //     std::cout << elem.first << " " << elem.second << "\n";
    // }

    // std::cout << "\nINIT MAP:" << endl;
    // for(const auto& elem : init)
    // {
    //     std::cout << elem.first << " ";
    //     for (auto x : elem.second) {
    //         std::cout << x << " ";
    //     }
    //     std::cout << "\n";
    // }

    // std::cout << "\nNEXT MAP:" << endl;
    // for(const auto& elem : next)
    // {
    //     std::cout << elem.first << " ";
    //     for (auto x : elem.second) {
    //         std::cout << x << " ";
    //     }
    //     std::cout << "\n";
    // }

    for (auto state: all_init_states) {
        stringstream output = transition_relation(var_type, state, next);
        cout << "transition relation: " << output.str() << endl;
        // map<string, string> single_state_map;
        // for (auto var: state) {
            // if (var[0] == '~') {
            //     single_state_map[var.substr(1)] = "FALSE";
            // } else {
            //     single_state_map[var] = "TRUE";
            // }
            
        // }
        // stringstream output = transition_relation(var_type, single_state_map, next);
        // cout << "transition relation: " << output.str() << endl;
    }

    // debugging

    // vector<vector<string>> v = {{"a", "~a"}, {"b", "~b"}};
    // vector<vector<string>> prod = cart_product(v);

    // for (auto x: prod) {
    //     cout << "________" << endl;
    //     for (auto y: x) {
    //         cout << y << endl;
    //     }
    // }

    return 0;
}