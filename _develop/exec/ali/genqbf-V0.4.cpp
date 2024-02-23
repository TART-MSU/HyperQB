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
   
   vector< int > arr;
   
   for (int i = 0; i < str.length(); i++) {
      if (str.substr(i, sub_str.length()) == sub_str) {
                        
         
         arr.push_back(i);
         
         
         
         flag = true;
      }
   }

   return arr;
}








//The function for until unroller

string until_unroller(string modelA, string modelB, int k, vector<string> added_strs_A, vector<string> added_strs_B, string status){
       
       
       
       string final_return;
       
       string new_A = modelA;
       string new_B = modelB;
       string paranthesis;
       
       string P_nextgen;
       string sub_str;
       int counter;
       vector<int> positions;
       
       
       for (int t = 0; t < k+1; t++){
           
           
           
       //    new_B.insert(new_B.length(), added_strs_B[i]);
       //    new_A.insert(new_A.length(), added_strs_A[i]);
           int added_l = end(added_strs_A[t])-begin(added_strs_A[t]);
           P_nextgen = "";
           
           
           if (t<k-1){
           
           P_nextgen += new_B;
           P_nextgen += "\\/";
           P_nextgen += "(";
           P_nextgen += new_A;
           P_nextgen += "/\\";
           P_nextgen += "(";
           
           paranthesis += "))";
           
           
           //The new algorithm starts right there**********************************************************************************
           sub_str = "/\\";
             
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                
                        
       final_return += P_nextgen; 
           
           
           
           
           
           }
           
           else if (t==k-1){
                
                P_nextgen = "";
                
                P_nextgen += new_B;
                P_nextgen += "\\/";
                
                P_nextgen += new_A;
                P_nextgen += "/\\";
                
                
               sub_str = "/\\";
             
                positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                
                        
       final_return += P_nextgen; 
                
                
                
                
                
                }
           
           else if (t==k){
                
                P_nextgen = "";

                if (status == "-pes"){
                
                P_nextgen += new_B;
                
                } else if (status == "-opt"){
                       
                       
                       P_nextgen += "(";
                       P_nextgen += new_A;
                       P_nextgen += "\\/";
                       P_nextgen += new_B;
                       P_nextgen += ")";
                       
                       
                       }
                P_nextgen += paranthesis;
                
                sub_str = "/\\";
             
                positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                
                
                
                
                
                
                final_return += P_nextgen;
                
                
                }
             
           
           
           }
           
       
       return final_return;
       
       
       }









//The function for release unroller

string release_unroller(string modelA, string modelB, int k, vector<string> added_strs_A, vector<string> added_strs_B, string status){
       
       
       
       string final_return;
       
       string new_A = modelA;
       string new_B = modelB;
       string paranthesis;
       
       string P_nextgen;
       string sub_str;
       int counter;
       vector<int> positions;
       
       
       for (int t = 0; t < k+1; t++){
           
           
           
       //    new_B.insert(new_B.length(), added_strs_B[i]);
       //    new_A.insert(new_A.length(), added_strs_A[i]);
       
           int added_l = end(added_strs_A[t])-begin(added_strs_A[t]);
           P_nextgen = "";
           
           
           if (t<k-1){
           
           P_nextgen += new_B;
           P_nextgen += "/\\";
           P_nextgen += "(";
           P_nextgen += new_A;
           P_nextgen += "\\/";
           P_nextgen += "(";
           
           paranthesis += "))";
           
           
           //The new algorithm starts right there**********************************************************************************
           sub_str = "/\\";
             
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                
                        
       final_return += P_nextgen; 
           
           
           
           
           
           }
           
           else if (t==k-1){
                
                P_nextgen = "";
                
                P_nextgen += new_B;
                P_nextgen += "/\\";
                
                P_nextgen += new_A;
                P_nextgen += "\\/";
                
                
               sub_str = "/\\";
             
                positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                
                        
       final_return += P_nextgen; 
                
                
                
                
                
                }
           
           else if (t==k){
                
                P_nextgen = "";

                
                if (status == "-pes"){
                           
                       P_nextgen += "(";
                       P_nextgen += new_A;
                       P_nextgen += "/\\";
                       P_nextgen += new_B;
                       P_nextgen += ")";
                       
                       } else if (status == "-opt"){
                              
                       P_nextgen += new_B;
                
                }
                
                P_nextgen += paranthesis;
                
                sub_str = "/\\";
             
                positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                
                
                
                
                
                
                final_return += P_nextgen;
                
                
                }
             
           
           
           }
           
       
       return final_return;
       
       
       }













