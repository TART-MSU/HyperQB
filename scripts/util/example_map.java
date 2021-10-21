
import java.io.*;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;
import java.util.*;

public class map{

	static String qcir_file_name; 
	static String quabs_result_name;

	static String out_by_name;
	static String out_by_time;

	static String[] info;


	PrintWriter writer;
	static String output = "";

	static HashMap<String, String> map = new HashMap<String, String>();

	static boolean cex_available;



	public static void main(String[] args) throws FileNotFoundException, UnsupportedEncodingException {

		// input files:
		qcir_file_name = (args[0]);
		quabs_result_name = (args[1]);

		out_by_name = (args[2]);
		out_by_time = (args[3]);


		

		
		File qcir_file = new File(qcir_file_name);
		File quabs_result_file = new File(quabs_result_name);
		Scanner qcir_sc = new Scanner(qcir_file);
		Scanner quabs_result_sc = new Scanner(quabs_result_file);





		// extract information from the files
		while (qcir_sc .hasNextLine()){
				String curr = qcir_sc.nextLine();
				if (curr.length()!=0 && curr.charAt(0) == '#' && Character.isDigit(curr.charAt(2))){
						//System.out.println(curr);
						String[] values = curr.substring(2).split(":");
						map.put(values[0], values[1]);
				}
		}

		String txt = "";
		String temp = "";
		while (quabs_result_sc.hasNextLine()){
				temp = quabs_result_sc.nextLine();
				if (temp.length() != 0 && temp.charAt(0) == 'V'){
					txt = temp.substring(2, temp.length()-2);
					//System.out.println(txt);
					info = txt.split(" ");
					break;
				}    
		}

		//System.out.println("txt:" + txt);
		List<String> output_sorted = new ArrayList<>();

		//System.out.println("length" + info.length);
		if (txt.length() == 0) {
			cex_available = false;
			System.out.println("UNSAT. Counter example not found. No mapping.");
		}
		else {
			cex_available = true;
		}


		if (cex_available){
			//System.out.println("------ simple mapping -------");
			System.out.println("SAT. Counter example found.");
			// System.out.println("map [" + qcir_file_name + "] variable names with ["+ quabs_result_name + "]");
			// System.out.println("outputs are written into: ");
			String out1_name = out_by_name;
			String out2_name = out_by_time;
			// System.out.println("1. " + out1_name);
			// System.out.println("2. " + out2_name);
			// hashmap the information together
			for (int i = 0 ; i < info.length ; i ++){
				if (info[i].charAt(0) == '-'){
					//System.out.println(map.get(info[i].substring(1)+" ") + " = 0");
					output += (map.get(info[i].substring(1)+" ") + " = 0\n");
					output_sorted.add(map.get(info[i].substring(1)+" ") + " = 0");
				}
				else {
					//System.out.println(map.get(info[i] + " ") + " = 1");
					output += (map.get(info[i] + " ") + " = 1\n");
					output_sorted.add(map.get(info[i] + " ") + " = 1");
				}
			}




			// short the output based on the variable namess
			Collections.sort(output_sorted, new Comparator<String>() {
				    public int compare(String o1, String o2) {
				        return findLength(o1) - findLength(o2);
				    }

				    int findLength(String s) {
				        int length = s.length();

				        // return 0 if no digits found, should not happen but incase there's wrong info was written
				        return (length == 0) ? 0 : length;
				    }
			});



			// write to the output
			PrintWriter writer1 = new PrintWriter(out1_name, "UTF-8");
			writer1.write("------ simple mapping -------\n");
			writer1.write("mapping [" + qcir_file_name + "] with  ["+ quabs_result_name + "]\n");
			writer1.write("sorting variables by name \n");
			writer1.write("");
			// for (int g = 0 ; g < output_sorted.size() ; g++){
			// 	writer1.write(output_sorted.get(g) + "\n");
			// }
			writer1.write(output);
			writer1.close();


			/// if we have EE...
			Collections.sort(output_sorted, new Comparator<String>() {
				    public int compare(String o1, String o2) {
				        return extractChar(o1) - extractChar(o2);
				    }

				    char extractChar(String s) {
				    	char begin = s.charAt(s.indexOf('[') - 1);
				    	//int end = s.indexOf('[');
				        //String num = s.substring(begin+1, end);
				    	return begin;
				        // return 0 if no digits found, should not happen but incase there's wrong info was written
				        //return num.isEmpty() ? 0 : Integer.parseInt(num);
				    }
			});




			/// awwwwwwwwww
			Collections.sort(output_sorted);

			// short the output based on the time steps: [x]
			Collections.sort(output_sorted, new Comparator<String>() {
				    public int compare(String o1, String o2) {
				        return extractInt(o1) - extractInt(o2);
				    }

				    int extractInt(String s) {
				    	int begin = s.indexOf('[');
				    	int end = s.indexOf(']');

				        String num = s.substring(begin+1, end);

				        // return 0 if no digits found, should not happen but incase there's wrong info was written
				        return num.isEmpty() ? 0 : Integer.parseInt(num);
				    }
			});

			PrintWriter writer2 = new PrintWriter(out2_name, "UTF-8");
			writer2.write("------ simple mapping -------\n");
			writer2.write("mapping [" + qcir_file_name + "] with  ["+ quabs_result_name + "]\n");
			writer2.write("sorting variables by time \n");
			for (int k = 0 ; k < output_sorted.size() ; k++){
				writer2.write(output_sorted.get(k) + "\n");
			}

			writer2.close();


		}

	}	

}

