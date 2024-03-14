#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <stack>
#include <algorithm>
#include <map>
#include <sstream>
#include <regex>
#include <stdio.h>
// #include <boost/algorithm/string/replace.hpp>
using namespace std;

map<pair<int, int>, int> gate_map;


// // A helper method
void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
	     subject.replace(pos, search.length(), replace);
	     pos += replace.length();
	}
}

// //THH updated

//The function for unrolling initial conditions
string I_unroller(int k, string I_file, string model_type)
{
	bool isAP = false;
	string suffix = "_" + model_type + "_[0]";
	string init;
	std::ifstream file(I_file);
	if (!file.eof()) {
	    std::string line;
	    while (std::getline(file, line)) {
				for (int i = 0 ; i < line.length() ; i++){
					switch (line[i]) {
						case '(':
							init += '(';
						break;
						case ')':
							if(isAP){
								init += suffix+")";
								isAP = false;
							}
							else{
								init += ')';
							}
						break;

						case '/':
							if (isAP){
						  	init += suffix+"/";
								isAP = false;
							}
							else{
								init += "/";
							}
						break;

						case '\\':
							if (isAP){
								init += suffix+"\\";
							}
							else{
								init += "\\";
							}
						break;

						default:
							isAP = true;
							init += line[i];
						break;
						}
					}
	    }
	    file.close();
	}
	return init;
}

string R_unroller(int k, string R_file, string model_type)
{
	bool isAP = false;
	string suffix = "_" + model_type + "_[0]";
	// string post_suffix = "_" + model_type + "_[1]";

	string trans;
	std::ifstream file(R_file);
	if (!file.eof()) {
	    std::string line;
	    while (std::getline(file, line)) {
				for (int i = 0 ; i < line.length() ; i++){
					// cout << endl;
					// cout << line[i];
					switch (line[i]) {
						case '\'':
							suffix = "_" + model_type + "_[1]";
						break;

						case '(':
							trans += '(';
						break;
						case ')':
							if(isAP){
								trans += suffix+")";
								isAP = false;
							}
							else{
								trans += ')';
							}
						break;

						case '/':
							if (isAP){
						  	trans += suffix+"/";
								isAP = false;
							}
							else{
								trans += "/";
							}
						break;

						case '\\':
							if (isAP){
								trans += suffix+"\\";
							}
							else{
								trans += "\\";
							}
						break;

						default:
							isAP = true;
							suffix = "_" + model_type + "_[0]";
							trans += line[i];
						break;
						}
					}
	    }
	    file.close();
	}
	// cout << trans << endl;
	trans = trans + "/\\";
	string output = "";
	output += trans;

	string pre;
	string curr;
	string next;
	for (int i = 0; i < k-1; i++){

		pre = "["+to_string(i)+"]";
		curr = "["+to_string(i+1)+"]";
		next = "["+to_string(i+2)+"]";

		ReplaceStringInPlace(trans, curr, next);
		ReplaceStringInPlace(trans, pre, curr);
		output+=trans;
 	}
	return output;
}

// Helper method
string attach_time(string expr, int t){
	bool isAP = false;
	string model_name;
	string suffix = "_[" + to_string(t) +"]";
	string timed_formula;
	for (int i = 0 ; i < expr.length() ; i++){
		switch(expr[i]){
			case ('['):
				model_name = (expr[i+1]);
				// model_name.assign(expr[i+1], expr[i+1]);
				// cout << "???" << endl;
				// cout << model_name << endl;
				timed_formula += "_" + model_name + suffix;
				// timed_formula +=  suffix;
				i = i+2; // skip the next 2 charss
			break;
			default:
				timed_formula += expr[i];
			break;
		}
	}
	// cout << suffix << endl;
	// cout << timed_formula << endl;
	return timed_formula;
}