//The function for unrolling G

string G_unroller(int k, int position_G, string new_P, vector<string> added_strs_A, vector<string> added_strs_B){
       
       
          string P_nextgen = "";
          string end_sign = "";
          string P_reference;
          string final_P;
          string sub_str;
          
          
          
          
          
          vector<int> positions;
          int counter;
          
          
          
         
          
          
          
             
               for (int i = position_G+1; i<new_P.length(); i++){
                   
                   if (new_P[i] == 'F' || new_P[i] == 'G'){
                                if (new_P[i+1] == '(' || new_P[i+1] == '~'){
                                
                            
                                
                                end_sign = P_nextgen[P_nextgen.length()-2];
                                end_sign += P_nextgen[P_nextgen.length()-1];
                                
                                P_nextgen = P_nextgen.substr(0, P_nextgen.size()-1);
                                P_nextgen = P_nextgen.substr(0, P_nextgen.size()-1);
                                
                                 break;
                                 }
                                
                                }
                   
                   P_nextgen += new_P[i];
                     
                    
                    } 
                    
                    P_reference = P_nextgen; 
                     
   
    
        for (int t = 0; t<k; t++){ 
            
            
                int added_l = end(added_strs_A[t])-begin(added_strs_A[t]);

     
                sub_str = "/\\";
             
                positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                
                        
       final_P += P_nextgen; 
       P_nextgen = P_reference;
                    
       if (t != k-1){
         final_P += "/\\";
         }
 
 
     
     }
     
     final_P += end_sign;
    
 
       
       
     return final_P;  
       
       
       
       
       
       
       
}









//The function for unrolling F

string F_unroller(int k, int position_F, string new_P, vector<string> added_strs_A, vector<string> added_strs_B){
       
       
       
          string P_nextgen = "";
          string end_sign = "";
          string P_reference;
          string final_P;
          string sub_str;
          
          
          
          vector<int> positions;
          int counter;
          
          
          
         
          
          
          
             
               for (int i = position_F+1; i<new_P.length(); i++){
                   
                   if (new_P[i] == 'F' || new_P[i] == 'G'){
                                if (new_P[i+1] == '(' || new_P[i+1] == '~'){
                                
                            
                                
                                end_sign = P_nextgen[P_nextgen.length()-2];
                                end_sign += P_nextgen[P_nextgen.length()-1];
                                
                                P_nextgen = P_nextgen.substr(0, P_nextgen.size()-1);
                                P_nextgen = P_nextgen.substr(0, P_nextgen.size()-1);
                                
                                 break;
                                 }
                                
                                }
                   
                   P_nextgen += new_P[i];
                     
                    
                    } 
                    
                    P_reference = P_nextgen; 
                     
   
    
        for (int t = 0; t<k; t++){ 
            
               
               
               int added_l = end(added_strs_A[t])-begin(added_strs_A[t]);
           

     
                sub_str = "/\\";
             
                positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                                                           
                             if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }               
                             
                        }
                        
                     
                    
                    }
                    
              
               
               sub_str = "\\/";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
             
               for (int i = 0; i<positions.size(); i++){
                    
                    
                     
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                         
                              if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                             }
                             
                        
                        }
            
      
                    
              sub_str = ")";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                        
                      
                      
                      
                      
                        
                        
                        
               sub_str = "<->";
               
               positions = substrPosition(P_nextgen, sub_str);
            
             
               counter = 0;
               
               
                  
            
               for (int i = 0; i<positions.size(); i++){
                    
             //        cout<<positions[i]<<endl;
                     if (P_nextgen[positions[i]+counter-1] != ')'){
                        
                                 if (P_nextgen[positions[i]+counter-2] == 'A'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_A[t]);
                                
                                counter += added_l;
                                
                                }
                             else if (P_nextgen[positions[i]+counter-2] == 'B'){
                         
                                P_nextgen.insert(positions[i]+counter, added_strs_B[t]);
                                
                                counter += added_l;
                                
                                }   
                            }
                        
                        }
                        
                        
                
                        
       final_P += P_nextgen; 
       P_nextgen = P_reference;
                    
       if (t != k-1){
         final_P += "\\/";
         }
 
 
     
     }
     
     final_P += end_sign;
    
 
       
       
     return final_P;  
       
       
       
       
       
       
       
}


















