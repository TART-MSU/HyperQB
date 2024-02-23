#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>




using namespace std;



//The function for finding indexes of a text
vector<int> substrPosition(string str, string sub_str) {
   bool flag = false;
   
   vector<int> arr;
   
   for (int i = 0; i < str.length(); i++) {
      if (str.substr(i, sub_str.length()) == sub_str) {
                        
         
         arr.push_back(i);
         
         
         
         flag = true;
      }
   }

   return arr;
}





//The function for replacing -> existing in the output by the equal logical relations

string if_replacer(string input){
       

               string sub_str;
               int distance;
       
               sub_str = "->";
             
               vector<int> positions_if = substrPosition(input, sub_str);
               
               sub_str = "(";
               
               vector<int> positions_p1 = substrPosition(input, sub_str);
               
               sub_str = ")";
               
               vector<int> positions_p2 = substrPosition(input, sub_str);

               int position=0;
               int added = 0;
               
               for (int i = 0; i<positions_if.size(); i++){

                    distance = input.length();
                    for (int j = 0; j<positions_p1.size(); j++){
                        
                        if ((positions_if[i]-positions_p1[j])<distance && (positions_if[i]-positions_p1[j])>0){
                                                                        
                                   position = positions_p1[j];  
                                   distance = positions_if[i]-positions_p1[j];                                 
                                                                        
                                 } else {
                                        break;
                                        }
                        
                        }
                        
                    input.insert(positions_if[i]+added, "\\/");
                    input.insert(position+1+added, "~");
                    
                    added += 3;
       
                     
                    
                    }
                    
         


         for (int i = 0; i<input.length(); i++){
            
            string s = "->";

            string::size_type x = input.find(s);
 
            if (x != string::npos)
              input.erase(x, s.length());
      
                }
                
                
                
          return input;
  
       }
       
       
       
       
       
       
       
       
       
       
       
//The function for replacing <-> existing in the output by the equal logical relations

string iff_replacer(string input){
       
       
               string sub_str;
               int distance;

               sub_str = "<->";
               
               vector<int> positions_iff = substrPosition(input, sub_str);
               
               sub_str = "(";
               
               vector<int> positions_p1 = substrPosition(input, sub_str);
               
               sub_str = ")";
               
               vector<int> positions_p2 = substrPosition(input, sub_str);
            
             
               
               int position1;
               int position2;
               int added;

               
               string part1;
               string part2;
               string added_part;
               
               added = 0;
                   
               for (int i = 0; i<positions_iff.size(); i++){
                   
                    distance = input.length();
                   
                    position1 = 0;
                    position2 = 0;
                    part1 = "";
                    part2 = "";
                    
                    for (int j = 0; j<positions_p1.size(); j++){
                        
                        if ((positions_iff[i]-positions_p1[j])<distance && (positions_iff[i]-positions_p1[j])>0){
                                                                        
                                   position1 = positions_p1[j];  
                                   distance = positions_iff[i]-positions_p1[j];                                 
                                                                        
                                 } else {
                                        break;
                                        }
                        
                        }
                        
                    distance = input.length(); 

                        
                    for (int j = 0; j<positions_p2.size(); j++){
    
                        
                        if ((-positions_iff[i]+positions_p2[j])>0){
                                                                        
                                   position2 = positions_p2[j];  
     
                                   break;
                               }                                     

                        }
  
                        
                    for (int j = position1+added+1; j<positions_iff[i]+added; j++){
                        
                        part1 += input[j];
                        input[j] = '@';
                        
                        
                        }
 
                        
                    for (int j = added+positions_iff[i]+3; j<position2+added; j++){
                        
                        part2 += input[j];
                        input[j] = '@';
                        
                        
                        }
 
                         
                    added_part += "(";
                    added_part += "~";
                    added_part +=part1;
                    added_part +="\\/";
                    added_part +=part2;
                    added_part +=")";
                    added_part +="/\\";
                    added_part +="(";
                    added_part +="~";
                    added_part +=part2;
                    added_part +="\\/";
                    added_part +=part1;
                    added_part +=")";


                        
                    input.insert(position1+added+1, added_part);
                    
                    added += added_part.length();
                    added_part = "";

                    }
                    
                    
         for (int i = 0; i<input.length(); i++){
            
            string s = "@";

            string::size_type x = input.find(s);
 
            if (x != string::npos)
              input.erase(x, s.length());
              
            s = "<->";

            x = input.find(s);
 
            if (x != string::npos)
              input.erase(x, s.length());

             
                }
                
                
       return input;

       
       }
       
       
       
       
       
int main(){
    
    
    string input = "(modelA<->modelB)\\/(modelC->modelD)/\\(modelE<->modelF)";
    
    string output = iff_replacer(input);
    output = if_replacer(output);
    
    cout<<output;

    return 0;
    
    
    
}
