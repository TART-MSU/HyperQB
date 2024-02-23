#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>


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

	


    
        
    char* a = argv[1];
    char* b = argv[2];
    
   
	
	int k;
	int bit;
	string R_file;
	k = atoi(a);
	bit = atoi(b);
	string ad_R = "";
	string R = "";
	R_file = argv[3];

	fstream  mini_R;
	mini_R.open(R_file);
	
	

//	mini_R >> R;


while(!mini_R.eof()){
                     
             string added;
                     
             mini_R >> added;
             R += added;
             R += " ";  
                     
                     }
//R -= " ";
//R.pop_back();
R = R.substr(0, R.size()-1);
	
cout<<R<<"\n";

char* R_str = &R[0];
string new_R = R_str;


//	char* R_str = &R[0];
	char added_str[6] = { '_','A','_','[','0',']' };
	
	vector<string> added_strs;
	
	for (int i = 0; i<k; i++){
        
        string s = to_string(i);
        char const *pchar = s.c_str();
        
        added_str[4] = *pchar;
        
    //    cout<<added_str;
          
        
        added_strs.push_back(added_str);
        
        }
        
        
        
        
	
	int added_l = end(added_str)-begin(added_str);
	
//	string new_R = R_str;

    	
	string final_R;
	
	

	
//	int len = new_I.length();

   string sub_str;
   vector<int> positions;
   int counter;
   int i;
   
   
   
   sub_str = ")";
               
               positions = substrPosition(new_R, sub_str);
            
             
               counter = 0;
               
               
  
    
    for (int t = 0; t<k-1; t++){ 
           

     
               sub_str = "/\\";
               
               positions = substrPosition(new_R, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                     //cout<<positions[i];
                     
                     if (new_R[positions[i]+counter-1] != ')'){
                         if (new_R[positions[i]+counter-1] == '\''){
                        
                        new_R.insert(positions[i]+counter, added_strs[t+1]);
                        }
                        else{
                             new_R.insert(positions[i]+counter, added_strs[t]);
                             }
                             counter += added_l;
                        }
                        
                     
                    
                    }
                    
       //        cout<<positions.size()<<"\n";
               
               sub_str = "\\/";
               
               positions = substrPosition(new_R, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                     //cout<<positions[i];
                     
                     if (new_R[positions[i]+counter-1] != ')'){
                         if (new_R[positions[i]+counter-1] == '\''){
                        
                        new_R.insert(positions[i]+counter, added_strs[t+1]);
                        }
                        else{
                             new_R.insert(positions[i]+counter, added_strs[t]);
                             }
                             counter += added_l;
                             }
                             
                        
                        }
            
      //       cout<<positions.size()<<"\n";
                    
               sub_str = ")";
               
               positions = substrPosition(new_R, sub_str);
            
             
               counter = 0;
               
               
                  
//The problem is something with positions )) at the end of the string             
               for (int i = 0; i<positions.size(); i++){
                    
 //                    cout<<positions[i]<<endl;
                     if (new_R[positions[i]+counter-1] != ')'){
                         if (new_R[positions[i]+counter-1] == '\''){
                        
                        new_R.insert(positions[i]+counter, added_strs[t+1]);
                        }
    
                        else{
                             
                        
                                 new_R.insert(positions[i]+counter, added_strs[t]);
                             }
                             counter += added_l;
                            }
                        
                        }
                        
                        
 //              sub_str = " -> ";
  
//               cout<<positions.size()<<"\n";
               
  //             positions = substrPosition(new_R, sub_str);
            
             
  //             counter = 0;
             
   //            for (int i = 0; i<positions.size(); i++){
                    
                     //cout<<positions[i];
  //                   if (new_R[positions[i]+counter-1] != ')'){
  //                       if (new_R[positions[i]+counter-1] == '\''){
                        
  //                      new_R.insert(positions[i]+counter, added_strs[t+1]);
  //                      }
   //                     else{
  //                           new_R.insert(positions[i]+counter, added_strs[t]);
  //                           }
 //                          counter += added_l;  
 //                            }
  //                      
                        
  //                      }
                    
               
               
//               sub_str = " ";
               
//               positions = substrPosition(new_R, sub_str);
            
             
 //              counter = 0;
             
  //             for (int i = 0; i<positions.size(); i++){
                    
                     //cout<<positions[i];
  //                   if (new_R[positions[i]+counter-1] != ')'){
  //                       if (new_R[positions[i]+counter-1] == '\''){
                        
 //                       new_R.insert(positions[i]+counter, added_strs[t+1]);
  //                      }
 //                       else{
 //                            new_R.insert(positions[i]+counter, added_strs[t]);
 //                            }
  //                           counter += added_l;
 //                            }
 //                       
                        
 //                       }
                        
                
                        
       final_R += new_R; 
       new_R = R_str;
                    
       if (t != k-2){
         final_R += "/\\";
         }
 
 
     
     }
        
        
        
        

    
//	ad_R += new_R;

    final_R.erase(remove(final_R.begin(), final_R.end(), '\''), final_R.end());

	
	
    cout<<"This is Unrolled R:\n"<<final_R;	





	return 0;

}
