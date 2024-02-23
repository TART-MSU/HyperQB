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

//The function that removes "~~" in our text
string negation_remover(string input){
        for (int i = 0; i<input.length(); i++){
            string s = "~~";
            string::size_type x = input.find(s);
            if (x != string::npos)
              input.erase(x, s.length());
              }
       return input;
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
	added_str+="_";
	added_str+=model_type;
	added_str+="_[0]";



//	if (model_type == "I"){


//           added_str = "_A_[0]";

//    } else if (model_type == "J") {


//           added_str = "_B_[0]";



//     }








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
	string ad_R = "";
	string R = "";
	fstream  mini_R;
	mini_R.open(R_file);
while(!mini_R.eof()){

             string added;

             mini_R >> added;
             R += added;


                     }


char* R_str = &R[0];
string new_R = R_str;



	vector<string> added_strs;


	string A;
	A+="_";
	A+=model_type;
	A+="_[]";

//	if (model_type == "R"){


//           A = "_A_[]";

//    } else if (model_type == "S") {


 //          A = "_B_[]";



//     }


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
                     }
char* P_str = &P[0];
string new_P = P_str;

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
	int k;
	char* a = argv[1];
	k = atoi(a);
    vector<string> inputs;
    string final_check;
    string variable;
    int counter = 2;
    string model_types[26] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
    ofstream outdata;
    while(final_check!="hq"){
         final_check = "";
         variable = argv[counter];
         inputs.push_back(variable);
         final_check += variable[variable.length()-2];
         final_check += variable[variable.length()-1];
         counter++;
     }
    outdata.open("output.txt");
    for (int i=0; i<inputs.size()-3; i+=2){
        	string unrolled_I = I_unroller(k, inputs[i], model_types[i/2]);
	        unrolled_I = iff_replacer(unrolled_I);
	        unrolled_I = if_replacer(unrolled_I);
	        unrolled_I = negation_remover(unrolled_I);
            string unrolled_R = R_unroller(k, inputs[i+1], model_types[i/2]);
	        unrolled_R = iff_replacer(unrolled_R);
	        unrolled_R = if_replacer(unrolled_R);
	        unrolled_R = negation_remover(unrolled_R);
            outdata<<unrolled_I<<endl;
            outdata<<"/\\"<<endl;
	        outdata<<unrolled_R<<endl;
	        outdata<<"/\\"<<endl;
        }
    string unrolled_formula = formula_unroller(k, inputs[inputs.size()-1], inputs[inputs.size()-2]);
	  unrolled_formula = iff_replacer(unrolled_formula);
	  unrolled_formula = if_replacer(unrolled_formula);
	  unrolled_formula = negation_remover(unrolled_formula);
    unrolled_formula="("+unrolled_formula+")";
    outdata<<unrolled_formula<<endl;
    outdata.close();
return 0;
}
