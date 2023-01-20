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
			if (sem == "-pes"){
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
		if (sem == "-pes"){
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
			if (sem == "-pes"){
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
			if (sem == "-pes"){
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
	// string prop;
	// std::ifstream file(P_file);
	// while (!file.eof())
	// {
	// 	std::string line;
	// 		while (std::getline(file, line)) {
	// 		for (int i = 0 ; i < line.length() ; i++){
	// 			prop += line[i];
	// 		}
	// 	}
	// }
	// cout << prop << endl;
	// prop = prop.substr( prop.find_last_of('.')+1, prop.length());
	// cout << "original formula: " << prop << endl;
	// cout << prop << endl;

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
		// THH update:
		stringstream ss;

		index_pair = find_last_vars(prefix);

    ss << to_string(count) << " = ";
    if (s.top() == "/\\") { // or operator
        ss << "or(";
    } else if (s.top() == "\\/") { // and operator
        ss << "and(";
    }

		// gate_map[index_pair] = count;

		// debug
		// cout << "gate_map: " << endl;
		// for (auto x: gate_map) {
		// 		cout << x.second << ": " << (x.first).first << ", " << x.first.second << endl;
		// }

    //my_file << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    //<< "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
    ss << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    << "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;

		// cout << ss.str() << endl;
    prefix = prefix.substr(0,index_pair.first) + '(' + to_string(count) + ')';


    count++;

    return ss.str();


    // stringstream ss;
		//
		//
    // ss << to_string(count) << " = ";
    // if (s.top() == "/\\") { // or operator
    //     //my_file << "or(";
    //     ss << "or(";
    // } else if (s.top() == "\\/") { // and operator
    //     //my_file << "and(";
    //     ss << "and(";
    // }
    // index_pair = find_last_vars(prefix);
		//
		//
		//
		//
    // //my_file << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    // //<< "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
    // ss << prefix.substr(index_pair.first+1, index_pair.second-(index_pair.first+2))
    // << "," << prefix.substr(index_pair.second+1, prefix.length()-(index_pair.second+2)) << ')' << endl;
		//
    // prefix = prefix.substr(0,index_pair.first) + '(' + to_string(count) + ')';
		//
		//
    // count++;
		//
    // return ss.str();
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

void InfixToQCIR(stack<string> s, string infix, map<string,int> &var_map, vector<char> const & quantifier, string out_file, map<pair<int, int>, int> &gate_map)
{
    int count = 1;
    pair<int, int> index_pair;
    ofstream my_file;
    stringstream ss;

		my_file.open(out_file);

    string prefix;
    string variable;

		// cout << infix << endl;

    reverse(infix.begin(), infix.end());


		// cout << infix << endl;

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
								// cout << "\n(A)" << endl;
								// cout << count << endl;
								// cout << prefix << endl;
								// cout << index_pair.first << ", " << index_pair.second << endl;
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
												// cout << "\n(B)" << endl;
												// cout << count << endl;
												// cout << prefix << endl;
												// cout << index_pair.first << ", " << index_pair.second << endl;
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
				// cout << "\n(C)" << endl;
				// cout << count << endl;
				// cout << prefix << endl;
				// cout << index_pair.first << ", " << index_pair.second << endl;
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


	// FORMULA
	start = clock();
	string prop;
	std::ifstream file(inputs[inputs.size() - 1]);
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
	string quants = prop.substr(0, last_dot+1);

	cout << quants << endl;
	regex re("([A-Z].)");
	cout << regex_replace(quants, re, "") << endl;



	vector<char> quantifier;
	for (int i = 0 ; i < quants.length() ; i++){
		if (quants[i] == 'e'){ // 'exists'
				quantifier.push_back('E');
		}
		if (quants[i] == 'f'){ // 'forall'
				quantifier.push_back('A');
		}
	}
	// for(int i=0; i < quantifier.size(); i++){
  //  std::cout << quantifier.at(i) << ' ';
 	// }
	prop = prop.substr(last_dot+1, prop.length());
	cout << prop << endl;



	string unrolled_formula = formula_unroller(k+1, prop, inputs[inputs.size() - 2]);
	end = clock();
	time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << "Time for unrolling formula : " << fixed
				 << time_taken << setprecision(5);
		cout << " sec " << endl;
	// unrolled_formula = iff_replacer(unrolled_formula);
	// unrolled_formula = if_replacer(unrolled_formula);
	// unrolled_formula = negation_remover(unrolled_formula);
	unrolled_formula = "(" + unrolled_formula + ")"; // warning: here has a hidden ,\\



	infix_formulas = infix_formulas+unrolled_formula;



	outdata.open("build_today/output.txt");
	outdata << infix_formulas << endl;
	outdata.close();


	// infix_formulas.erase(std::remove(infix_formulas.begin(), infix_formulas.end(), '\n'), infix_formulas.cend());
	// infix_formulas.erase(std::remove(infix_formulas.begin(), infix_formulas.end(), ' '), infix_formulas.cend());

	map<string, int> var_map;
	stack<string> stack;
	string line, input_file;
	// map<pair<int, int>, int> gate_map;

	//change this to change the quantifiers

	// takes an input file
	// cout << "Enter a file name: " << endl;
	// cin >> input_file;

	// this is broken
	// infix_formulas = "(some_A_[0]/\\~some_B_[0])\\/(some_A_[0]/\\~some_B_[0])/\\some_A[0]";

	// string QCIR_out = "test.qcir";

	// string QCIR_out = "build_today/HQ-cpp.qcir";

	string QCIR_out = "test.qcir";
	start = clock();
	InfixToQCIR(stack, infix_formulas, var_map, quantifier, QCIR_out, gate_map);
	end = clock();
	time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << "Time for converting QCIR : " << fixed
				 << time_taken << setprecision(5);
		cout << " sec " << endl;
	// cout << "\nConversion complete\n" << endl;
	// cout << "tiiiiiiiiiiiiiime" << endl;
	return 0;
}
