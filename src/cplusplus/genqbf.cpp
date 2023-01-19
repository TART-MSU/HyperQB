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

//The function for finding indexes of a text
vector<int> substrPosition(string str, string sub_str)
{
	bool flag = false;
	vector<int> arr;
	for (int i = 0; i < str.length(); i++)
	{
		if (str.substr(i, sub_str.length()) == sub_str)
		{
			arr.push_back(i);
			flag = true;
		}
	}

	return arr;
}

//The function that removes "~~" in our text
string negation_remover(string input)
{
	for (int i = 0; i < input.length(); i++)
	{
		string s = "~~";
		string::size_type x = input.find(s);
		if (x != string::npos)
			input.erase(x, s.length());
	}
	return input;
}


//The function for replacing -> existing in the output by the equal logical relations
string if_replacer(string input)
{
	string sub_str;
	int distance;
	sub_str = "->";
	vector<int> positions_if = substrPosition(input, sub_str);
	sub_str = "(";
	vector<int> positions_p1 = substrPosition(input, sub_str);
	sub_str = ")";
	vector<int> positions_p2 = substrPosition(input, sub_str);
	int position = 0;
	int added = 0;
	for (int i = 0; i < positions_if.size(); i++)
	{
		distance = input.length();
		for (int j = 0; j < positions_p1.size(); j++)
		{
			if ((positions_if[i] - positions_p1[j]) < distance && (positions_if[i] - positions_p1[j]) > 0)
			{
				position = positions_p1[j];
				distance = positions_if[i] - positions_p1[j];
			}
			else
			{
				break;
			}
		}

		input.insert(positions_if[i] + added, "\\/");
		input.insert(position + 1 + added, "~");
		added += 3;
	}

	for (int i = 0; i < input.length(); i++)
	{
		string s = "->";
		string::size_type x = input.find(s);
		if (x != string::npos)
			input.erase(x, s.length());
	}

	return input;
}