//The function for unrolling initial conditions

string I_unroller(int k, string I_file, string model_type) {


	

//	string I_file = I_file;
//	int k = k;
	int bit = 5;
	string ad_I = "";
	string I = "";


	fstream  mini_I;
	mini_I.open(I_file, ios::in);
	
	while(!mini_I.eof()){
                     
             string added;
                     
             mini_I >> added;
             I += added;
              
                     
                     }
	
	
	
	
	
	
	

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
	
	
	
	string added_str;
	
	if (model_type == "I"){
                       

           added_str = "_A_[0]";
           
    } else if (model_type == "J") {
           
           
           added_str = "_B_[0]";
         

     
     }  

    
	
	
	
	
	
	
	int added_l = added_str.length();
	
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
        
        
        
        
   sub_str = "<->";
   
   positions = substrPosition(new_I, sub_str);

 
   counter = 0;
 
   for (int i = 0; i<positions.size(); i++){
        
         //cout<<positions[i];
         if (new_I[positions[i]+counter-1] != ')'){
            new_I.insert(positions[i]+counter, added_str);
            
            counter += added_l;
            }
         
        
        }
        


	
	
 //   cout<<"This is Unrolled I:\n"<<new_I;	





	return new_I;

}


















//The function for unrolling translation relations

string R_unroller(int k, string R_file, string model_type) {

	//step1: unrolling the model

	
   
	
//	int k = k;
//	string R_file = R_file;
	string ad_R = "";
	string R = "";


	fstream  mini_R;
	mini_R.open(R_file);
	
	

//	mini_R >> R;


while(!mini_R.eof()){
                     
             string added;
                     
             mini_R >> added;
             R += added;
          
                     
                     }


char* R_str = &R[0];
string new_R = R_str;



	vector<string> added_strs;
	
	
	string A;
	
	if (model_type == "R"){
                       

           A = "_A_[]";
           
    } else if (model_type == "S") {
           
           
           A = "_B_[]";
         

     
     }  
	
	
	string new_A;
	
	for (int i = 0; i<k+1; i++){
        
        string s = to_string(i);

      
        new_A = A;
        
        new_A.insert(4, s);

        added_strs.push_back(new_A);

        }
        
        
        
        
        

	
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
        
        
               int added_l = end(added_strs[t])-begin(added_strs[t]);
           

     
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
               
               
                  
           
               for (int i = 0; i<positions.size(); i++){
                    

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
                        
                        
                        
                        
                        
                        
             sub_str = "<->";
               
               positions = substrPosition(new_R, sub_str);
            
             
               counter = 0;
               
               
                  
           
               for (int i = 0; i<positions.size(); i++){
                    

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
                        
                        

                        
                
                        
       final_R += new_R; 
       new_R = R_str;
                    
       if (t != k-2){
         final_R += "/\\";
         }
 
 
     
     }
        
        
        
        

    
//	ad_R += new_R;

    final_R.erase(remove(final_R.begin(), final_R.end(), '\''), final_R.end());

	
	
 //   cout<<"This is Unrolled R:\n"<<final_R;	





	return final_R;

}










































//This is the function for unrolling our formula

string formula_unroller(int k, string P_file, string status) {



    string ad_P = "";
	string P = "";
	
	fstream  mini_P;
	mini_P.open(P_file);
	
	






while(!mini_P.eof()){
                     
             string added;
                     
             mini_P >> added;
             P += added;
        //     P += " ";  
                     
                     }



char* P_str = &P[0];
string new_P = P_str;


//	char* R_str = &R[0];
	
	
	
	

	
	vector<string> added_strs_A;
	vector<string> added_strs_B;
	
	string A = "_A_[]";
	
	string B = "_B_[]";
	
	string new_A;
	
	string new_B;
	

	
	for (int i = 0; i<k+1; i++){
        
        string s = to_string(i);

      
        new_A = A;
        new_B = B;
        
        new_A.insert(4, s);
        new_B.insert(4, s);
          
        
        added_strs_A.push_back(new_A);
        
        
        
        added_strs_B.push_back(new_B);

        
        
        
        }
        
        
        
        
        






        







//Detecting G, F, U, R signs:
            
          vector<int> positions_all;
    
    
    
    
            
          vector<int> positions_G_1;
            
          string sub_str = "G(";
               
          positions_G_1 = substrPosition(new_P, sub_str);
          
          if (positions_G_1.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_G_1.begin(), positions_G_1.end());              
                                      
                                      }
          
          
          vector<int> positions_G_2;
            
          sub_str = "G~";
               
          positions_G_2 = substrPosition(new_P, sub_str);
          
          
          if (positions_G_2.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_G_2.begin(), positions_G_2.end());              
                                      
                                      }
          
          
          
          vector<int> positions_F_1;
            
          sub_str = "F(";
               
          positions_F_1 = substrPosition(new_P, sub_str);
          
          
          if (positions_F_1.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_F_1.begin(), positions_F_1.end());              
                                      
                                      }
          
          
          vector<int> positions_F_2;
            
          sub_str = "F~";
               
          positions_F_2 = substrPosition(new_P, sub_str);
          
          
          if (positions_F_2.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_F_2.begin(), positions_F_2.end());              
                                      
                                      }
          
          
          vector<int> positions_U_1;
            
          sub_str = "]U";
               
          positions_U_1 = substrPosition(new_P, sub_str);
          
          if (positions_U_1.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_U_1.begin(), positions_U_1.end());              
                                      
                                      }
          
          
          vector<int> positions_U_2;
            
          sub_str = ")U";
               
          positions_U_2 = substrPosition(new_P, sub_str);
          
          
          if (positions_U_2.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_U_2.begin(), positions_U_2.end());              
                                      
                                      }
          
          
          vector<int> positions_R_1;
            
          sub_str = "]R";
               
          positions_R_1 = substrPosition(new_P, sub_str);
          
          if (positions_R_1.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_R_1.begin(), positions_R_1.end());              
                                      
                                      }
          
          
          vector<int> positions_R_2;
            
          sub_str = ")R";
               
          positions_R_2 = substrPosition(new_P, sub_str);
          
          
          if (positions_R_2.size()>0){
                                      
            positions_all.insert(positions_all.begin(), positions_R_2.begin(), positions_R_2.end());              
                                      
                                      }
                                      
                                      
          string final_P;
          vector<int> positions;
                                      
                                      
                                      
          if  (positions_all.size()>0){                         
          
          
          sort(positions_all.begin(), positions_all.end());
  
          //cout << "Sorted \n";
          for (auto x : positions_all){
              
              
              
              if (count(positions_G_1.begin(), positions_G_1.end(), x)) {
                                        
                            final_P += G_unroller(k, x, new_P, added_strs_A, added_strs_B);
                        }
              else if (count(positions_G_2.begin(), positions_G_2.end(), x)) {
              
                            final_P += G_unroller(k, x, new_P, added_strs_A, added_strs_B);
                   
                        }
              else if (count(positions_F_1.begin(), positions_F_1.end(), x)) {
                   
                            final_P += F_unroller(k, x, new_P, added_strs_A, added_strs_B);
                        }
                        
              else if (count(positions_F_2.begin(), positions_F_2.end(), x)) {
                   
                            final_P += F_unroller(k, x, new_P, added_strs_A, added_strs_B);
                        }
                        
              else if (count(positions_U_1.begin(), positions_U_1.end(), x)) {
                   
                   
                           sub_str = "(";
               
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           int minimum = new_P.length();
                           int positions_1;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                              if ((x - positions[i])>0 && (x - positions[i])<minimum){
                                                  
                                                  
                                                  
                                                  minimum = x - positions[i];
                                                  
                                                  positions_1 = positions[i];
                                                  
                                                  
                                                  }
                               
                               
                                    }
                                    
                                    
                                    
                           string atom_A;
                           string atom_B;
                           
                           for (int i=positions_1+1; i<x+1; i++){
                               
                               
                               atom_A += new_P[i];
                               
                               
                               
                               }
                        
                           
                                
                          sub_str = ")";
                           
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           
                           
                           minimum = new_P.length();
                           int positions_2;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                               
                               
                              if ((positions[i]-x)>0 && (positions[i]-x)<minimum){
                                                  
                                                  minimum = positions[i]-x;
                                                  
                                                  
                                                  
                                                  positions_2 = positions[i];
                                                  
                                                  
                                                  }
                         
                           
                           
                           }
                           
                           
                           
                          
                           
                           for (int i=x+2; i<positions_2; i++){
                               
                               
                               atom_B += new_P[i];
                               
                               
                               
                              }
                               
                           
                           
                          string output_U =  until_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status); 
                          
                      
                          
                          final_P += output_U;
                          
                          
                          if (new_P[positions_2+1] == '/'){
                                                   
                                                   
                                     final_P.insert(0, "(");              
                                     
                                      final_P += ")";              
                                                   
                                     final_P += "/\\";              
                                                   
                                                   } else if (new_P[positions_2+1] == '\\'){
                                                          
                                                          
                                                          final_P.insert(0, "(");              
                                     
                                                          final_P += ")"; 
                                                          
                                                          final_P += "\\/"; 
                                                          
                                                          
                                                          }
                                    
                                    
                                    
                                    
                                    
                    }
                        
              else if (count(positions_U_2.begin(), positions_U_2.end(), x)) {
                   
                           sub_str = "(";
               
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           int minimum = new_P.length();
                           int positions_1;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                              if ((x - positions[i])>0 && (x - positions[i])<minimum){
                                                  
                                                  
                                                  
                                                  minimum = x - positions[i];
                                                  
                                                  positions_1 = positions[i];
                                                  
                                                  
                                                  }
                               
                               
                                    }
                                    
                                    
                                    
                           string atom_A;
                           string atom_B;
                           
                           for (int i=positions_1+1; i<x+1; i++){
                               
                               
                               atom_A += new_P[i];
                               
                               
                               
                               }
                        
                           
                                
                          sub_str = ")";
                           
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           
                           
                           minimum = new_P.length();
                           int positions_2;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                               
                               
                              if ((positions[i]-x)>0 && (positions[i]-x)<minimum){
                                                  
                                                  minimum = positions[i]-x;
                                                  
                                                  
                                                  
                                                  positions_2 = positions[i];
                                                  
                                                  
                                                  }
                         
                           
                           
                           }
                           
                           
                           
                          
                           
                           for (int i=x+2; i<positions_2; i++){
                               
                               
                               atom_B += new_P[i];
                               
                               
                               
                              }
                               
                           
                           
                          string output_U =  until_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status); 
                          
                      
                          
                          final_P += output_U;
                          
                          
                          if (new_P[positions_2+1] == '/'){
                                                   
                                                   
                                     final_P.insert(0, "(");              
                                     
                                      final_P += ")";              
                                                   
                                     final_P += "/\\";              
                                                   
                                                   } else if (new_P[positions_2+1] == '\\'){
                                                          
                                                          
                                                          final_P.insert(0, "(");              
                                     
                                                          final_P += ")"; 
                                                          
                                                          final_P += "\\/"; 
                                                          
                                                          
                                                          }
                   
                   
                        }
                        
                        
              else if (count(positions_R_1.begin(), positions_R_1.end(), x)) {
                   
                           sub_str = "(";
               
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           int minimum = new_P.length();
                           int positions_1;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                              if ((x - positions[i])>0 && (x - positions[i])<minimum){
                                                  
                                                  
                                                  
                                                  minimum = x - positions[i];
                                                  
                                                  positions_1 = positions[i];
                                                  
                                                  
                                                  }
                               
                               
                                    }
                                    
                                    
                                    
                           string atom_A;
                           string atom_B;
                           
                           for (int i=positions_1+1; i<x+1; i++){
                               
                               
                               atom_A += new_P[i];
                               
                               
                               
                               }
                        
                           
                                
                          sub_str = ")";
                           
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           
                           
                           minimum = new_P.length();
                           int positions_2;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                               
                               
                              if ((positions[i]-x)>0 && (positions[i]-x)<minimum){
                                                  
                                                  minimum = positions[i]-x;
                                                  
                                                  
                                                  
                                                  positions_2 = positions[i];
                                                  
                                                  
                                                  }
                         
                           
                           
                           }
                           
                           
                           
                          
                           
                           for (int i=x+2; i<positions_2; i++){
                               
                               
                               atom_B += new_P[i];
                               
                               
                               
                              }
                               
                           
                           
                          string output_R =  release_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status); 
                          
                      
                          
                          final_P += output_R;
                          
                          
                          if (new_P[positions_2+1] == '/'){
                                                   
                                                   
                                     final_P.insert(0, "(");              
                                     
                                      final_P += ")";              
                                                   
                                     final_P += "/\\";              
                                                   
                                                   } else if (new_P[positions_2+1] == '\\'){
                                                          
                                                          
                                                          final_P.insert(0, "(");              
                                     
                                                          final_P += ")"; 
                                                          
                                                          final_P += "\\/"; 
                                                          
                                                          
                                                          }
                   
                   
                        }
                        
              else if (count(positions_R_2.begin(), positions_R_2.end(), x)) {
                   
                   
                           sub_str = "(";
               
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           int minimum = new_P.length();
                           int positions_1;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                              if ((x - positions[i])>0 && (x - positions[i])<minimum){
                                                  
                                                  
                                                  
                                                  minimum = x - positions[i];
                                                  
                                                  positions_1 = positions[i];
                                                  
                                                  
                                                  }
                               
                               
                                    }
                                    
                                    
                                    
                           string atom_A;
                           string atom_B;
                           
                           for (int i=positions_1+1; i<x+1; i++){
                               
                               
                               atom_A += new_P[i];
                               
                               
                               
                               }
                        
                           
                                
                          sub_str = ")";
                           
                           positions = substrPosition(new_P, sub_str);
                           
                           
                           
                           
                           
                           minimum = new_P.length();
                           int positions_2;
                           
                           for (int i=0; i<positions.size(); i++){
                               
                               
                               
                               
                              if ((positions[i]-x)>0 && (positions[i]-x)<minimum){
                                                  
                                                  minimum = positions[i]-x;
                                                  
                                                  
                                                  
                                                  positions_2 = positions[i];
                                                  
                                                  
                                                  }
                         
                           
                           
                           }
                           
                           
                           
                          
                           
                           for (int i=x+2; i<positions_2; i++){
                               
                               
                               atom_B += new_P[i];
                               
                               
                               
                              }
                               
                           
                           
                          string output_R =  release_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status); 
                          
                      
                          
                          final_P += output_R;
                          
                          
                          if (new_P[positions_2+1] == '/'){
                                                   
                                                   
                                     final_P.insert(0, "(");              
                                     
                                      final_P += ")";              
                                                   
                                     final_P += "/\\";              
                                                   
                                                   } else if (new_P[positions_2+1] == '\\'){
                                                          
                                                          
                                                          final_P.insert(0, "(");              
                                     
                                                          final_P += ")"; 
                                                          
                                                          final_P += "\\/"; 
                                                          
                                                          
                                                          }
                   
                   
                        }
              
              
              
          
              
              
              }
              
     
          
              int final_len =  final_P.length();  
              
            
 
 
 
     	for (int i = 0; i<final_len; i++){
            
            string s = "[A]";

            string::size_type x = final_P.find(s);
 
            if (x != string::npos)
              final_P.erase(x, s.length());
              
            s = "[B]";

            x = final_P.find(s);
 
            if (x != string::npos)
              final_P.erase(x, s.length());
              
              
              
             
        }
 
       
       }


	return final_P;

}
























