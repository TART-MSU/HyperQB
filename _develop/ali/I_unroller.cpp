#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>


using namespace std;

//finding indexes of a text
vector<int> substrPosition(string str, string sub_str) {
   bool flag = false;
   
   vector< int > arr;
   
   for (int i = 0; i < str.length(); i++) {
      if (str.substr(i, sub_str.length()) == sub_str) {
                        
         //cout << i << " ";
         arr.push_back(i);
         
         
         
         flag = true;
      }
   }
   //if (flag == false)
      //cout << "NONE";
   
   return arr;
}


//Recreating genqbf using C++

int main(int argc, char** argv) {

	//step1: unrolling the model

	//Unrolling the inital condition

    
        
    char* a = argv[1];
    char* b = argv[2];
    
   
	
	int k;
	int bit;
	string I_file;
	k = atoi(a);
	bit = atoi(b);
	string ad_I = "";
	string I = "";
	I_file = argv[3];

	fstream  mini_I;
	mini_I.open(I_file, ios::in);
	
	while(!mini_I.eof()){
                     
             string added;
                     
             mini_I >> added;
             I += added;
             I += " ";  
                     
                     }

   I = I.substr(0, I.size()-1);
	
	
	
	
	
	
	

	for (int i = 0; i < bit; i++) {

		string s = to_string(i);

		if (i == 0) {

			ad_I += "(";

		}

		ad_I += "~n_";
		ad_I += s;

		if (i != bit-1) {

			ad_I += "/\\";

		}


		if (i == bit-1) {

			ad_I += ")/\\";

		}

	}

//	mini_I >> I;


	char* I_str = &I[0];
	char added_str[6] = { '_','A','_','[','0',']' };
	
	int added_l = end(added_str)-begin(added_str);
	
	string new_I = I_str;
	

	
//	int len = new_I.length();
	
 
   string sub_str = "/\\";
   
   vector<int> positions = substrPosition(new_I, sub_str);

 
   int counter = 0;
 
   for (int i = 0; i<positions.size(); i++){
        
         //cout<<positions[i];
         
         if (new_I[positions[i]+counter-1] != ')'){
            new_I.insert(positions[i]+counter, added_str);
            
            counter += added_l;
            }
            
         
        
        }
        
   
   
   sub_str = "\\/";
   
   positions = substrPosition(new_I, sub_str);

 
   counter = 0;
 
   for (int i = 0; i<positions.size(); i++){
        
         //cout<<positions[i];
         
         if (new_I[positions[i]+counter-1] != ')'){
            new_I.insert(positions[i]+counter, added_str);
            
            counter += added_l;
            }
            
         
        
        }

 
        
   sub_str = ")";
   
   positions = substrPosition(new_I, sub_str);

 
   counter = 0;
 
   for (int i = 0; i<positions.size(); i++){
        
         //cout<<positions[i];
         if (new_I[positions[i]+counter-1] != ')'){
            new_I.insert(positions[i]+counter, added_str);
            
            counter += added_l;
            }
         
        
        }
        

 //cout<<new_I;      
 
 


    
//	ad_I += new_I;

	
	
    cout<<"This is Unrolled I:\n"<<new_I;	





	return 0;

}
