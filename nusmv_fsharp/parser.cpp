#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
using namespace std;

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
    var_str = init_str.substr(first_paren + 1, second_paren - (first_paren + 1));
    init_str.pop_back();
    string init_cond = init_str.substr(second_paren + 3);
    // debugging
    // std::cout << "init_cond:  " << init_cond << endl;
    init[var_str] = init_cond;
}

// store vars in maps and sets
void init_var (set<string> &vars, map<string, string> &var_type, map<string, string> &init, string init_str) {
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

void next_state (set<string> &vars, map<string, string> &init, map<string, vector<string>> &next, string next_var, string line) {
    line.pop_back();
    vector<string> next_state_list;
    if (line.find("TRUE") == 0) {
        size_t found_colon = line.find(":");
        // debugging
        // std::cout << "next function:  "<< line.substr(found_colon + 1) << endl;
        // if state does not change
        if (line.substr(found_colon + 1) == next_var) {
            // find state in init map
            next_state_list.push_back(init[next_var]);
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
    }
    
    next[next_var] = next_state_list;
}

vector<vector<string>> cart_product (const vector<vector<string>>& v) {
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

stringstream transition_relation (map<string, string> var_type, map<string, string> init, map<string, vector<string>> next) {
    stringstream ss;
    string init_str;
    ss << "[" ;
    for (auto x: init) {
        if (x.second == "FALSE" ) {
            init_str += "~";
        }
        init_str += x.first + "/\\";
    }
    ss << init_str.substr(0, init_str.length() - 2) << "] -> [" ;
    string next_str;
    vector<vector<string>> next_vec_pairs;
    // int combinations = 1;

    // trying to figure out how to find all possible pairs
    for (auto x: next) {
        vector<string> possible_states;
        for (auto y: x.second) {
            if (y == "FALSE") {
                string var = "~" + x.first + "'";
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
    map<string, string> init;
    map<string, vector<string>> next;
    // map<string, string> define;

    string line, input_file;
    
    // std::cout << "Enter a file name: " << endl;
    // cin >> input_file;
    ifstream myfile ("smv_2.txt");
    
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
                    // skip over blank lines and comments
                    while (line == "" or line.find("--") != string::npos) {
                        getline(myfile, line);
                        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    }
                    // debugging
                    // cout << "next line: " << line << endl;
                    next_state(vars, init, next, next_var, line);
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
    //     std::cout << elem.first << " " << elem.second << "\n";
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

    stringstream output = transition_relation(var_type, init, next);
    cout << "transition relation: " << output.str() << endl;

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