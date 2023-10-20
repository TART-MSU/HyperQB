#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
using namespace std;


vector<string> to_binary(int n, string var, int max)
{
    int bit_order = 0;
    int max_bit_order = 0;
    vector<string> bit_vector;
    string bit,r,max_bit,max_r;
    while(n!=0) {
        r=(n%2==0 ?"0":"1");
        if (r == "0") {
            bit_vector.push_back("~" + var + "_" + to_string(bit_order));
        } else {
            bit_vector.push_back(var + "_" + to_string(bit_order));
        }
        bit=(n%2==0 ?"0":"1")+bit; 
        n/=2;
        bit_order++;
    }

    while(max!=0) {
        max_r = (max%2==0 ?"0":"1");
        max_bit=(max%2==0 ?"0":"1") + max_bit; 
        max/=2;
        max_bit_order++;
    }

    while (max_bit_order != bit_order) {
       
        bit_vector.push_back("~" + var + "_" + to_string(bit_order));
        bit_order++;
    }

    reverse(bit_vector.begin(), bit_vector.end());
    // for (auto x: bit_vector){
    //     // cout << "bit:    " << x <<endl;
    // }
    return bit_vector;
}

// ********* handle conditions ***************************************

vector<pair<string,string> > condition_tokenizer (string condition) {
    int i = 0;
    vector<pair<string,string> > condition_vec;
    while (i < (condition.length()) ) {
        if (condition.substr(i, 2) == "!=") {
            condition_vec.push_back({"op", "!="});
            i+=2;
        } else if (condition.substr(i, 2) == ">=") {
            condition_vec.push_back({"op", ">="});
            i+=2;
        } else if (condition.substr(i, 2) == "<=") {
            condition_vec.push_back({"op", "<="});
            i+=2;
        } else if (condition[i] == '<') {
            condition_vec.push_back({"op", "<"});
            i+=1;
        } else if (condition[i] == '(') {
            condition_vec.push_back({"op", "("});
            i+=1;
        } else if (condition[i] == ')') {
            condition_vec.push_back({"op", ")"});
            i+=1;
        } else if (condition[i] == '>') {
            condition_vec.push_back({"op", ">"});
            i+=1;
        } else if (condition[i] == '|') {
            condition_vec.push_back({"op", "|"});
            i+=1;
        } else if (condition[i] == '&') {
            condition_vec.push_back({"op","&"});
            i+=1;
        } else if (condition[i] == ' '){
            i+=1;
        } else if (condition[i] == '!'){
            i+=1;
        } else if (condition[i] == '=') {
            condition_vec.push_back({"op", "="});
            i+=1;
        } else {
            string var;
            int j = i;
            while (condition[j] != '&'  && condition[j] != '|' && condition[j]!= ' ' && j < condition.length() && condition[j] != '=' 
                && condition[j] != ')' && condition[j] != ')' && condition.substr(j, 2) != "!=" && condition.substr(j, 2) != "<="
                && condition.substr(j, 2) != ">=" && condition[j] != '>' && condition[j] != '<' && condition[j] != '(') 
            {
                var += condition[j];
                j+=1;
            }
            i += (j-i);
            condition_vec.push_back({"var",var});
        }
    }
    return condition_vec;
}

// Function to find precedence of operators.
int precedence(string op){
    if(op == "|")
        return 1;
    if(op == "&")
        return 2;
    if (op == "=" || op == ">=" || op == "<=" || op == "<" || op == ">" || op == "!=") {
        return 3;
    }
    return 0;
}
 