//The function for replacing < -> existing in the output by the equal logical relations
string iff_replacer(string input)
{
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
	for (int i = 0; i < positions_iff.size(); i++)
	{
		distance = input.length();
		position1 = 0;
		position2 = 0;
		part1 = "";
		part2 = "";
		for (int j = 0; j < positions_p1.size(); j++)
		{
			if ((positions_iff[i] - positions_p1[j]) < distance && (positions_iff[i] - positions_p1[j]) > 0)
			{
				position1 = positions_p1[j];
				distance = positions_iff[i] - positions_p1[j];
			}
			else
			{
				break;
			}
		}

		distance = input.length();
		for (int j = 0; j < positions_p2.size(); j++)
		{
			if ((-positions_iff[i] + positions_p2[j]) > 0)
			{
				position2 = positions_p2[j];
				break;
			}
		}

		for (int j = position1 + added + 1; j < positions_iff[i] + added; j++)
		{
			part1 += input[j];
			input[j] = '@';
		}

		for (int j = added + positions_iff[i] + 3; j < position2 + added; j++)
		{
			part2 += input[j];
			input[j] = '@';
		}

		added_part += "(";
		added_part += "~";
		added_part += part1;
		added_part += "\\/";
		added_part += part2;
		added_part += ")";
		added_part += "/\\";
		added_part += "(";
		added_part += "~";
		added_part += part2;
		added_part += "\\/";
		added_part += part1;
		added_part += ")";
		input.insert(position1 + added + 1, added_part);
		added += added_part.length();
		added_part = "";
	}

	for (int i = 0; i < input.length(); i++)
	{
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
string until_unroller(string modelA, string modelB, int k, vector<string> added_strs_A, vector<string> added_strs_B, string status)
{
	string final_return;
	string new_A = modelA;
	string new_B = modelB;
	string paranthesis;
	string P_nextgen;
	string sub_str;
	int counter;
	vector<int> positions;
	for (int t = 0; t < k + 1; t++)
	{
		int added_l = end(added_strs_A[t]) - begin(added_strs_A[t]);
		P_nextgen = "";
		if (t < k - 1)
		{
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
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = "\\/";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = ")";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = "<->";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			final_return += P_nextgen;
		}
		else if (t == k - 1)
		{
			P_nextgen = "";
			P_nextgen += new_B;
			P_nextgen += "\\/";
			P_nextgen += new_A;
			P_nextgen += "/\\";
			sub_str = "/\\";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = "\\/";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = ")";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = "<->";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			final_return += P_nextgen;
		}
		else if (t == k)
		{
			P_nextgen = "";
			if (status == "-pes")
			{
				P_nextgen += new_B;
			}
			else if (status == "-opt")
			{
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
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = "\\/";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = ")";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
						counter += added_l;
					}
				}
			}

			sub_str = "<->";
			positions = substrPosition(P_nextgen, sub_str);
			counter = 0;
			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);
						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);
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
string release_unroller(string modelA, string modelB, int k, vector<string> added_strs_A, vector<string> added_strs_B, string status)
{
	string final_return;

	string new_A = modelA;
	string new_B = modelB;
	string paranthesis;

	string P_nextgen;
	string sub_str;
	int counter;
	vector<int> positions;

	for (int t = 0; t < k + 1; t++)
	{
		int added_l = end(added_strs_A[t]) - begin(added_strs_A[t]);
		P_nextgen = "";

		if (t < k - 1)
		{
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

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = "\\/";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = ")";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
			 	//        cout<<positions[i]<<endl;
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = "<->";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
			 	//        cout<<positions[i]<<endl;
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			final_return += P_nextgen;
		}
		else if (t == k - 1)
		{
			P_nextgen = "";

			P_nextgen += new_B;
			P_nextgen += "/\\";

			P_nextgen += new_A;
			P_nextgen += "\\/";

			sub_str = "/\\";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = "\\/";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = ")";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
			 	//        cout<<positions[i]<<endl;
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = "<->";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
			 	//        cout<<positions[i]<<endl;
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			final_return += P_nextgen;
		}
		else if (t == k)
		{
			P_nextgen = "";

			if (status == "-pes")
			{
				P_nextgen += "(";
				P_nextgen += new_A;
				P_nextgen += "/\\";
				P_nextgen += new_B;
				P_nextgen += ")";
			}
			else if (status == "-opt")
			{
				P_nextgen += new_B;
			}

			P_nextgen += paranthesis;

			sub_str = "/\\";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = "\\/";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = ")";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
			 	//        cout<<positions[i]<<endl;
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

						counter += added_l;
					}
				}
			}

			sub_str = "<->";

			positions = substrPosition(P_nextgen, sub_str);

			counter = 0;

			for (int i = 0; i < positions.size(); i++)
			{
				if (P_nextgen[positions[i] + counter - 1] != ')')
				{
					if (P_nextgen[positions[i] + counter - 2] == 'A')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

						counter += added_l;
					}
					else if (P_nextgen[positions[i] + counter - 2] == 'B')
					{
						P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

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

string G_unroller(int k, int position_G, string new_P, vector<string> added_strs_A, vector<string> added_strs_B)
{
	string P_nextgen = "";
	string end_sign = "";
	string P_reference;
	string final_P;
	string sub_str;

	vector<int> positions;
	int counter;

	for (int i = position_G + 1; i < new_P.length(); i++)
	{
		if (new_P[i] == 'F' || new_P[i] == 'G')
		{
			if (new_P[i + 1] == '(' || new_P[i + 1] == '~')
			{
				end_sign = P_nextgen[P_nextgen.length() - 2];
				end_sign += P_nextgen[P_nextgen.length() - 1];

				P_nextgen = P_nextgen.substr(0, P_nextgen.size() - 1);
				P_nextgen = P_nextgen.substr(0, P_nextgen.size() - 1);

				break;
			}
		}

		P_nextgen += new_P[i];
	}

	P_reference = P_nextgen;

	for (int t = 0; t < k; t++)
	{
		int added_l = end(added_strs_A[t]) - begin(added_strs_A[t]);

		sub_str = "/\\";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		sub_str = "\\/";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		sub_str = ")";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			//        cout<<positions[i]<<endl;
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		sub_str = "<->";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			//        cout<<positions[i]<<endl;
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		final_P += P_nextgen;
		P_nextgen = P_reference;

		if (t != k - 1)
		{
			final_P += "/\\";
		}
	}

	final_P += end_sign;

	return final_P;

}

//The function for unrolling F
string F_unroller(int k, int position_F, string new_P, vector<string> added_strs_A, vector<string> added_strs_B)
{
	string P_nextgen = "";
	string end_sign = "";
	string P_reference;
	string final_P;
	string sub_str;

	vector<int> positions;
	int counter;

	for (int i = position_F + 1; i < new_P.length(); i++)
	{
		if (new_P[i] == 'F' || new_P[i] == 'G')
		{
			if (new_P[i + 1] == '(' || new_P[i + 1] == '~')
			{
				end_sign = P_nextgen[P_nextgen.length() - 2];
				end_sign += P_nextgen[P_nextgen.length() - 1];

				P_nextgen = P_nextgen.substr(0, P_nextgen.size() - 1);
				P_nextgen = P_nextgen.substr(0, P_nextgen.size() - 1);

				break;
			}
		}

		P_nextgen += new_P[i];
	}

	P_reference = P_nextgen;

	for (int t = 0; t < k; t++)
	{
		int added_l = end(added_strs_A[t]) - begin(added_strs_A[t]);

		sub_str = "/\\";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		sub_str = "\\/";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		sub_str = ")";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			//        cout<<positions[i]<<endl;
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		sub_str = "<->";

		positions = substrPosition(P_nextgen, sub_str);

		counter = 0;

		for (int i = 0; i < positions.size(); i++)
		{
			//        cout<<positions[i]<<endl;
			if (P_nextgen[positions[i] + counter - 1] != ')')
			{
				if (P_nextgen[positions[i] + counter - 2] == 'A')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_A[t]);

					counter += added_l;
				}
				else if (P_nextgen[positions[i] + counter - 2] == 'B')
				{
					P_nextgen.insert(positions[i] + counter, added_strs_B[t]);

					counter += added_l;
				}
			}
		}

		final_P += P_nextgen;
		P_nextgen = P_reference;

		if (t != k - 1)
		{
			final_P += "\\/";
		}
	}

	final_P += end_sign;

	return final_P;

}

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


//This is the function for unrolling our formula
string formula_unroller(int k, string P_file, string status)
{
	string ad_P = "";
	string P = "";
	fstream mini_P;
	mini_P.open(P_file);
	while (!mini_P.eof())
	{
		string added;

		mini_P >> added;
		P += added;
	}

	char *P_str = &P[0];
	string new_P = P_str;

	vector<string> added_strs_A;
	vector<string> added_strs_B;

	string A = "_A_[]";

	string B = "_B_[]";

	string new_A;

	string new_B;

	for (int i = 0; i < k + 1; i++)
	{
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

	if (positions_G_1.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_G_1.begin(), positions_G_1.end());
	}

	vector<int> positions_G_2;

	sub_str = "G~";

	positions_G_2 = substrPosition(new_P, sub_str);

	if (positions_G_2.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_G_2.begin(), positions_G_2.end());
	}

	vector<int> positions_F_1;

	sub_str = "F(";

	positions_F_1 = substrPosition(new_P, sub_str);

	if (positions_F_1.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_F_1.begin(), positions_F_1.end());
	}

	vector<int> positions_F_2;

	sub_str = "F~";

	positions_F_2 = substrPosition(new_P, sub_str);

	if (positions_F_2.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_F_2.begin(), positions_F_2.end());
	}

	vector<int> positions_U_1;

	sub_str = "]U";

	positions_U_1 = substrPosition(new_P, sub_str);

	if (positions_U_1.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_U_1.begin(), positions_U_1.end());
	}

	vector<int> positions_U_2;

	sub_str = ")U";

	positions_U_2 = substrPosition(new_P, sub_str);

	if (positions_U_2.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_U_2.begin(), positions_U_2.end());
	}

	vector<int> positions_R_1;

	sub_str = "]R";

	positions_R_1 = substrPosition(new_P, sub_str);

	if (positions_R_1.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_R_1.begin(), positions_R_1.end());
	}

	vector<int> positions_R_2;

	sub_str = ")R";

	positions_R_2 = substrPosition(new_P, sub_str);

	if (positions_R_2.size() > 0)
	{
		positions_all.insert(positions_all.begin(), positions_R_2.begin(), positions_R_2.end());
	}

	string final_P;
	vector<int> positions;

	if (positions_all.size() > 0)
	{
		sort(positions_all.begin(), positions_all.end());

		//cout << "Sorted \n";
		for (auto x: positions_all)
		{
			if (count(positions_G_1.begin(), positions_G_1.end(), x))
			{
				final_P += G_unroller(k, x, new_P, added_strs_A, added_strs_B);
			}
			else if (count(positions_G_2.begin(), positions_G_2.end(), x))
			{
				final_P += G_unroller(k, x, new_P, added_strs_A, added_strs_B);
			}
			else if (count(positions_F_1.begin(), positions_F_1.end(), x))
			{
				final_P += F_unroller(k, x, new_P, added_strs_A, added_strs_B);
			}
			else if (count(positions_F_2.begin(), positions_F_2.end(), x))
			{
				final_P += F_unroller(k, x, new_P, added_strs_A, added_strs_B);
			}
			else if (count(positions_U_1.begin(), positions_U_1.end(), x))
			{
				sub_str = "(";

				positions = substrPosition(new_P, sub_str);

				int minimum = new_P.length();
				int positions_1;

				for (int i = 0; i < positions.size(); i++)
				{
					if ((x - positions[i]) > 0 && (x - positions[i]) < minimum)
					{
						minimum = x - positions[i];

						positions_1 = positions[i];
					}
				}

				string atom_A;
				string atom_B;

				for (int i = positions_1 + 1; i < x + 1; i++)
				{
					atom_A += new_P[i];
				}

				sub_str = ")";

				positions = substrPosition(new_P, sub_str);

				minimum = new_P.length();
				int positions_2;

				for (int i = 0; i < positions.size(); i++)
				{
					if ((positions[i] - x) > 0 && (positions[i] - x) < minimum)
					{
						minimum = positions[i] - x;

						positions_2 = positions[i];
					}
				}

				for (int i = x + 2; i < positions_2; i++)
				{
					atom_B += new_P[i];
				}

				string output_U = until_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status);

				final_P += output_U;

				if (new_P[positions_2 + 1] == '/')
				{
					final_P.insert(0, "(");

					final_P += ")";

					final_P += "/\\";
				}
				else if (new_P[positions_2 + 1] == '\\')
				{
					final_P.insert(0, "(");

					final_P += ")";

					final_P += "\\/";
				}
			}
			else if (count(positions_U_2.begin(), positions_U_2.end(), x))
			{
				sub_str = "(";

				positions = substrPosition(new_P, sub_str);

				int minimum = new_P.length();
				int positions_1;

				for (int i = 0; i < positions.size(); i++)
				{
					if ((x - positions[i]) > 0 && (x - positions[i]) < minimum)
					{
						minimum = x - positions[i];

						positions_1 = positions[i];
					}
				}

				string atom_A;
				string atom_B;

				for (int i = positions_1 + 1; i < x + 1; i++)
				{
					atom_A += new_P[i];
				}

				sub_str = ")";

				positions = substrPosition(new_P, sub_str);

				minimum = new_P.length();
				int positions_2;

				for (int i = 0; i < positions.size(); i++)
				{
					if ((positions[i] - x) > 0 && (positions[i] - x) < minimum)
					{
						minimum = positions[i] - x;

						positions_2 = positions[i];
					}
				}

				for (int i = x + 2; i < positions_2; i++)
				{
					atom_B += new_P[i];
				}

				string output_U = until_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status);
				final_P += output_U;
				if (new_P[positions_2 + 1] == '/')
				{
					final_P.insert(0, "(");
					final_P += ")";
					final_P += "/\\";
				}
				else if (new_P[positions_2 + 1] == '\\')
				{
					final_P.insert(0, "(");
					final_P += ")";
					final_P += "\\/";
				}
			}
			else if (count(positions_R_1.begin(), positions_R_1.end(), x))
			{
				sub_str = "(";
				positions = substrPosition(new_P, sub_str);
				int minimum = new_P.length();
				int positions_1;
				for (int i = 0; i < positions.size(); i++)
				{
					if ((x - positions[i]) > 0 && (x - positions[i]) < minimum)
					{
						minimum = x - positions[i];
						positions_1 = positions[i];
					}
				}

				string atom_A;
				string atom_B;
				for (int i = positions_1 + 1; i < x + 1; i++)
				{
					atom_A += new_P[i];
				}

				sub_str = ")";
				positions = substrPosition(new_P, sub_str);
				minimum = new_P.length();
				int positions_2;
				for (int i = 0; i < positions.size(); i++)
				{
					if ((positions[i] - x) > 0 && (positions[i] - x) < minimum)
					{
						minimum = positions[i] - x;
						positions_2 = positions[i];
					}
				}

				for (int i = x + 2; i < positions_2; i++)
				{
					atom_B += new_P[i];
				}

				string output_R = release_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status);
				final_P += output_R;
				if (new_P[positions_2 + 1] == '/')
				{
					final_P.insert(0, "(");
					final_P += ")";
					final_P += "/\\";
				}
				else if (new_P[positions_2 + 1] == '\\')
				{
					final_P.insert(0, "(");
					final_P += ")";
					final_P += "\\/";
				}
			}
			else if (count(positions_R_2.begin(), positions_R_2.end(), x))
			{
				sub_str = "(";
				positions = substrPosition(new_P, sub_str);
				int minimum = new_P.length();
				int positions_1;
				for (int i = 0; i < positions.size(); i++)
				{
					if ((x - positions[i]) > 0 && (x - positions[i]) < minimum)
					{
						minimum = x - positions[i];
						positions_1 = positions[i];
					}
				}

				string atom_A;
				string atom_B;
				for (int i = positions_1 + 1; i < x + 1; i++)
				{
					atom_A += new_P[i];
				}

				sub_str = ")";
				positions = substrPosition(new_P, sub_str);
				minimum = new_P.length();
				int positions_2;
				for (int i = 0; i < positions.size(); i++)
				{
					if ((positions[i] - x) > 0 && (positions[i] - x) < minimum)
					{
						minimum = positions[i] - x;
						positions_2 = positions[i];
					}
				}

				for (int i = x + 2; i < positions_2; i++)
				{
					atom_B += new_P[i];
				}

				string output_R = release_unroller(atom_A, atom_B, k, added_strs_A, added_strs_B, status);
				final_P += output_R;
				if (new_P[positions_2 + 1] == '/')
				{
					final_P.insert(0, "(");
					final_P += ")";
					final_P += "/\\";
				}
				else if (new_P[positions_2 + 1] == '\\')
				{
					final_P.insert(0, "(");
					final_P += ")";
					final_P += "\\/";
				}
			}
		}

		int final_len = final_P.length();
		for (int i = 0; i < final_len; i++)
		{
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

// Lilly & Tess
void write_quantifiers (vector<char> const &quantifier, map<string, int> const &var_map, ofstream &my_file) {
    int len = quantifier.size();
    char letter = 'A';
    stringstream ss;
    for (int i=0; i<len; i++) {
        if (quantifier.at(i)=='A') {
            ss << "forall(" ;
        } else {
            ss << "exists(" ;
        }
        string identifier = "_";
        identifier.push_back(letter);
        for (auto x : var_map ) {
            if (x.first.find(identifier) != string::npos){
                ss << x.second << ",";
            }

        }
        string string_ss = ss.str();
        my_file << string_ss.substr(0,string_ss.length()-1) << ")" << endl;
        ss.str(std::string());
        letter++;
    }
}

pair<int, int> find_last_vars(string formula) {
    int index1 = formula.find_last_of("(");
    int index2 = formula.substr(0,index1-1).find_last_of("(");
    pair<int, int> index_pair(index2, index1);
    return index_pair;
}

string write_to_file (ofstream &my_file, int &count, stack<string> &s, string &prefix, pair<int, int> &index_pair){
    stringstream ss;
    //my_file << to_string(count) << " = ";
    ss << to_string(count) << " = ";
    if (s.top() == "/\\") { // or operator
        //my_file << "or(";
        ss << "or(";
    } else if (s.top() == "\\/") { // and operator
        //my_file << "and(";
        ss << "and(";
    }
    index_pair = find_last_vars(prefix);

    //my_file << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    //<< "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
    ss << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    << "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
    prefix = prefix.substr(0,index_pair.first) + '(' + to_string(count) + ')';
    count++;

    return ss.str();
}

bool isOperator(string c)
{
    if (c == "/\\" || c == "\\/") {
        return true;
    }
    else {
        return false;
    }
}

bool isOperator2(string c)
{
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

void InfixToQCIR(stack<string> s, string infix, map<string,int> &var_map, vector<char> const & quantifier, string out_file)
{
    int count = 1;
    pair<int, int> index_pair;
    ofstream my_file;
    stringstream ss;

		my_file.open(out_file);

    string prefix;
    string variable;
    reverse(infix.begin(), infix.end());

    for (int i = 0; i < infix.length(); i++) {
        if (infix[i] == '(') {
            infix[i] = ')';
        }
        else if (infix[i] == ')') {
            infix[i] = '(';
        }
    }
    for (int i = 0; i < infix.length(); i++) {
        if (infix[i]=='~') {
            //account for negation
            prefix +=infix[i];
            size_t pos;
            while ((pos = prefix.find(")~")) != std::string::npos) {
                size_t pos2 = prefix.substr(0,pos).find_last_of("(");
                prefix.replace(pos, 2, ")");
                prefix.insert(pos2+1, "-");
            }
        // adding variable to prefix
        } else if (infix[i] != '/' && infix[i] != '\\' && infix[i] != '~' && !isPar(infix.substr(i,1))){
            // assign numbers to variables here
            variable += infix[i];
            // end of variable due to operator or parentheses
            if ((infix.length() >= (i+1) && (infix[i+1] == '/' || infix[i+1] == '\\' || infix[i+1] == '~' || isPar(infix.substr(i+1,1))))||(infix.length()-1) == i) {
                reverse(variable.begin(), variable.end());
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
            string s1;
            s1 += infix[i];
            s.push(s1);
        }
        else if (infix[i] == ')') {
            while ((s.top() != "(") && (!s.empty())) {
                // assign number to formula and write to file
                ss << write_to_file (my_file, count, s, prefix, index_pair);
                s.pop();
            }
            if (s.top() == "(") {
                s.pop();
            }
        }
        else if (isOperator(infix.substr(i,2)) || isOperator2(infix.substr(i,1))) {
            int index1;
            if (isOperator(infix.substr(i,2))) {
                index1 = 2;
            } else {
                index1 = 1;
            }
            string prec = infix.substr(i,index1);
            if (s.empty()) {
                s.push(infix.substr(i,index1));
            }
            else {
                if (precedence(prec) > precedence(s.top())) {
                    s.push(prec);
                }
                else if (precedence(prec) == precedence(s.top())) {
                    s.push(prec);
                }
                else {
                    while ((!s.empty()) && (precedence(prec) < precedence(s.top()))) {
                        // assign number to formula and write to file
                        ss << write_to_file (my_file, count, s, prefix, index_pair);
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
        ss << write_to_file (my_file, count, s, prefix, index_pair);

        s.pop();
    }
		my_file << "#QCIR-G14" << endl;
    // quantifiers
    write_quantifiers (quantifier, var_map, my_file);
    // output and variables
    my_file << "output" << prefix << endl;
    // write formulas to file
    my_file << ss.str();
    // new map so variables can be sorted by number in qcir file
    map<int, string> new_map;
    for (auto x: var_map) {
        new_map[x.second] = x.first;
    }
    // my_file << "#  variables" << endl;
    for (auto x: new_map) {
        my_file << "# " << x.first << " : " << x.second << endl;
    }
    my_file.close();
}





















//Min genqbf function to run
int main(int argc, char **argv)
{
	int k;
	char *a = argv[1];
	k = atoi(a);
	vector<string> inputs;
	string final_check;
	string variable;
	int counter = 2;
	string model_types[26] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

	ofstream outdata;
	while (final_check != "hq")
	{
		final_check = "";
		variable = argv[counter];
		inputs.push_back(variable);
		final_check += variable[variable.length() - 2];
		final_check += variable[variable.length() - 1];
		counter++;
	}

	// outdata.open("build_today/output.txt");
	clock_t start, end;
	double time_taken;


	start = clock();
	string infix_formulas;
	for (int i = 0; i < inputs.size() - 3; i += 2)
	{
		string unrolled_I = I_unroller(k, inputs[i], model_types[i / 2]);
		// unrolled_I = iff_replacer(unrolled_I);
		// unrolled_I = if_replacer(unrolled_I);
		// unrolled_I = negation_remover(unrolled_I);
		string unrolled_R = R_unroller(k, inputs[i + 1], model_types[i / 2]);
		// unrolled_R = iff_replacer(unrolled_R);
		// unrolled_R = if_replacer(unrolled_R); // THH: to update // THH: to update!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// unrolled_R = negation_remover(unrolled_R);
		// outdata << unrolled_I << endl;
		// outdata << "/\\" << endl;
		// outdata << unrolled_R << endl;
		// outdata << "/\\" << endl;
		infix_formulas = infix_formulas + unrolled_I + "/\\" + unrolled_R;
	}
	end = clock();
	time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "Time for unrolling models  : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;


	start = clock();
	string unrolled_formula = formula_unroller(k+1, inputs[inputs.size() - 1], inputs[inputs.size() - 2]);
	end = clock();
	time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << "Time for unrolling formula : " << fixed
				 << time_taken << setprecision(5);
		cout << " sec " << endl;
	unrolled_formula = iff_replacer(unrolled_formula);
	unrolled_formula = if_replacer(unrolled_formula);
	unrolled_formula = negation_remover(unrolled_formula);
	unrolled_formula = "(" + unrolled_formula + ")"; // warning: here has a hidden ,\\



	infix_formulas = infix_formulas+unrolled_formula;
	// infix_formulas = infix_formulas+"TRUE";

	// infix_formulas = "((~PC_1_A_[0]/\\PC_0_A_[0])/\\~a_B_[0])";
	// cout << infix_formulas << endl;

	outdata.open("build_today/output.txt");
	outdata << infix_formulas << endl;
	outdata.close();


	map<string, int> var_map;
	stack<string> stack;
	string line, input_file;

	//change this to change the quantifiers
	vector<char> quantifier {'E','E'};
	// takes an input file
	// cout << "Enter a file name: " << endl;
	// cin >> input_file;

	// this is broken
	// infix_formulas = "(b_A_[0]/\\~a_A_[0])/\\(((~b_A_[0]/\\~a_A_[0]))\\/((b_A_[1]/\\~a_A_[1])))/\\(((~b_A_[1]/\\~a_A_[1]))\\/((b_A_[2]/\\~a_A_[2])))/\\(b_B_[0]/\\~a_B_[0])";

	// string QCIR_out = "test.qcir";

	string QCIR_out = "build_today/HQ.qcir";
	start = clock();
	InfixToQCIR(stack, infix_formulas, var_map, quantifier, QCIR_out);
	end = clock();
	time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << "Time for converting QCIR : " << fixed
				 << time_taken << setprecision(5);
		cout << " sec " << endl;
	// cout << "\nConversion complete\n" << endl;
	// cout << "tiiiiiiiiiiiiiime" << endl;
	return 0;


}
