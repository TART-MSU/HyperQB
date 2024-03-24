// TODO: add the syntax checking 
// TODO: add undefined variable checking (string seg fault)

#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
#include <cstdlib>
using namespace std;

/* GLOBAL SETTING */
string TRUE     = "TRUE";
string FALSE    = "FALSE";
string AND      = "/\\";
string OR       = "\\/";
string IMPLIES  = "->";
string IFF      = "<->";
string NOT      = "~";
string NEWL     = "\n";
string EQ       = "=";
string PLUS     = "+";
string MINUS    = "-";
string COMMENT  = "--";
string PRIME    = "'";
string OPEN_PARAN   = "(";
string CLOSE_PARAN  = ")";

/* DEBUG */
void debug_out(string s){
    cout << s << endl;
}

/* Reutru true if a string is a digit */
bool is_digit(const std::string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

/* Get the highest bit number from a given num */
unsigned int get_maxbit(unsigned int number){
   unsigned int count = 0;
   unsigned i;
   while (number){
      count++;
      number >>= 1;
   }
   return (count-1);
}

/* Get the range of a numerical variable <min, max>*/
pair<int,int> get_range(string name, map<string, string> &var_type){
    string range = var_type[name];
    size_t found_char = range.find("..");
    int min_num = stoi(range.substr(0,range.length()-(range.length()-found_char)));
    int max_num = stoi(range.substr(found_char + 2));
    return make_pair(min_num, max_num);
}

/* Bit-blasting a variable according to its max value */
vector<string> to_binary(int n, string var, int max)
{
    //TODO: what is n, OK N IS THE BOUND??
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

/* handle conditions */
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
        } 
        // BUG
        // else if (condition[i] == '!'){
        //     condition_vec.push_back({"op", "!"});
        //     i+=1;} 
        else if (condition[i] == '=') {
            condition_vec.push_back({"op", "="});
            i+=1;
        } else {
            string var;
            int j = i;
            while (condition[j] != '&'  
                && condition[j] != '|' 
                && condition[j]!= ' ' 
                && j < condition.length() 
                && condition[j] != '=' 
                && condition[j] != ')' 
                && condition[j] != '(' 
                && condition.substr(j, 2) != "!=" 
                && condition.substr(j, 2) != "<="
                && condition.substr(j, 2) != ">=" 
                && condition[j] != '>' 
                && condition[j] != '<') 
            {
                var += condition[j];
                j+=1;
            }
            i += (j-i);
            condition_vec.push_back({"var", var});
        }
    }
    return condition_vec;
}

/* Find precedence of operators */
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
 