// Function to perform arithmetic operations.
string applyOp(string a, string b, string op, map<string, string> &var_type){
    string a_val;
    string b_val;
    // bool a_neg = false;
    // bool b_neg = false;

    if (a.substr(0,1) == "!") {
        // a_neg = true;
        a_val = "~" + a.substr(1);
    }

    if (b.substr(0,1) == "!") {
        // b_neg = true;
        b_val = "~" + b.substr(1);
        
    }
    // cout << "A: " << a << "  " << a_neg << endl;
    // cout << "B: " << b << "  " << b_neg << endl;
    
    // find initial vals
    // for (auto var: init_state) {
    //     if (a == var.first) {
    //         a_val = var.second;
    //     }
    //     if (b == var.first) {
    //         b_val = var.second;
    //     }
    // }

    // if (define_map.find(a) != define_map.end() ) {
    //     a_val = define_map[a];
    // }

    // if (define_map.find(b) != define_map.end() ) {
    //     b_val = define_map[b];
    // }
    // negation

    // if (a_neg) {
    //     if (a_val == "TRUE") {
    //         a_val = "FALSE";
    //     } else if (a_val == "FALSE") {
    //         a_val = "TRUE";
    //     }
    // }
    // cout << "B VAR: " << b << "  " << b_val << endl;
    // cout << "B_NEG: " << b_neg << endl;
    // if (b_neg) {
    //     if (b_val == "TRUE") {
    //         b_val = "FALSE";
    //     } else if (b_val == "FALSE") {
    //         b_val = "TRUE";
    //     }
    // }


    // cout << "B VAR AFTER: " << b << "  " << b_val << endl;

    if (a_val == "") {
        a_val = a;
    }

    if (b_val == "") {
        b_val = b;
    }

    // cout << "A_VAL: " << a_val << endl;
    // cout << "B_VAL: " << b_val << endl;

    if (op == "&") {
        return "(" + a_val + " /\\ " + b_val + ")";
    } else if (op == "|") {
        return "(" + a_val + " \\/ " + b_val + ")";
    
    
    } else if (op == "="){
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        
        max_num = max_num.substr(found_char + 2);
        vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
        string blasted_var;
        for (const auto& bit: bits) {
            blasted_var += bit + " /\\ ";
        }
        return "(" + blasted_var.substr(0, blasted_var.length()-4) + ")";
    } else if (op == "!=") {
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        max_num = max_num.substr(found_char + 2);
        vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
        string blasted_var;
        for (const auto& bit: bits) {
            blasted_var += bit + " /\\ ";
        }
        return "~(" + blasted_var.substr(0, blasted_var.length()-4) + ")";
    } else if (op == ">=") {
        string all_vars;
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        int min_num = stoi(max_num.substr(0,max_num.length()-(max_num.length()-found_char)));
        
        max_num = max_num.substr(found_char + 2);
        for (int i=min_num; i <= stoi(b_val); i++) {
            vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + " /\\ ";
            }
           all_vars += "(" + blasted_var.substr(0, blasted_var.length()-4) + ") /\\ ";
        }
        return "~(" + all_vars.substr(0,all_vars.length()-4) + ")";

    } else if (op == "<=") {
        string all_vars;
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        int min_num = stoi(max_num.substr(0,max_num.length()-(max_num.length()-found_char)));
        
        max_num = max_num.substr(found_char + 2);
        for (int i=min_num; i <= stoi(b_val); i++) {
            // cout << "I: " << i << endl;
            vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + " /\\ ";
            }
           all_vars += "(" + blasted_var.substr(0, blasted_var.length()-4) + ") /\\ ";
        }
        return "(" + all_vars.substr(0,all_vars.length()-4) + ")";
    } else if (op == ">") {
        string all_vars;
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        int min_num = stoi(max_num.substr(0,max_num.length()-(max_num.length()-found_char)));
        
        max_num = max_num.substr(found_char + 2);
        for (int i=min_num; i < stoi(b_val); i++) {
            // cout << "I: " << i << endl;
            vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + " /\\ ";
            }
           all_vars += "~(" + blasted_var.substr(0, blasted_var.length()-4) + ") /\\ ";
        }
        return "(" + all_vars.substr(0,all_vars.length()-4) + ")";
    } else if (op == "<") {
        string all_vars;
        string max_num = var_type[a_val];
        
        size_t found_char = max_num.find("..");
        int min_num = stoi(max_num.substr(0,max_num.length()-(max_num.length()-found_char)));
        max_num = max_num.substr(found_char + 2);
        for (int i=min_num; i < stoi(b_val); i++) {
            // cout << "I: " << i << endl;
            vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + " /\\ ";
            }
           all_vars += "(" + blasted_var.substr(0, blasted_var.length()-4) + ") /\\ ";
        }
        return "(" + all_vars.substr(0,all_vars.length()-4) + ")";
    } 

    return "FALSE";
    
}
 
// Function that returns value of expression after evaluation.
string evaluate(vector<pair<string,string>> tokens, map<string,string> &var_type){
    int i;
    // cout << tokens.size() << endl;
    bool is_neg = false;
    if (tokens.size() == 1) { 
        return tokens[0].second;

        string var_val;
        string variable = tokens[0].second;

        // cout << "VAR: " << variable << endl;

        if (variable == "TRUE") {
            return "TRUE";
        }
        
        if (variable.substr(0,1) == "!") {
            is_neg = true;
            variable = variable.substr(1);
        }
        
        // for (auto var: init_state) {
        //     if (variable == var.first) {
        //         var_val = var.second;
        //         cout << "VAR_VAL: " << var_val;
        //     }
        // }

        // if (define_map.find(variable) != define_map.end() ) {
        //     var_val = define_map[variable];
        // }

        if (is_neg) {
            if (var_val == "FALSE") {
                return "TRUE";
            } else if (var_val == "TRUE") {
                return "FALSE";
            }
        } 
        return var_val; 
    }

    // stack to store vars and vals
    stack <string> values;
     
    // stack to store operators.
    stack <string> ops;
     
    for(i = 0; i < tokens.size(); i++){
        

        // Current token is an opening
        // brace, push it to 'ops'
        if(tokens[i].second == "("){
            ops.push("(");
        }
        
        // else if (tokens[i].second == "!") {
        //     string val1 = values.top();
        //     values.pop();

        //     string val2 = "";

        //     string op = "!";

        //     cout << "EVAL: "  << op << val1 << endl;
        //     values.push(applyOp(val1, val2, op, init_state));

        // }
        // Current token is a number, push
        // it to stack for numbers.
        else if(tokens[i].first == "var"){
            
            values.push(tokens[i].second);
         
        // Closing brace encountered, solve
        // entire brace.
        } else if(tokens[i].second == ")")
        {
            while(!ops.empty() && ops.top() != "(")
            {
                string val2 = values.top();
                values.pop();
                 
                string val1 = values.top();
                values.pop();
                 
                string op = ops.top();
                ops.pop();
                 
                // cout << "EVAL: " << val1 << op << val2 << endl;
                values.push(applyOp(val1, val2, op, var_type));
            }
             
            // pop opening brace.
            if(!ops.empty())
               ops.pop();
        }
         
        // Current token is an operator.
        else
        {
            // While top of 'ops' has same or greater
            // precedence to current token, which
            // is an operator. Apply operator on top
            // of 'ops' to top two elements in values stack.
            while(!ops.empty() && precedence(ops.top())
                                >= precedence(tokens[i].second)){
                string val2 = values.top();
                values.pop();
                 
                string val1 = values.top();
                values.pop();
                 
                string op = ops.top();
                ops.pop();
                 
                // cout << "EVAL: " << val1 << op << val2 << endl;
                values.push(applyOp(val1, val2, op, var_type));
            }
             
            // Push current token to 'ops'.
            ops.push(tokens[i].second);
        }
    }
     
    // Entire expression has been parsed at this
    // point, apply remaining ops to remaining
    // values.
    while(!ops.empty()){
        string val2 = values.top();
        values.pop();
                 
        string val1 = values.top();
        values.pop();
                 
        string op = ops.top();
        ops.pop();

        // cout << "EVAL: " << val1 << op << val2 << endl;   
        values.push(applyOp(val1, val2, op, var_type));
    }
     
    // Top of 'values' contains result, return it.
    return values.top();
}


