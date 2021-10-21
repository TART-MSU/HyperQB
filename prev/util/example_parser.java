
import java.io.*;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;
import java.util.*;

public class test_parser{

	static String mapped_filename; 
	static String parse_output_filename;

	PrintWriter writer;
	static String output = "";
	static HashMap<String, String> map = new HashMap<String, String>();
	static boolean cex_available;

	public static void main(String[] args) throws FileNotFoundException, UnsupportedEncodingException {

		// input files:
		mapped_filename = (args[0]);
		parse_output_filename = (args[1]);

		System.out.println("parsed: " + parse_output_filename);
		File mapped_file = new File(mapped_filename);
		Scanner sc = new Scanner(mapped_file);
		ArrayList<String> all = new ArrayList<>();

		// extract information from the map
		while (sc.hasNextLine()){
				all.add(sc.nextLine());
		}


		PrintWriter writer1 = new PrintWriter(parse_output_filename, "UTF-8");
		writer1.write("------ MAPPING SYNTHESIS -------\n");




		ArrayList<String> temp = new ArrayList<>();
		for (int i = 3 ; i < all.size()-1; i ++){
			
			String aName = "";
			String bName = "";
			int c = 0;

			for (int a = 0 ; a < all.get(i).length() ; a++){
				if (Character.isDigit(all.get(i).charAt(a))){
					aName = all.get(i).substring(0, a);
					break;
				}
			}

			for (int b = 0 ; b < all.get(i+1).length() ; b++){
				if (Character.isDigit(all.get(i+1).charAt(b))){
					bName = all.get(i+1).substring(0, b);
					break;
				}
			}

			if (aName.equals(bName)){
				temp.add(all.get(i));
				//System.out.println(temp + "\n");

			}else{
				temp.add(all.get(i));
				//System.out.println(temp + "\n");
				//System.out.println("---------");

				if (temp.size() == 1){
					writer1.write(temp.get(0)); 
					if(temp.get(0).contains("mapping_decision_A") && (temp.get(0).charAt(temp.get(0).length()-1) == '1'))
						writer1.write(" ---> decided to: map(Alice_write_secret_to_Bob, Sender_write_m ) " + "\n");
					else if(temp.get(0).contains("mapping_decision_B") && (temp.get(0).charAt(temp.get(0).length()-1) == '1'))
						writer1.write(" ---> decided to: map(Alice_write_secret_to_Bob, Sender_write_mx) " + "\n");
					else if(temp.get(0).contains("mapping_decision_C") && (temp.get(0).charAt(temp.get(0).length()-1) == '1'))
						writer1.write(" ---> decided to: map(Alice_write_random_to_Eve, Sender_write_m ) " + "\n");
					else if(temp.get(0).contains("mapping_decision_D") && (temp.get(0).charAt(temp.get(0).length()-1) == '1'))
						writer1.write(" ---> decided to: map(Alice_write_random_to_Eve, Sender_write_mx) " + "\n");
					else if(temp.get(0).contains("mapping_decision_E") && (temp.get(0).charAt(temp.get(0).length()-1) == '1'))
						writer1.write(" ---> decided to: map(Bob_write_secret_to_Eve, Sender_write_m ) " + "\n");
					else if(temp.get(0).contains("mapping_decision_F") && (temp.get(0).charAt(temp.get(0).length()-1) == '1'))
						writer1.write(" ---> decided to: map(Bob_write_secret_to_Eve, Sender_write_mx) " + "\n");
					else 
						writer1.write("\n");
					//temp = new ArrayList<>();
					//sSystem.out.println(temp.get(0));
					//System.out.println("---------");
				}

				temp = new ArrayList<>();
			}
		}




		writer1.close();
	}





		








		//System.out.println(all);



		//System.out.println("txt:" + txt);
		List<String> output_sorted = new ArrayList<>();

		
			
			// writer1.write("mapping [" + qcir_file_name + "] with  ["+ quabs_result_name + "]\n");
			// writer1.write("sorting variables by name \n");
			// writer1.write("");
			// // for (int g = 0 ; g < output_sorted.size() ; g++){
			// // 	writer1.write(output_sorted.get(g) + "\n");
			// // }
			// writer1.write(output);
		// writer1.close();
	



}