/* Arithmetic operations*/
string applyOp(string a, string b, string op, map<string, string> &var_type){
    string a_val;
    string b_val;
    if (a.substr(0,1) == "!") {
        a_val = "~" + a.substr(1);
    }

    if (b.substr(0,1) == "!") {
        b_val = "~" + b.substr(1);
    }

    if (a_val == "") {
        a_val = a;
    }

    if (b_val == "") {
        b_val = b;
    }

    if (op == "&") {
        return OPEN_PARAN + a_val + AND + b_val + CLOSE_PARAN;
    } else if (op == "|") {
        return OPEN_PARAN + a_val + OR + b_val + CLOSE_PARAN;
    } else if (op == "="){
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        max_num = max_num.substr(found_char + 2);
        vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
        string blasted_var;
        for (const auto& bit: bits) {
            blasted_var += bit + AND;
        }
        return "(" + blasted_var.substr(0, blasted_var.length()-AND.size()) + ")";
    } else if (op == "!=") {
        string max_num = var_type[a_val];
        size_t found_char = max_num.find("..");
        max_num = max_num.substr(found_char + 2);
        vector<string> bits = to_binary(stoi(b_val), a_val, stoi(max_num)); // change to max_num
        string blasted_var;
        for (const auto& bit: bits) {
            blasted_var += bit + AND;
        }
        return "~(" + blasted_var.substr(0, blasted_var.length()-4) + ")";
    } else if (op == ">=") {
        string all_vars;
        int min_num = (get_range(a_val, var_type).first);
        int max_num = (get_range(a_val, var_type).second);
        int bound = stoi(b_val);
        for (int i = max_num; i >= bound; i--) {
            vector<string> bits = to_binary(i, a_val, max_num); // BUG: fixed
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + AND;
            }
            all_vars +=  OPEN_PARAN + blasted_var + CLOSE_PARAN + AND;
        }
        return all_vars.substr(0,all_vars.length()-CLOSE_PARAN.size()-1) ;
    } else if (op == "<=") {
        string all_vars;
        int min_num = (get_range(a_val, var_type).first);
        int max_num = (get_range(a_val, var_type).second);
        if (is_digit(b_val)){
            int bound = stoi(b_val);
            for (int i = min_num; i <= bound; i++) {
                vector<string> bits = to_binary(i, a_val, max_num); // BUG: fixed
                string blasted_var;
                for (const auto& bit: bits) {
                    blasted_var += bit + AND;
                }
                all_vars +=  OPEN_PARAN + blasted_var + CLOSE_PARAN + AND;
            }
            return all_vars.substr(0,all_vars.length()-CLOSE_PARAN.size()-1);
        } else {
            int a_min = (get_range(a_val, var_type).first);
            int a_max = (get_range(a_val, var_type).second);
            int b_min = (get_range(b_val, var_type).first);
            int b_max = (get_range(b_val, var_type).second);
            if (get_maxbit(a_max) == get_maxbit(b_max)){
                debug_out("work");
                string expr = "";
                string enumeration = "";
                for (int a = a_min; a <= a_max; a++){
                    for (int b = a; b <= b_max; b++){
                        string left = (applyOp(a_val,to_string(a),"=", var_type));
                        string right = (applyOp(b_val,to_string(b),"=", var_type));
                        expr = OPEN_PARAN + left + AND + right + CLOSE_PARAN;
                        enumeration += expr + OR;
                    }
                }
                enumeration = enumeration.substr(0, enumeration.size()-OR.size());
                return OPEN_PARAN + enumeration + CLOSE_PARAN;
                // TODO: fix all other arithmetic operations!
            } else {
                debug_out("MODEL ERROR: two integers must have same number of bits for comparison");
                exit(0);
            }
        }
    } else if (op == ">") {
        string all_vars;
        int min_num = (get_range(a_val, var_type).first);
        int max_num = (get_range(a_val, var_type).second);
        int bound = stoi(b_val);
        for (int i = max_num; i > bound; i--) {
            vector<string> bits = to_binary(i, a_val, max_num); // BUG: fixed
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + AND;
            }
            all_vars +=  OPEN_PARAN + blasted_var + CLOSE_PARAN + AND;
        }
        return all_vars.substr(0,all_vars.length()-CLOSE_PARAN.size()-1) ;
    } else if (op == "<") {
        string all_vars;
        int min_num = (get_range(a_val, var_type).first);
        int max_num = (get_range(a_val, var_type).second);
        int bound = stoi(b_val);
        for (int i = min_num; i < bound; i++) {
            vector<string> bits = to_binary(i, a_val, max_num); // BUG: fixed
            string blasted_var;
            for (const auto& bit: bits) {
                blasted_var += bit + AND;
            }
            all_vars +=  OPEN_PARAN + blasted_var.substr(0,blasted_var.length()-AND.size()) + CLOSE_PARAN + AND;
        }
        return all_vars.substr(0,all_vars.length()-AND.size()) ;
    }

    return "FALSE";   
}
 