// recursive functions
string rec_F(int k, string expr, string sem){
	string unrolled_formula = "(";
	// cout << expr;
	for (int i = 0 ; i < k ; i++){
		unrolled_formula += attach_time(expr, i) + "\\/";
		if (i == (k-1)){
			if (sem == "PES"){
				unrolled_formula += attach_time(expr, i) + ")";
			}
			else{
				unrolled_formula += attach_time(expr, i) + ")"; //add a FALSE
			}
		}
	}
	return unrolled_formula;
}
string rec_G(int k, string expr, string sem){
	string unrolled_formula = "(";
	for (int i = 0 ; i < k-1 ; i++){
		unrolled_formula += attach_time(expr, i) + "/\\";
		if (sem == "PES"){
			unrolled_formula += attach_time(expr, i) + ")"; //add a TRUE
		}
		else{
			unrolled_formula += attach_time(expr, i) + ")";
		}
	}
	return unrolled_formula;
}
string rec_U(int k, string phi1, string phi2, string sem){
	string unrolled_formula;
	string parans(k, ')');
	for (int i = 0 ; i < k ; i++){
		unrolled_formula += attach_time(phi2, i) + "\\/ (" + attach_time(phi1, i) + "/\\";
		if (i == (k-1)){
			if (sem == "PES"){
				unrolled_formula += attach_time(phi2, k) + parans;
			}
			else{
				unrolled_formula += attach_time(phi1, k) + parans;
			}
		}
	}
	return unrolled_formula;
}
string rec_R(int k, string phi1, string phi2, string sem){
	string unrolled_formula;
	string parans(k, ')');
	for (int i = 0 ; i < k ; i++){
		unrolled_formula += attach_time(phi2, i) + "/\\ (" + attach_time(phi1, i) + "\\/";
		if (i == (k-1)){
			if (sem == "PES"){
				unrolled_formula += attach_time(phi1, k) + parans;
			}
			else{
				unrolled_formula += attach_time(phi2, k) + parans;
			}
		}
	}
	return unrolled_formula;
}


// THH: edit here
string formula_unroller(int k, string prop, string sem)
{
	int L_ptr = 0;
	int R_ptr = 0;
	string phi1;
	string phi2;
	for (int i = 0; i < prop.length(); i++){
		switch (prop[i]) {
			case ('F'):
				// cout << "eventually" << endl;
			 	L_ptr = i+1;
				R_ptr = prop.find_last_of(')');
				prop = rec_F(k, prop.substr(L_ptr, R_ptr), sem);
				// cout << L_ptr << endl;
				// cout << rec_F(k, prop.substr(L_ptr, R_ptr)) << endl;
			break;
			case ('G'):
				// cout << "always" << endl;
				L_ptr = i+1;
				R_ptr = prop.find_last_of(')');
				prop = rec_G(k, prop.substr(L_ptr, R_ptr), sem);
				// cout << L_ptr << endl;
				// cout << rec_G(k, prop.substr(L_ptr, R_ptr)) << endl;
			break;
			case ('U'):
				phi1 = prop.substr(0, i-1);
				phi2 = prop.substr(i+1, prop.length());
				prop = rec_U(k, phi1, phi2, sem);
				// cout << "until" << endl;
				// cout << phi1 << endl;
				// cout << phi2 << endl;
				// cout << rec_U(k, phi1, phi2, "-pes");
			break;
			case ('R'):
			phi1 = prop.substr(0, i-1);
			phi2 = prop.substr(i+1, prop.length());
			prop = rec_R(k, phi1, phi2, sem);
			// cout << "release" << endl;
			// cout << rec_R(k, phi1, phi2, "-pes");
			break;
		}
	}
	// int L_pointer = prop.find('(');
	// int R_pointer = prop.find_last_of(')');
	// cout << L_ptr << endl;
	// cout << R_ptr << endl;
	return prop;
}