// ********* handle conditions done ***************************************


vector<vector<pair<string,string>> > cart_product (const vector<vector<pair<string,string>> >& v) {
    vector<vector<pair<string,string>>> s = {{}};
    for (const auto& u : v) {
        vector<vector<pair<string,string>>> r;
        for (const auto& x : s) {
            for (const auto& y : u) {
                r.push_back(x);
                r.back().push_back(y);
            }
        }
        s = std::move(r);
    }
    return s;
}

vector<vector<string> > cart_product_2 (const vector<vector<string> >& v) {
    vector<vector<string>> s = {{}};
    for (const auto& u : v) {
        vector<vector<string>> r;
        for (const auto& x : s) {
            for (const auto& y : u) {
                r.push_back(x);
                r.back().push_back(y);
            }
        }
        s = std::move(r);
    }
    return s;
}

// create state map of initial conditions
vector<vector<pair<string,string>>> create_init_map (map<string, vector<string>> &init) {
    map<string, vector<string>> init_state_map;

    vector<vector<pair<string,string>>> state_vec;

    for (auto x: init) {
        vector<pair<string,string>> possible_states;
        for (auto y: x.second) {
            if (y == "FALSE") {
                string var = x.first;
                possible_states.push_back({var, "FALSE"});
            } else if (y == "TRUE") {
                string var = x.first;
                possible_states.push_back({var, "TRUE"});
            } else {
                possible_states.push_back({x.first, y});
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
void next_state (set<string> &vars, map<string, vector<string> > &init, map<string, vector<vector<string> > > &next, string next_var, string line) {
    line.pop_back();
    // cout << "next var: " << next_var << endl;
    // cout << "line: " << line << endl;
    string condition;
    vector<string> next_state_list;
    if (line.find("TRUE") == 0) {
        condition = "TRUE";
        size_t found_colon = line.find(":");
        // debugging
        // std::cout << "next function:  "<< line.substr(found_colon + 1) << endl;
        // if state does not change

        // uncomment
        // if (line.substr(found_colon + 1) == next_var) {
        //     // find state in init map
        //     // cout << "next_var: " << next_var << endl; 
            
        //     if (next_var[0] == '!') {
        //         next_state_list.push_back("FALSE");
        //     } else {
        //         next_state_list.push_back("TRUE");
        //     }
        // }

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
            next_state_list.push_back(line.substr(found_colon+1));
        }
    // conditional
    } else {
        size_t open_paren = line.find("(");
        size_t closed_paren = line.find(")");
        condition = line.substr(open_paren + 1, closed_paren - (open_paren + 1));
        // cout << "condition: " << condition << endl;
        // cout << "next_var: " << next_var << endl;

        // if (condition[0] == '!') {
        //     condition = "~" + condition.substr(1);
        // }
        // fix later on, prob need a separate function / parser
        size_t found_colon = line.find(":");
        // debugging
        // std::cout << "next function:  "<< line.substr(found_colon + 1) << endl;
        // if state does not change

        // uncomment
        // if (line.substr(found_colon + 1) == next_var) {
        //     // find state in init map
        //     // cout << "next_var: " << next_var << endl; 
        //     if (next_var[0] == '!') {
        //         next_state_list.push_back("FALSE");
        //     } else {
        //         next_state_list.push_back("TRUE");
        //     }
        // }

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


        // next_var = condition;
    }
    // debugging
    // for (auto x: next_state_list) {
    //     cout << "list: " << x << endl;
    // }
    // insert condition at beginning of next_state_list
    // cout << "conditionnnnnnn  " << condition << endl;
    next_state_list.push_back(condition);

    reverse(next_state_list.begin(), next_state_list.end());
    for (auto x: next_state_list) {
        // cout << "BRO; " << x << endl;
    }
    next[next_var].push_back(next_state_list);
}


string prime_expr(string expr){
    string rt;
    string var;
    for(std::string::size_type i = 0; i < expr.size(); ++i){
        if(expr[i] == ' '){
            continue;
        }
        else if((expr[i] == '/' && expr[i+1] == '\\') || 
                (expr[i] == '\\' && expr[i+1] == '/') ||
                (expr[i] == ')')){
            var.push_back('\'');
            var.push_back(expr[i]);
            rt += var;
            var.clear();
        }
        else{
            var.push_back(expr[i]);
            if (i == expr.size()-1){
                var.push_back('\'');
                rt += var;
            }
        }
    }
    // cout << rt << endl;
    return rt;
}

bool is_digit(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void debug_out(string s){
    cout << s << endl;
}

void cleanup(string &line){
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
    if (line.find("--")!=string::npos) {
                line = line.substr(0, line.find("--"));
                std::cout << "line: " <<line << endl;
    }
}

bool is_command(string line){
    return line[0] == '-' && line[1] == '-';
}


int main() {
    set<string> vars;
    map<string, string> var_type;
    map<string, vector<string> > init;
    map<string, vector<vector<string> > > next;
    // map<string, vector<pair<string, vector<string> > > > next;
    vector<vector<pair<string,string> > > all_init_states;
    map<string, string> define_map; // variable : condition
    //int state_num = 0;
    // map<string, string> define;

    string line, input_file;
    
    // std::cout << "Enter a file name: " << endl;
    // cin >> input_file;
    // ifstream myfile (input_file);
    // ifstream myfile ("smv/smv_1.smv");
    // ifstream myfile ("smv/smv_2.smv");
    // ifstream myfile ("smv/smv_3.smv");
    // ifstream myfile ("smv/smv_4.smv");
    ifstream myfile ("smv/tiny.smv");
    // ifstream myfile ("smv_3.txt");
    // ifstream myfile ("bakery_3procs.txt");
    
    debug_out("Check file read.");
    if (myfile.is_open())
    {
        // collect variables and their types
        while ( getline(myfile,line))
        {
            cleanup(line);
            // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
            

            // if (line.find("--")!=string::npos) {
            //     line = line.substr(0, line.find("--"));
            //     std::cout << "line: " <<line << endl;
            // }

            // MODULE
            // std::cout << "line: " << line << endl;
            
            if (line.find("MODULE") != string::npos) {
                // TODO: modules
            }
            // VAR
            if (line == "VAR") {
                getline(myfile, line);
                cleanup(line);
                // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                // if (line.find("--")!=string::npos) {
                //     line = line.substr(0, line.find("--"));
                //     // std::cout << "line: " <<line << endl;
                // }
                debug_out(line);
                while (line != "ASSIGN") {
                    // cleanup(line);
                    if(is_command(line) || line == "" ) {
                        continue;
                        // getline (myfile, line);
                        // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    }
                    else{
                        init_var(vars, var_type, init, line);    
                    }
                    getline(myfile, line);
                    cleanup(line);
                    // while ((line[0] == '-' && line[1] == '-') || line == "" ) {
                    //     getline (myfile, line);
                    //     line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    //     if (line.find("--")!=string::npos) {
                    //         line = line.substr(0, line.find("--"));
                    //         // std::cout << "line: " <<line << endl;
                    //     }
                    // }
                    // std::cout << line << endl;
                    // init_var(vars, var_type, init, line);
                    // getline(myfile, line);
                    // cleanup(line);
                    // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    // if (line.find("--")!=string::npos) {
                    //     line = line.substr(0, line.find("--"));
                    //     // std::cout << "line: " <<line << endl;
                    // }
                    // skip comments and blank lines
                    // while (is_command(line) || line == "" ) {
                    //     getline (myfile, line);
                    //     line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    // }
                }
            } 
            // ASSIGN -- init() and next()
            if (line == "ASSIGN") {
                debug_out(line);
                
                // loop all the assignments
                while (getline(myfile, line) 
                            && line.find("DEFINE") == string::npos 
                            && line.find("MODULE") == string::npos) {
                    
                    cleanup(line);
                    // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    // if (line.find("--")!=string::npos) {
                    //     line = line.substr(0, line.find("--"));
                    //     // std::cout << "line: " <<line << endl;
                    // }
                    // std::cout << "ASSIGN: " << line << endl;
                    size_t found_next = line.find("next(");
                    
                    // find lines defined like "next(some_var) := some_next"
                    if (found_next != string::npos && line.find(";") != string::npos) {
                        found_next += 5;
                        char ch = line[found_next];
                        // cout << "test" << ch << endl;
                        string next_var;
                        while (ch != ')') {
                            next_var += ch;
                            found_next += 1;
                            ch = line[found_next];
                        }

                        vector<string> next_state_list = {"TRUE"};
                        // next[next_var].first = "TRUE";
                        line.pop_back();
                        size_t found_colon = line.find(":=");
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
                            next_state_list.push_back(line.substr(found_colon+2));
                        }

                        
                        next[next_var].push_back(next_state_list);
                    // find lines defined with "next(some_var)" and "esac;"
                    } else if (found_next != string::npos && line.find(";") == string::npos) {
                        found_next += 5;
                        char ch = line[found_next];
                        cout << "test" << ch << endl;
                        string next_var;
                        while (ch != ')') {
                            next_var += ch;
                            found_next += 1;
                            ch = line[found_next];
                        }
                        // debug_out(line);
                        // debugging
                        // std::cout << "next var chars: " << next_var << endl;
                        getline (myfile, line);
                        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                        if (line.find("--")!=string::npos) {
                            line = line.substr(0, line.find("--"));
                            std::cout << "line: " <<line << endl;
                        }
                        if (line == "case") {
                                    
                            getline(myfile, line);
                            line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                            if (line.find("--")!=string::npos) {
                                line = line.substr(0, line.find("--"));
                                std::cout << "line: " <<line << endl;
                            }
                            while (line.find("esac") == string::npos){

                                // skip over blank lines and comments
                                while (line == "" or line.find("--") != string::npos) {
                                    getline(myfile, line);
                                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                                    if (line.find("--")!=string::npos) {
                                        line = line.substr(0, line.find("--"));
                                        std::cout << "line: " <<line << endl;
                                    }
                                }
                                // debugging
                                // cout << "next line: " << line << endl;
                                next_state(vars, init, next, next_var, line);
                                        
                                getline(myfile, line);
                                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                                if (line.find("--")!=string::npos) {
                                    line = line.substr(0, line.find("--"));
                                    std::cout << "line: " <<line << endl;
                                }
                            
                            }
                        }
                    // find init lines
                    } else if (line.find("init") != string::npos) {
                        init_state(vars, var_type, init, line);
                        all_init_states = create_init_map(init);
                    // deal with comments and white space
                    } else {
                        // continue;
                    }
                }
            
            cleanup(line);
            // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
            // if (line.find("--")!=string::npos) {
            //         line = line.substr(0, line.find("--"));
            //         std::cout << "line: " <<line << endl;
            // }


            } if (line == "DEFINE") {
                string define_condition;
                string define_variable;
                while (getline(myfile, line) &&  line.find("MODULE") == string::npos) {
                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                    if (line.find("--")!=string::npos) {
                        line = line.substr(0, line.find("--"));
                        std::cout << "line: " <<line << endl;
                    }
                    size_t find_var_end = line.find(":=");
                    if (find_var_end != string::npos) {
                        define_variable = line.substr(0,find_var_end);
                        // cout << "define_var: " << define_variable;
                    }
                    // string full_condition;

                    // size_t found_first_paren = line.find("(");
                    size_t found_semi = line.find(";");
                    // std::cout << "FOUND: " << found_semi << endl;
                    if (find_var_end != string::npos) {
                        // one line define
                        if (found_semi != string::npos) {
                            define_condition = line.substr(find_var_end + 2, (line.length()-(find_var_end+3)));
                            if (define_condition[0] == '(' && define_condition[define_condition.length()-1] == ')') {
                                define_condition = define_condition.substr(1,define_condition.length()-2);
                            }
                            define_map.insert({define_variable, define_condition});
                            define_condition = "";
                        // multiple line define
                        } else {
                            define_condition += line.substr(find_var_end);
                        }
                    } else {
                        if (found_semi && define_map.find(define_variable) == define_map.end()) {
                            define_condition += line.substr(0,(line.length()-1));
                            if (define_condition[0] == '(' && define_condition[define_condition.length()-1] == ')') {
                                define_condition = define_condition.substr(1,define_condition.length()-2);
                            }
                            define_map[define_variable] = define_condition;
                            define_condition = "";
                        } else {
                            define_condition += line;
                        }
                    }
                }
            } 

        }
        myfile.close();





    }


    else{
        std::cout << "Error: Unable to open file";
        terminate();
    }
    
    // debugging
    std::cout << "\nVAR TYPE MAP:" << endl;
    for(const auto& elem : var_type)
    {
        std::cout << elem.first << " " << elem.second << "\n";
    }

    std::cout << "\nDEFINE MAP:" << endl;
    for(const auto& elem : define_map)
    {
        std::cout << elem.first << " " << elem.second << "\n";
    }

    std::cout << "\nINIT MAP:" << endl;
    for(const auto& elem : init)
    {
        std::cout << elem.first << " ";
        for (auto x : elem.second) {
            std::cout << x << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nNEXT MAP:" << endl;
    for(const auto& elem : next)
    {
        std::cout << elem.first << " transition: " << endl;
        for (auto x : elem.second) {
            // std::cout << "_________" << endl;
            for (auto z: x) {
                std::cout << z << ", ";
            }
        }
        std::cout << "\n";
    }

    // output initial conditions **************************************
    ofstream init_file;
    init_file.open("_init.bool");
    string to_file;
    for(const auto& elem : init)
    // elem.first = variable name
    // elem.second = vector of initial conditions
    {
        string output;
        for (auto condition: elem.second) {
            if (condition == "FALSE") {
                output += "~" + elem.first + " \\/ " ;
            } else if (condition == "TRUE") {
                output += elem.first + " \\/ " ;
            } else {
                // integers
                string max_num = var_type[elem.first];
                size_t found_char = max_num.find("..");
                max_num = max_num.substr(found_char + 2);
                // cout << "max_num: " << max_num << endl;
                vector<string> bin_var = to_binary(stoi(condition), elem.first, stoi(max_num));
                for (auto bit: bin_var) {
                    output += bit + " /\\ ";
                }
                // output = "(" + output.substr(0, output.length()-4) + ")";
            }
            to_file += output.substr(0, output.size()-4) + " /\\ \n";
        }
        
    }
    init_file << to_file.substr(0, to_file.length()-4);
    init_file.close();

    cout << "\n(DEBUG) "  << endl;
    // output transition relations ************************************************
    ofstream trans_file;
    trans_file.open("_trans.bool");
    stringstream final_output;

    // for (const auto& var: next) {
    //     for (const auto& x: var.second) {
    //         debug_out("?"+x[1]);
    //         debug_out("?"+x[2]);
    //     }
    // }

    // new
    string var_name;
    for (const auto& var: next) {
        var_name = var.first;
        debug_out("VARNAME: " + var_name);
        string cases;
        string rest_cases = "~(";
        for (const auto& x: var.second) {
            for (int i = 0; i < x.size() ; i+=2) {

                string LS = x[i];
                string RS = x[i+1];

                string LS_expr;
                string RS_expr;
                
                if(LS == "TRUE"){
                    if(rest_cases == "~("){
                        LS_expr = "";
                    }
                    else{
                        rest_cases.pop_back();
                        rest_cases.pop_back();
                        LS_expr = rest_cases + ")";
                    }
                }
                else{
                    LS_expr = evaluate(condition_tokenizer(LS), var_type);
                    rest_cases = rest_cases + LS_expr + "\\/";
                }

                string PRIME = "'";
                string NOT   = "~";
                string EQ    = "=";

                if (RS == "TRUE"){
                    RS_expr = var_name + PRIME;
                }
                else if (RS == "FALSE"){
                    RS_expr = NOT + var_name + PRIME;
                }
                // small trick
                else if (is_digit(RS)){
                    RS = var_name + EQ + RS;
                    RS_expr = prime_expr(evaluate(condition_tokenizer(RS), var_type));
                }
                else if (RS.find('+') != string::npos || RS.find('-') != string::npos){
                    string var_range = var_type[var_name];
                    string var_max = var_range.substr(var_range.find(".")+2, var_range.size());
                    string temp_L;
                    string temp_R;
                    RS_expr = "(";
                    for (int i = 0; i < stoi(var_max) ; i++){
                        temp_L = evaluate(condition_tokenizer(var_name+"="+to_string(i)), var_type);
                        temp_R = prime_expr(evaluate(condition_tokenizer(var_name+"="+to_string(i+1)), var_type));
                        RS_expr += "("+temp_L+" -> "+temp_R+")/\\\n";
                    }
                    RS_expr =  RS_expr.substr(0, RS_expr.size()-3);
                    RS_expr += ")";
                    debug_out(RS_expr);
                }
                else{
                    RS_expr = evaluate(condition_tokenizer(RS), var_type);
                    RS_expr = "(" + var_name + "'" + " <-> " + RS_expr + ")";
                } 
                
                if (LS_expr != ""){
                    cases = '(' + LS_expr + " -> " + RS_expr + ") /\\"; 
                }
                else{
                    cases = RS_expr + "/\\";
                }
                

                debug_out(cases);
                trans_file << cases << endl;
                
            }
        }
    }
    trans_file << "TRUE" << endl;
    
    trans_file << "\n\n(old output below)\n" << endl;


    // prev
    // for (const auto& var: next) {
    //     stringstream output;
    //     string var_name = var.first;
    //     // debug_out("VARNAME: " + var_name);
    //     for (const auto& x: var.second) {
    //         for (int i = 0; i < x.size() ; i++) {
    //     vector<string> true_conditions;
                
    //             /// RIGHT HAND SIDE
    //             string file_var;
    //             if (i != 0) {
    //                 if (x[i] == var.first) {
    //                     if (var_type[var.first].find("..")!=string::npos) {
    //                         // find initial condition
    //                         // blast var
    //                         // structure is ((PC0' <-> PC0) /\ (PC1' /\ PC1))
    //                         string max_num = var_type[var.first];
    //                         size_t found_char = max_num.find("..");
    //                         max_num = max_num.substr(found_char + 2);
    //                         int max = stoi(max_num);
    //                         // vector<string> bin_var = to_binary(n, var.first, stoi(max_num));
    //                         string max_r, max_bit;
    //                         int max_bit_order = 0;
    //                         while(max!=0) {
    //                             max_r = (max%2==0 ?"0":"1");
    //                             max_bit=(max%2==0 ?"0":"1") + max_bit; 
    //                             max/=2;
    //                             max_bit_order++;
    //                         }
    //                         string rhs_str;
    //                         for (int i = 0; i < max_bit_order; i++) {
    //                             rhs_str += "(" + var.first + "_" + to_string(i) + "' <-> " + var.first + "_" + to_string(i) + ") /\\ ";
    //                         }
    //                         file_var = "(" + rhs_str.substr(0, rhs_str.size()-4) + ")";


    //                     } else {
    //                         file_var = "(" + var.first + "' <-> " + var.first + ")";
    //                     }
    //                 } else if (x[i] == "TRUE"){
    //                     file_var = var.first + "'";
    //                 } else if (x[i] == "FALSE") {
    //                     file_var = "~" + var.first + "'";
    //                 } else {
    //                     bool digit = true;
    //                     for (int j = 0; j < x[i].length(); j++) {
    //                         if(!isdigit(x[i][j])) {
    //                             digit = false;
    //                         }
    //                     }
    //                     if (digit) {
    //                         // find max number for to binary function
    //                         int n = stoi(x[i]);
    //                         string max_num = var_type[var.first];
    //                         size_t found_char = max_num.find("..");
    //                         max_num = max_num.substr(found_char + 2);
    //                         // cout << "max_num: " << max_num << endl;
    //                         vector<string> bin_var = to_binary(n, var.first, stoi(max_num));
    //                         for (const auto& bit: bin_var) {
    //                             file_var += bit + "' /\\ ";
    //                         }
    //                         file_var = "(" + file_var.substr(0, file_var.length()-4) + ")";
    //                     }
    //                 }
    //             }

    //             if (file_var == "") {
    //                 file_var = x[i];
    //             }
    //             // cout << "file_var: " << file_var << endl;
    //             // if (file_var == "TRUE") {
    //             //     cout << "TRUEEEEEEEEEEEEEEEE" << endl;
    //             // }
    //             if (i == 0) {
    //                 if (file_var != "TRUE"){
    //                     output <<  evaluate(condition_tokenizer(file_var), var_type)  << " -> " ;
    //                     true_conditions.push_back(evaluate(condition_tokenizer(file_var), var_type));
    //                 } else {
    //                     string true_cond;
    //                     for (auto condition: true_conditions) {
    //                         true_cond += condition + " /\\ ";
    //                     }
    //                     true_cond = "~(" + true_cond.substr(0, true_cond.length()-4) + ")";
    //                     output << true_cond << " -> ";                      
    //                 }
 
    //             } else if ((i+1) != x.size()){
    //                 output << file_var  << " \\/ ";
    //             } else {
    //                 output << file_var ;
                    
    //             }

    //         }
    //         output << " /\\ \n" ;
    //     }
    //     final_output << output.str() << "\n" << endl;
    //     // if (define_map.empty()) {
    //     //     trans_file <<  output.str().substr(0,output.str().size()-4) << "\n" << endl;
    //     // } else {
    //     //     trans_file <<  output.str() << "\n" << endl;
    //     // }
    //     // trans_file <<  output.str().substr(0, output.str().length()-5) << "\n" << endl;
    // }
    // if (define_map.empty()){
    //     trans_file <<  final_output.str().substr(0,final_output.str().size()-6) << "\n" << endl;
    // } else {
    //     trans_file <<  final_output.str() << "\n" << endl;
    // }

    // DEFINE
    stringstream define_ss;
    for(const auto& elem : define_map)
    {
        string condition = evaluate(condition_tokenizer(elem.second), var_type);
        // trans_file << elem.first << endl;
        define_ss << elem.first << " <-> " << condition << " /\\ " << elem.first << "' <-> " <<  condition <<  " /\\ \n";
    }
    trans_file << define_ss.str().substr(0, define_ss.str().size()-4);
    trans_file.close();    




    return 0;




    // vector <vector<pair<string,string>>> all_possible_inits;
    // for (const auto& elem : var_type)
    // {
    //     vector<pair<string,string>> temp = {};
    //     if (elem.second == "boolean") {
    //         pair<string,string> temp_pair (elem.first, "TRUE");
    //         pair<string,string> temp_pair_2 (elem.first, "TRUE");
    //         temp = {temp_pair, temp_pair_2};
    //     } else {
    //         size_t found_middle = elem.second.find("..");
    //         string lower_range = elem.second.substr(0,found_middle);
    //         string upper_range = elem.second.substr(found_middle + 2);
    //         // cout << "Upper: " << upper_range << endl;
    //         // cout << "Lower: " << lower_range << endl;
    //         for (int i = stoi(lower_range) ; i < stoi(upper_range) +1; i++) {
    //             cout << "NUM: " << i << endl;
    //             pair<string, string> temp_pair (elem.first, to_string(i));
    //             temp.push_back(temp_pair);
    //         }
    //     }
    //     all_possible_inits.push_back(temp);
    // }

    // cartesian product of all possible inits
    // all_possible_inits = cart_product(all_possible_inits);

    // for (const auto& elem: all_possible_inits) {
    //     for (const auto& state: elem) {
    //         cout << "ALL POSSIBLE: " << state.first << ",  " << state.second << endl;
    //     }
    // }
    // for (const auto& elem: all_init_states) {
    //     for (const auto& state: elem) {
    //         cout << "Initial conditions: " << state.first << ",  " << state.second << endl;
    //     }
    // }
    // write to text file
    // ofstream my_file;
    // my_file.open("output.txt");
    // for (auto state: all_init_states) {
    //     stringstream output = transition_relation(var_type, state, next, define_map);
    //     my_file <<  output.str() << "\n" << endl;

    // }
    // my_file.close();


    // ofstream trans_file;
    // trans_file.open("output_2.txt");
    // for (auto state: all_possible_inits) {
    //     stringstream output = transition_relation(var_type, state, next, define_map);
    //     trans_file <<  output.str() << "\n" << endl;

    // }
    // trans_file.close();
    // debugging

    // vector<vector<string>> v = {{"a", "~a"}, {"b", "~b"}};
    // vector<vector<string>> prod = cart_product(v);

    // for (auto x: prod) {
    //     cout << "________" << endl;
    //     for (auto y: x) {
    //         cout << y << endl;
    //     }
    // }

    // vector<string> bin = to_binary(10, "PC");
    // cout << "bin: " << bin << endl;

    // vector<pair<string,string>> condition_vec = condition_tokenizer("PCA=3 | PCA=4");
    // for (auto x: condition_vec) {
    //     cout << "X: " << x.second << endl;
    // }

    // return 0;
}

// started <-> condition /\ started' <-> condition




// finding the correct next states

// stringstream transition_relation (map<string, string> var_type, vector<pair<string,string>> init_state, map<string, vector<vector<string>>> all_next, map<string, string> define_map) {
//     // for (auto x: init_state) {
//     //     cout << "init_state: " << x.first << x.second << endl;
//     // }
//     // cout << "complete" << endl;
//     stringstream ss;
//     string init_str;
//     ss << "(" ;

//     map<string, vector<string>> next;
//     // uncomment
//     map<string, string> define_pairs;
//     for(auto elem : define_map)
//     {
//         string define_eval = evaluate(condition_tokenizer(elem.second), init_state, define_map);
//         define_pairs[elem.first] =  define_eval;
//     }

//     // std::cout << "DEFINE PAIRS: " << endl;
//     // for (auto elem: define_pairs) {
//     //     std::cout << elem.first <<  "  " << elem.second << endl;
//     // }

//     // for (auto x: init_state) {
//     //     if (x.second == "FALSE") {
//     //         init_str += "~" + x.first + "/\\";
//     //     } else {
//     //         init_str += x.first + "/\\";
//     //     } 
//     // }
//     // std::cout << "\nALL NEXT MAP:" << endl;

//     // for(const auto& elem : all_next)
//     // {
//     //     std::cout << elem.first << " ";
//     //     for (auto x : elem.second) {
//     //         for (auto z: x) {
//     //             std::cout << z << " ";
//     //         }
//     //     }
//     //     std::cout << "\n";
//     // }
//     for (auto x: define_pairs) {
//         if (x.second == "FALSE") {
//             init_str += "~" + x.first + "/\\";
//         } else {
//             init_str += x.first + "/\\";
//         }  
//     }

//     for (auto x: init_state) {
//         if (std::find_if(x.second.begin(), x.second.end(), [](unsigned char c) { return !std::isdigit(c);}) == x.second.end()) {
//             size_t num = var_type[x.first].find("..");
//                 int max = stoi(var_type[x.first].substr(num + 2));
//             vector<string> bin_vec = to_binary(stoi(x.second), x.first, max);
//             for (auto bit: bin_vec) {
//                 init_str += bit + "/\\";
//             }
//         } else {
//             if (x.second == "FALSE") {
//                 init_str += "~" + x.first + "/\\";
//             } else {
//                 init_str += x.first + "/\\";
//             }   
//         }
    
//         for (auto y: all_next) {
//                 // cout << "ALL_NEXT: " << x << endl;
//                 // std::cout << "ALL NEXT: " << y.first << endl;
//             for (auto z: y.second) {
//                 // std::cout << "z[0]: " << z[0] << endl;
//                 string var;
//                 // int num;
                
//                 if ((evaluate(condition_tokenizer(z[0]), init_state, define_map) == "TRUE" && next.find(y.first) == next.end()) || (define_pairs[z[0]] == "TRUE" && next.find(y.first) == next.end()) ) {
//                     vector<string>::const_iterator first = z.begin() + 1;
//                     vector<string>::const_iterator last = z.begin() + z.size();
//                     vector<string> newVec(first, last);
//                     if (newVec.size() ==1 && newVec[0] == y.first) {
//                         for (auto state: init_state) { 
//                             newVec = {state.second};
//                         }
//                     }
//                     next[y.first] = newVec;
//                 }

//                 // if (z[0].find("=") != string::npos) {
//                 //     size_t equality = z[0].find("=");
//                 //     var = z[0].substr(0,equality);
//                 //     num = stoi(z[0].substr(equality + 1));
//                 // }
//                 // // equlaity condition
//                 // if (var == x.first && num == stoi(x.second)) {
//                 //     vector<string>::const_iterator first = z.begin() + 1;
//                 //     vector<string>::const_iterator last = z.begin() + z.size();
//                 //     vector<string> newVec(first, last);
//                 //     next[y.first] = newVec;
//                 // }
//                 // 
//                 if ((z[0] == x.first && x.second == "TRUE") || (z[0] == "~"+x.first && x.second == "FALSE")|| (z[0] == "TRUE" && next.find(y.first) == next.end())) {
//                     vector<string>::const_iterator first = z.begin() + 1;
//                     vector<string>::const_iterator last = z.begin() + z.size();
//                     vector<string> newVec(first, last);
//                     next[y.first] = newVec;
//                 } 
//             }
//             // if (y.first == x) {
//             //     next[y.first] = y.second;
//             }
//     }
//     std::cout << "NEXT STATES MAP" << endl;
//     for (auto x: next) {
//         std::cout  << "next:   "<< x.first;
//         for (auto y: x.second) {
//             std::cout << "   " << y << endl;
//         }
//     }

//     // for(const auto& elem : next)
//     // {
//     //     std::cout << elem.first << " ";
//     //     for (auto x : elem.second) {
//     //         std::cout << x << " ";
//     //     }
//     //     std::cout << "\n";
//     // }

//     ss << init_str.substr(0, init_str.length() - 2) << ") -> (" ;
//     string next_str;
//     vector<vector<string>> next_vec_pairs;
//     // int combinations = 1;



//     // clean this
//     // trying to figure out how to find all possible pairs
//     for (auto x: next) {
//         vector<string> possible_states;
//         for (auto y: x.second) {
//             // std::cout << "Y:  " << y << endl;
//             if (y == "FALSE") {
//                 string var = "~" + x.first + "'";
//                 possible_states.push_back(var);
//             } else if (y == "TRUE") {
//                 string var = x.first + "'";
//                 possible_states.push_back(var);
//             // var stays the same
//             } else if (y == x.first) {
//                 for (auto var_pair: init_state) {
//                     if (var_pair.first == x.first) {
//                         if (var_pair.second == "TRUE") {
//                             possible_states.push_back(x.first + "'");
//                         } else {
//                             possible_states.push_back("~" + x.first + "'");
//                         }
//                     }
//                 }
                
//             // if variable + int
//             } else if (y.find("+") != string::npos) {
//                 // std::cout << "DEBUGGGGG" << endl;
//                 size_t plus = y.find("+");
//                 string var = y.substr(0, plus);
//                 int current_val;
//                 int addend = stoi(y.substr(plus +1));
//                 for (auto var_pair: init_state) {
//                     if (var_pair.first == var) {
//                         current_val = stoi(var_pair.second);
//                     }
//                 }
//                 // std::cout << "CURRENT: " << current_val << endl;
//                 int new_num = addend + current_val;
//                 // std:: cout << "NEW NUM: " << new_num << endl;
//                 size_t num = var_type[var].find("..");
//                 int max = stoi(var_type[var].substr(num + 2));
//                 vector<string> bin = to_binary(new_num, var, max);
//                 string bin_var;
//                 for (auto x: bin) {
//                     // std::cout << "BINARY: " << x << endl;
//                     bin_var += x + "'" + "/\\";
//                 }
//                 possible_states.push_back(bin_var.substr(0,bin_var.length()-2));
//             // just number
//             } else {
//                 string var = x.first;
//                 // perform binary here
//                 // cout << "Y:  " << y << "    varr:   " << var << endl;
//                 size_t num = var_type[var].find("..");
//                 int max = stoi(var_type[var].substr(num + 2));
//                 vector<string> bin = to_binary(stoi(y), var, max);
//                 string bin_var;
//                 for (auto x: bin) {
//                     bin_var += x + "'" + "/\\";
//                 }
//                 possible_states.push_back(bin_var.substr(0,bin_var.length()-2));
//             }
//         }
//         // combinations *= possible_states.size();
//         for (auto x: possible_states) {
//             // cout << "state: " << x << endl;
//         }
//         next_vec_pairs.push_back(possible_states);
//     }
    
    

//     // cout << "combinations: " << combinations << endl;
//     // for (auto x: next_vec_pairs) {
//     //     for (auto y: x) {
//     //         cout << "y: " << y << endl;
//     //     }
//     // }

//     next_vec_pairs = cart_product_2(next_vec_pairs);

//     for (int i =0; i < next_vec_pairs.size(); i++) {
//         string formula;
//         for (auto state: next_vec_pairs[i]) {
//             formula += state + "/\\";
//         }
//         ss << "(" << formula.substr(0, formula.length()-2) << ")";
//         if ((i+1) < next_vec_pairs.size()) {
//             ss << "\\/";
//         }
//     }
//     ss << ")";
//     // for (auto x: next_vec_pairs) {
//     //     cout << "________" << endl;
//     //     for (auto y: x) {
//     //         cout << y << endl;
//     //     }
//     // }

//     // for (int i = 0; i < next_vec_pairs.size(); i++) {
//     //     for (int j = 0; j < 2; j++) {
//     //         string formula = next_vec_pairs[i][0] + "/\\" + next_vec_pairs[i][1];
//     //         cout << formula << endl;
//     //     }
//     // }
    
//     return ss;
// }