//Our main function to run

int main(int argc, char** argv) {
    
        
    
//Defining and arranging the variables for taking the corresponding inputs
   
	
	int k;
	string I_file;
	string R_file;
	string J_file;
	string S_file;
	string status;
	string formula_file;
	
	
	
	char* a = argv[1];	
	k = atoi(a);	

	I_file = argv[2];
	R_file = argv[3];
	J_file = argv[4];
	S_file = argv[5];
	status = argv[6];
	formula_file = argv[7];

	
	
//calling the I_unroller function to unroll both the initial conditions	
	
    
	string unrolled_I = I_unroller(k, I_file, "I");
	string unrolled_J = I_unroller(k, J_file, "J");
	
	

//calling the R_unroller function to unroll both the translation relations		
	
	string unrolled_R = R_unroller(k, R_file, "R");
	string unrolled_S = R_unroller(k, S_file, "S");
	
	
//calling the formula_unroller function to unroll our formula
	
	string unrolled_formula = formula_unroller(k, formula_file, status);
	
	

	
	
    //Writing the output into an integrated file
       
       ofstream outdata;
       outdata.open("output.txt"); // opens the file
       if( !outdata ) { // file couldn't be opened
          cerr << "Error: file could not be opened" << endl;
          exit(1);
       }
    
       
       outdata<<unrolled_I<<endl;
       outdata<<"/\\"<<endl;
	   outdata<<unrolled_R<<endl;
	   outdata<<"/\\"<<endl;
	   outdata<<unrolled_J<<endl;
	   outdata<<"/\\"<<endl;
	   outdata<<unrolled_S<<endl;
	   outdata<<"/\\"<<endl;
	   outdata<<unrolled_formula<<endl;
       outdata.close();




return 0;	
	
	
}