/* Returns value of expression after evaluation */
string evaluate(vector<pair<string,string>> tokens, map<string,string> &var_type){
    // cout << "size:" << tokens.size() << endl;
    // cout << tokens[0].second << endl;
    if (tokens.size() == 1) { 
        return tokens[0].second;      
    }
    stack <string> values; // stack to store vars and vals
    stack <string> ops; // stack to store operators
    for(int i = 0; i < tokens.size(); i++){
        // cout << tokens[i].second << endl;
        // Current token is an opening brace, push it to 'ops'
        if(tokens[i].second == "("){
            ops.push("(");
        }
        else if(tokens[i].first == "var"){
            values.push(tokens[i].second);
        // Closing brace encountered, solve entire brace.
        } else if(tokens[i].second == ")"){
            while(!ops.empty() && ops.top() != "("){
                string val2 = values.top();
                values.pop();
                string val1 = values.top();
                values.pop();
                string op = ops.top();
                ops.pop();
                values.push(applyOp(val1, val2, op, var_type));
            }
            // pop opening brace.
            if(!ops.empty())
               ops.pop();
        }
        // Current token is an binary operator.
        else{
            // While top of 'ops' has same or greater precedence to current token, which
            // is an operator, apply operator on top of 'ops' to top two elements in values stack.
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
     
    // Entire expression has been parsed at this point
    // apply remaining ops to remaining values.
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

/* cartesian product for TRUE case*/
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

/* create state map of initial conditions */
vector<vector<pair<string,string>>> create_init_map (map<string, vector<string>> &init) {
    map<string, vector<string>> init_state_map;
    vector<vector<pair<string,string>>> state_vec;

    for (auto var: init) {
        vector<pair<string,string>> possible_states;
        string var_name = var.first;
        for (auto var_inits: var.second) {
            if (var_inits == "FALSE") {
                possible_states.push_back({var_name, "FALSE"});
            } else if (var_inits == "TRUE") {
                possible_states.push_back({var_name, "TRUE"});
            } else {
                possible_states.push_back({var_name, var_inits});
            }
        }
        state_vec.push_back(possible_states);
    }
    state_vec = cart_product(state_vec);
    return state_vec;
}

/* set initial conditions */
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
    // std::cout << "init_cond:  " << init_state_list[2] << endl;
    init[var_str] = init_state_list;
}

/* store vars in maps and sets */
void store_var (set<string> &vars, map<string, string> &var_type, map<string, vector<string>> &init, string init_str) {
    int i = 0;
    string var_str;
    while (init_str[i] != ':') {
        // debugging
        // std::cout << init_str[i] << endl;
        var_str += init_str[i];
        i++;
    }
    vars.insert(var_str);
    init_str.pop_back();
    string type_info = init_str.substr(i+1);
    var_type[var_str] = type_info;
    // debugging
    // std::cout << "var: " << type_info << endl;
}

/* finding all possible next states, no matter init conditions */
void next_state (set<string> &vars, map<string, vector<string> > &init, map<string, vector<vector<string> > > &next, string next_var, string line) {
    line.pop_back();
    // cout << "next var: " << next_var << endl;
    // cout << "line: " << line << endl;
    string condition;
    vector<string> next_state_list;
    if (line.find("TRUE") == 0) {
        condition = "TRUE";
        size_t found_colon = line.find(":");
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
    // conditional transition
    } else {
        size_t open_paren = line.find("(");
        size_t closed_paren = line.find(")");
        condition = line.substr(open_paren + 1, closed_paren - (open_paren + 1));
        size_t found_colon = line.find(":");
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
    }

    next_state_list.push_back(condition);
    reverse(next_state_list.begin(), next_state_list.end()); //TODO: why?
    // for (auto x: next_state_list) {
        // cout << "BRO; " << x << endl;
    // }
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
    return rt;
}

/* add prime to a string */
string primed(string var){
    return var+PRIME;
}

/* Clean up the comments with '--' */
void cleanup(string &line){
    // string comments = "--";
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
    if (line.find(COMMENT)!=string::npos) {
                line = line.substr(0, line.find(COMMENT));
                // std::cout << "line: " <<line << endl;
    }
}

/* If a line is a comment */
bool is_comment(string line){
    return line[0] == '-' && line[1] == '-';
}

/* Match numerical values with each bit */
vector<string> match_bits(string v1, string v2, int maxnum)
{
    vector<string> bit_vector;
    bit_vector.push_back("(");
    int max_bit = get_maxbit(maxnum);
    
    while(max_bit > (-1)) {
        bit_vector.push_back("(" + v1 + "_" + to_string(max_bit) + "<->" + v2 + "_" + to_string(max_bit) + "')");
        bit_vector.push_back("/\\");
        max_bit = max_bit-1;
    }
    bit_vector.pop_back();
    bit_vector.push_back(")");
    return bit_vector;
}

size_t get_maxnum(string v1, map<string,string> &var_type){
    string max_num = var_type[v1];
    size_t found_char = max_num.find("..");
    return stoi(max_num.substr(found_char + 2));
}

string match_nums(string v1, string v2, map<string,string> &var_type){
    size_t maxnum = get_maxnum(v2, var_type);
    // size_t v2_max = get_maxnum(v2, var_type);
    vector<string> matches = match_bits(v1, v2, maxnum);
    std::string s;
    for (const auto &piece : matches) s += piece;
    return s;
    // return str(matches.begin(), matches.end());
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        size_t end_pos = start_pos + from.length();
        str.replace(start_pos, end_pos, to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

int main(int argc, char** argv) {
    set<string> vars;
    map<string, string> var_type;
    map<string, vector<string> > init;
    map<string, vector<vector<string> > > next;
    vector<vector<pair<string,string> > > all_init_states;
    map<string, string> define_map; // variable : condition

    string line, input_file;
    string smv_name = argv[1];
    string I_name = argv[2];
    string R_name = argv[3];

    ifstream myfile (smv_name);

    /* FILE READ */
    if (myfile.is_open())
    {
        // collect variables and their types
        while(getline(myfile,line))
        {
            cleanup(line);
            if (line.find("MODULE") != string::npos) { }
            // VAR
            if (line == "VAR") {
                getline(myfile, line);
                cleanup(line);
                while (line != "ASSIGN") {
                    store_var(vars, var_type, init, line);    
                    getline(myfile, line);
                    cleanup(line);
                }
            } 
            
            if (line == "ASSIGN") {
                // ASSIGN -- init() and next()
                // loop all the assignments
                while(getline(myfile, line) 
                        && line.find("DEFINE") == string::npos 
                        && line.find("MODULE") == string::npos) {
                    cleanup(line);
                    size_t found_next = line.find("next(");
                    // find lines defined like "next(some_var) := some_next"
                    if (line.find("next(") != string::npos && line.find(";") != string::npos) 
                    {
                        found_next += 5;
                        char ch = line[found_next];
                        string next_var;
                        while (ch != ')') {
                            next_var += ch;
                            found_next += 1;
                            ch = line[found_next];
                        }

                        vector<string> next_state_list = {"TRUE"};
                        size_t found_colon = line.find(":=");
                        size_t found_bracket = line.find("{");
                        if (found_bracket != string::npos) {
                            line.pop_back(); // pop "}"
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
                        // cout << "test" << ch << endl;
                        string next_var;
                        while (ch != ')') {
                            next_var += ch;
                            found_next += 1;
                            ch = line[found_next];
                        }
                        // debugging
                        // std::cout << "next var chars: " << next_var << endl;
                        getline(myfile, line);
                        cleanup(line);
                        // debug_out(line);
                        // line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                        // if (line.find("--")!=string::npos) {
                        //     line = line.substr(0, line.find("--"));
                        //     // std::cout << "line: " <<line << endl;
                        // }
                        if (line == "case") {
                            getline(myfile, line);
                            cleanup(line);
                            line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                            if (line.find("--")!=string::npos) {
                                line = line.substr(0, line.find("--"));
                                // std::cout << "line: " <<line << endl;
                            }
                            while (line.find("esac") == string::npos){
                                // skip over blank lines and comments
                                while (line == "" or line.find("--") != string::npos) {
                                    getline(myfile, line);
                                    line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                                    if (line.find("--")!=string::npos) {
                                        line = line.substr(0, line.find("--"));
                                        // std::cout << "line: " <<line << endl;
                                    }
                                }
                                // debugging
                                // cout << "next line: " << line << endl;
                                next_state(vars, init, next, next_var, line);
                                        
                                getline(myfile, line);
                                line.erase(std::remove_if(line.begin(), line.end(), ::isspace),line.end());
                                if (line.find("--")!=string::npos) {
                                    line = line.substr(0, line.find("--"));
                                    // std::cout << "line: " <<line << endl;
                                }
                            
                            }
                        }
                    // find init lines
                    } 
                    else if (line.find("init") != string::npos) {
                        init_state(vars, var_type, init, line);
                        all_init_states = create_init_map(init);
                    } else {
                        // comments and white space
                        // continue;
                    }
                }
            cleanup(line);
            // DEFINE section
            } if (line == "DEFINE") {
                string define_condition;
                string define_variable;
                while (getline(myfile, line)) {
                    cleanup(line);
                    size_t find_var_end = line.find(":=");
                    if (find_var_end != string::npos) {
                        define_variable = line.substr(0,find_var_end);
                    }
                    debug_out(line);
                    // string full_condition;
                    // size_t found_first_paren = line.find("(");
                    size_t found_semi = line.find(";");

                    
                    string condition = line.substr(0, found_semi);
                    condition = condition.substr(find_var_end+2, condition.size()); // bad move

                    // std::cout << "FOUND: " << found_semi << endl;
                    if (find_var_end != string::npos) {
                        // one line define
                        if (found_semi != string::npos) {
                            define_condition = line.substr(find_var_end+2, (line.length()-(find_var_end+3)));
                            // BE CAREFUL!! CHECK AGAIN!
                            // if (define_condition[0] == '(' && define_condition[define_condition.length()-1] == ')') {
                            //     define_condition = define_condition.substr(1,define_condition.length()-2);
                            // }
                            define_map.insert({define_variable, define_condition});
                            define_condition = "";
                        // multiple line define
                        } else {
                            define_condition += line.substr(find_var_end);
                        }
                    } else {
                        if (found_semi && define_map.find(define_variable) == define_map.end()) {
                            define_condition += line.substr(0,(line.length()-1));
                            // if (define_condition[0] == '(' && define_condition[define_condition.length()-1] == ')') {
                            //     define_condition = define_condition.substr(1,define_condition.length()-2);
                            // }
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
    
    // DEBUG
    // std::cout << "\nVAR TYPE MAP:" << endl;
    // for(const auto& elem : var_type)
    // {
    //     std::cout << elem.first << " " << elem.second << "\n";
    // }

    std::cout << "\nDEFINE MAP:" << endl;
    for(const auto& elem : define_map)
    {
        std::cout << elem.first  << "\n";
        std::cout << elem.second << "\n";
    }

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
    //     std::cout << elem.first << " trans: " << endl;
    //     for (auto x :elem.second) {
    //         for (auto z: x) {
    //             std::cout << z << ", ";
    //         }
    //         std::cout << "\n";
    //     }
    //     std::cout << "\n";
    // }

    /* WRITE INITIAL I.bool */
    // TODO: ADD NonDeterministic Inputs
    ofstream init_file;
    init_file.open(I_name);
    string to_file;
    for(const auto& elem : init)
    {
        string output;
        string varname = elem.first;
        debug_out(varname);
        vector<string> inits = elem.second;
        to_file += OPEN_PARAN;
        for (auto condition: inits) {
            debug_out(condition);
            if (condition == "FALSE") {
                to_file += NOT + varname;
            } else if (condition == "TRUE") {
                to_file += varname;
            } else {
                // integer
                int max_num = get_range(varname, var_type).second;
                output = "";
                vector<string> bin_var = to_binary(stoi(condition), varname, max_num);
                for (auto bit: bin_var) {
                    output += bit + AND;
                }
                to_file += output.substr(0, output.length()-AND.size());
            }
            to_file += OR;
        }
        to_file = to_file.substr(0, to_file.length()-OR.size()) + CLOSE_PARAN;
        to_file += AND + "\n";
    }
    to_file = to_file.substr(0, to_file.length()-AND.size()-1);
    init_file << to_file; // DANGEROUS
    init_file.close();
    
    /* WRITE TRANSITION RELATIONS R.bool */
    ofstream trans_file;
    trans_file.open(R_name);
    stringstream final_output;
    // THH: new code
    string var_name;
    for (const auto& var: next) {
        var_name = var.first;
        string cases;
        string rest_cases = "~(";
        for (const auto& x: var.second) {
            // parse pre-condition
            string LS = x[0];
            string LS_expr;
            if(LS == "TRUE"){
                if (var.second.size()==1){
                    LS_expr = "TRUE";
                } else {
                    if(rest_cases == "~("){
                        LS_expr = "";
                    }
                    else{
                        rest_cases.pop_back();
                        rest_cases.pop_back();
                        LS_expr = rest_cases + ")";
                    }
                }
            } else {
                LS_expr = evaluate(condition_tokenizer(LS), var_type);
                replaceAll(LS_expr, "!", "~"); // patch
                rest_cases = rest_cases + LS_expr + "\\/";
            }    
            // parse pos-condition
            string RS_expr;
            for (int i = 1; i < x.size() ; i+=1) {
                string RS = x[i];
                if (RS == "TRUE"){
                    RS_expr += var_name + PRIME;
                }
                else if (RS == "FALSE"){
                    RS_expr += NOT + var_name + PRIME;
                }
                else if (var_type[RS].find("..") != string::npos){
                    RS_expr = match_nums(RS, var_name, var_type);
                }
                else if (is_digit(RS)) {
                    RS = var_name + EQ + RS;
                    RS_expr += prime_expr(evaluate(condition_tokenizer(RS), var_type));
                }
                // RS contains arithmetic operations
                // TODO: range might be another number
                else if (RS.find('+') != string::npos){
                    int var_max = get_range(var_name, var_type).second;
                    string temp_L, temp_R;
                    //TODO: increment with different number
                    for (int k = 0; k < (var_max) ; k++){
                        temp_L = evaluate(condition_tokenizer(var_name+"="+to_string(k)), var_type);
                        temp_R = prime_expr(evaluate(condition_tokenizer(var_name+"="+to_string(k+1)), var_type));
                        RS_expr += "(("+temp_L+") -> ("+temp_R+"))" + AND + "\n";
                    }
                    RS_expr =  RS_expr.substr(0, RS_expr.size()-AND.size()-1); //BUG               
                } else if (RS.find('-') != string::npos){
                    int var_max = get_range(var_name, var_type).second;
                    //TODO: increment with different number
                    string temp_L, temp_R;
                    for (int k = (var_max); k >= (0) ; k--){
                        temp_L = evaluate(condition_tokenizer(var_name+"="+to_string(k)), var_type);
                        temp_R = prime_expr(evaluate(condition_tokenizer(var_name+"="+to_string(k-1)), var_type));
                        RS_expr += "(("+temp_L+") -> ("+temp_R+"))" + AND + "\n";
                    }
                    RS_expr =  RS_expr.substr(0, RS_expr.size()-AND.size()-1); //BUG               
                } else {   
                    string expr = evaluate(condition_tokenizer(RS), var_type);
                    RS_expr += OPEN_PARAN + expr + IFF + primed(var_name) + CLOSE_PARAN;
                }     
                RS_expr += OR; 
            }
            RS_expr =  RS_expr.substr(0, RS_expr.size()-OR.size());

            if (LS_expr != ""){
                cases = "((" + LS_expr + ") -> (" + RS_expr + "))" + AND; 
            }
            else{
                cases = RS_expr + AND;
            }
            trans_file << cases << endl;  
        }
    }


    
    /* TODO: WRITE DEFINE */
    stringstream define_ss;
    for(const auto& elem : define_map)
    {
        string condition = evaluate(condition_tokenizer(elem.second), var_type);
        define_ss << OPEN_PARAN << elem.first << IFF << condition << CLOSE_PARAN << AND << NEWL;
        // << AND << primed(elem.first) << IFF <<  condition << AND << NEWL; //wrong
    }
    trans_file << define_ss.str().substr(0, define_ss.str().size());

    trans_file << "TRUE" << endl; // closing R.bool
    trans_file.close();    

    return 0;
}





// vector<vector<string> > cart_product_2 (const vector<vector<string> >& v) {
//     vector<vector<string>> s = {{}};
//     for (const auto& u : v) {
//         vector<vector<string>> r;
//         for (const auto& x : s) {
//             for (const auto& y : u) {
//                 r.push_back(x);
//                 r.back().push_back(y);
//             }
//         }
//         s = std::move(r);
//     }
//     return s;
// }