// Lilly & Tess
void write_quantifiers (vector<char> const &quantifier, map<string, int> const &var_map, ofstream &my_file) {
    int len = quantifier.size();
    char letter = 'A';
    // stringstream ss;
		string s;
    for (int i=0; i<len; i++) {
        if (quantifier.at(i)=='A') {
            // ss << "forall(" ;
						s += "forall(";
        } else {
            // ss << "exists(" ;
						s += "exists(";
        }
        string identifier = "_";
        identifier.push_back(letter);
        for (auto x : var_map ) {
            if (x.first.find(identifier) != string::npos){
                // ss << x.second << ",";
								s += to_string(x.second) + ",";
            }
        }
        // string string_ss = ss.str();
        // my_file << string_ss.substr(0,string_ss.length()-1) << ")" << endl;
				my_file << s.substr(0, s.size()-1) << ")" << endl;
				s = "";
        // ss.str(std::string());
        letter++;
    }
}

pair<int, int> find_last_vars(string formula) {
    int index1 = formula.find_last_of("(");
    int index2 = formula.substr(0,index1-1).find_last_of("(");
    pair<int, int> index_pair(index2, index1);
    return index_pair;
}

ofstream my_file("build_today/HQ.qcir");
string write_to_file (map<string, int> &formula_map,int &count, stack<string> &s, string &prefix, pair<int, int> &index_pair){
    // stringstream ss;
		string ss_string;
    string formula;
    // ss << to_string(count) << " = ";
		ss_string += to_string(count) + " = ";
    if (s.top() == "/\\") { // or operator
        formula += "or(";
    } else if (s.top() == "\\/") { // and operator
        formula += "and(";
    }
    index_pair = find_last_vars(prefix);
    formula += prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2)) + "," + prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) + ")";

    //check if formula already in ss
    for (auto x : formula_map ) {
        if (x.first==formula){
            prefix = prefix.substr(0,index_pair.first) + '(' + to_string(x.second) + ')';
            return "";
        }
    }
    formula_map[formula] = count;

    // ss << formula << endl;

		ss_string += formula + "\n";

    prefix = prefix.substr(0,index_pair.first) + '(' + to_string(count) + ')';
    count++;

    // return ss.str();
		// my_file << ss_string;
		return ss_string;
}

bool isOperator(string c){
    if (c == "/\\" || c == "\\/") {
        return true;
    }
    else {
        return false;
    }
}

bool isOperator2(string c){
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
    map<string, int> formula_map;
    // ofstream my_file;
    // stringstream ss;
		string ss_string; // THH: let's try using string

		// my_file.open("build_today/HQ-test.qcir");
    // my_file.open("build_today/HQ-cpp.qcir");
    string prefix;
    string variable;


    reverse(infix.begin(), infix.end());

		string prec;
		int index1;

    for (int i = 0; i < infix.length(); i++) {

				// THH: it's the same change them here right?
				if (infix[i] == '(') {
						infix[i] = ')';
				}
				else if (infix[i] == ')') {
						infix[i] = '(';
				}


        if (infix[i]=='~') {
            //account for negation
            prefix += infix[i];
            size_t pos;
            while ((pos = prefix.find(")~")) != std::string::npos) {
                size_t pos2 = prefix.substr(0,pos).find_last_of("(");
                prefix.replace(pos, 2, ")");
                prefix.insert(pos2+1, "-");
            }
        // adding variable to prefix
        }
				else if (infix[i] != '/' && infix[i] != '\\' && infix[i] != '~' && !isPar(infix.substr(i,1))){
            // assign numbers to variables here
            variable += infix[i];
            // end of variable due to operator or parentheses
            if ((infix.length() >= (i+1) && (infix[i+1] == '/' || infix[i+1] == '\\' || infix[i+1] == '~' || isPar(infix.substr(i+1,1))))||(infix.length()-1) == i) {
                // reverse(variable.begin(), variable.end());
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
            // string s1 = infix[i];
						string s1(1, infix[i]);
            s.push(s1);
        }
        else if (infix[i] == ')') {
            while ((s.top() != "(") && (!s.empty())) {
                // assign number to formula and write to file
                // ss << write_to_file (formula_map, count, s, prefix, index_pair);
								ss_string += write_to_file (formula_map, count, s, prefix, index_pair);
                s.pop();
            }
            if (s.top() == "(") {
                s.pop();
            }
        }
        else if (isOperator(infix.substr(i,2)) || isOperator2(infix.substr(i,1))) {
				// else if (infix[i] == '/' || infix[i] == '\\' || infix[i] == '~') {
            // int index1;
            if (isOperator(infix.substr(i,2))) {
                index1 = 2;
            } else {
                index1 = 1;
            }
            if (s.empty()) {
                s.push(infix.substr(i,index1));
            }
            else {
								prec = infix.substr(i,index1);
                if (precedence(prec) > precedence(s.top())) {
                    s.push(prec);
                }
                else if (precedence(prec) == precedence(s.top())) {
                    s.push(prec);
                }
                else {
                    while ((!s.empty()) && (precedence(prec) < precedence(s.top()))) {
                        // assign number to formula and write to file
                        // ss << write_to_file (formula_map, count, s, prefix, index_pair);
												ss_string += write_to_file (formula_map, count, s, prefix, index_pair);
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
        // ss << write_to_file (formula_map, count, s, prefix, index_pair);
				ss_string += write_to_file (formula_map, count, s, prefix, index_pair);
        s.pop();
    }

    // quantifiers
    write_quantifiers (quantifier, var_map, my_file);
    // output and variables
    my_file << "output" << prefix << '\n' << endl;
    // write formulas to file
    // my_file << ss.str() << endl;
		my_file << ss_string;
    // new map so variables can be sorted by number in qcir file
    map<int, string> new_map;
		my_file << "# variables" << endl;
    for (auto x: var_map) {
        // new_map[x.second] = x.first;
				// my_file << "#  " << x.first << " " << x.second << endl;
				// var_map[x] = reverse(x.first.begin(), x.first.end());
				my_file <<  "#  " << x.second << ": "<< x.first  << endl;
				// my_file <<  "#  " << x.second << ": "<< (x.first.begin(), x.first.end())  << endl;
				// reverse(variable.begin(), variable.end());
    }
    // for (auto x: new_map) {
    //     my_file << "#  " << x.first << " " << x.second << endl;
    // }

		//// debug


    my_file.close();
}



map<string, int> variable_map;
int global_counter = 1;
string final_QCIR;

string numerize_vars(string var_name){
	if (!variable_map.count(var_name)){
		variable_map[var_name] = ++global_counter;
	}
	return to_string(variable_map[var_name]);
  // return "error";
}

void debug(string s){
	cout << s << endl;
}

int build_AND(string expr){
	debug("building: " + expr);
	string var_name = "";
	string final_QCIR = "";
	final_QCIR += to_string(global_counter) + "=(";
	for (int i = 0 ; i < expr.length() ; i++){
		if (expr[i] == ')'){
			final_QCIR += numerize_vars(var_name);
			final_QCIR += ')';
		}
		else if (expr[i] == '~'){
			final_QCIR += "-";
		}
		else if (expr[i] != '/'){
			var_name += expr[i];
			debug(var_name);
		}
		else{
			final_QCIR += numerize_vars(var_name) + ",";
			var_name = "";
			i = i + 1;
		}

	}

	// final_QCIR += to_string(global_counter) + "=(" + var_name + ")";

	debug("final: "+final_QCIR);
	global_counter ++;

	return 999;
}

int build_OR(string expr){
	debug("building: " + expr);
	string var_name = "";
	string final_QCIR = "";
	final_QCIR += to_string(global_counter) + "=(";
	for (int i = 0 ; i < expr.length() ; i++){
		if (expr[i] == ')'){
			final_QCIR += numerize_vars(var_name);
			final_QCIR += ')';
		}
		else if (expr[i] == '~'){
			final_QCIR += "-";
		}
		else if (expr[i] != '\\'){
			var_name += expr[i];
			debug(var_name);
		}
		else{
			final_QCIR += numerize_vars(var_name) + ",";
			var_name = "";
			i = i + 1;
		}

	}

	// final_QCIR += to_string(global_counter) + "=(" + var_name + ")";

	debug("final: "+final_QCIR);
	global_counter ++;

	return 999;
}

// A binary version
// int BoolToQCIR(string formula, int L_ptr, int R_ptr ){
int BoolToQCIR(string formula, int L_ptr, int R_ptr){

	string L_expr;
	string R_expr;
	// int L_ptr = formula[0];
	// int R_ptr = formula[formula.length()];

	if(formula[L_ptr+1] == '(' && formula[R_ptr-1] == ')'){
		BoolToQCIR(formula, L_ptr+1, R_ptr-1);
	}
	else{
		// for (int k = L_ptr; k < formula.length(); k++){
			// cout << "[last layer, assigning numbers]" << endl;
			// if (k == formula.find(")")){ //
				if (formula[L_ptr+2] == '/'){
					cout << "build AND: " << endl;
					build_AND(formula.substr(1, R_ptr-1)); // also remove the parans
				}
				else if (formula.find("\\")){
					build_OR(formula.substr(1, R_ptr-1));

				}
			// }
		// }
	}
	for (auto x: variable_map) {
		 cout << "#  " << x.second << ": "<< x.first  << endl;
	}
	return 1;

}

// string test_formula = "((a/\\b)\\/(c/\\d))";

//Min genqbf function to run
int main(int argc, char **argv)
{
	// clock_t start, end;
	// double time_taken;
	vector<char> quantifier;
	bool test = false;

	// cout << "You have entered " << argc << " arguments:"
       // << "\n";
  // for (int i = 0; i < argc; ++i)
      // cout << argv[i] << "\n";
	int k = atoi(argv[1]);
	// cout << "???" << k << "\n";
	// exit(1);

	vector<string> inputs;
	string final_check;
	string variable;
	int counter = 2;
	string model_names[26] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

	string SEM = (argv[2]);
	string QS = (argv[3]);
	string infix_formulas;
	string FORMULA;



	for (int i = 4; i < argc; i+=2)
	{
		if (i == (argc-1)){
			// last argument is formula
			FORMULA=argv[i];
			break;
		}
		else{
			string I_file = argv[i];
			string R_file = argv[i+1];
			string unrolled_I = I_unroller(k, I_file, model_names[i/2]);
			string unrolled_R = R_unroller(k, R_file, model_names[i/2]);
			infix_formulas = infix_formulas + "(" +unrolled_I + ")" + "/\\" + "(" + unrolled_R + ")";
		}
	}





	// FORMULA
	// int last_dot = prop.find_last_of('.');
	// string quants = prop.substr(0, last_dot+1);
	// regex re("([A-Z].)");
	//
	// for (int i = 0 ; i < quants.length() ; i++){
	// 	if (quants[i] == 'e'){ // 'exists'
	// 			quantifier.push_back('E');
	// 	}
	// 	if (quants[i] == 'f'){ // 'forall'
	// 			quantifier.push_back('A');
	// 	}
	// }


	// string semantic = inputs[inputs.size() - 3];
	// string QS = inputs[inputs.size() - 2];
	// string prop = inputs[inputs.size() - 1];


	// FORMULA !!!!
	string prop;
	std::ifstream file(FORMULA);
	while (!file.eof())
	{
		std::string line;
			while (std::getline(file, line)) {
			for (int i = 0 ; i < line.length() ; i++){
				prop += line[i];
			}
		}
	}
	int last_dot = prop.find_last_of('.');
	prop = prop.substr(last_dot+1, prop.length());
	string unrolled_formula = formula_unroller(k+1, prop, SEM);
	unrolled_formula = "(" + unrolled_formula + ")"; // warning: here has a hidden
	infix_formulas = infix_formulas+unrolled_formula;


	// ofstream bool_output("build_today/output.txt");
	// bool_output << infix_formulas;


	// GATE BUILDING
	map<string, int> var_map;
	stack<string> stack;
	string line, input_file;
	string QCIR_out = "build_today/HQ.qcir";
	InfixToQCIR(stack, infix_formulas , var_map, quantifier);


	// cout << infix_formulas << endl;
	// exit(1);

	return 0;
}
