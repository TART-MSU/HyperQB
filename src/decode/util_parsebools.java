
import java.io.*;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;
import java.util.*;
import java.util.regex.*;

public class util_parsebools{

	static int num_bits_state = 3;
	static int num_bits_length = 4; // length can be from 0 - 16
	static int num_bits_info = 5; // info can be from 0 - 32
	static int num_bits_line = 4; // program has lines 0-9
	static int num_bits_pc = 5;

	static String mapped_filename;
	static String parse_output_filename;

	static int timestep = 0;

	PrintWriter writer;
	static String output = "";

	static HashMap<String, String> map = new HashMap<String, String>();
	static boolean cex_available;

	public static void main(String[] args) throws UnsupportedEncodingException, FileNotFoundException {

		// input files:
		mapped_filename = (args[0]);
		parse_output_filename = (args[1]);
		Scanner sc = new Scanner("(empty))");

		try{
			File mapped_file = new File(mapped_filename);
			sc = new Scanner(mapped_file);
			System.out.println("parsed output: " + parse_output_filename);
			if(!mapped_file.exists()){
				System.out.println("no output to format.");
				System.exit(0);
			}
		}
		catch (FileNotFoundException e){
			System.out.println("no output to format.");
		}


		// System.out.println("parsed output: " + parse_output_filename);
		// Scanner sc = new Scanner(mapped_file);


		ArrayList<String> all_A = new ArrayList<>();
		ArrayList<String> all_B = new ArrayList<>();

		// extract information from the map
		while (sc.hasNextLine()){
			String line = sc.nextLine();
			if(line.contains("_A"))
				all_A.add(line);
			else if(line.contains("_B"))
				all_B.add(line);
		}

		PrintWriter writer1 = new PrintWriter(parse_output_filename, "UTF-8");
		parse(all_A, writer1);
		timestep = 0; // reset
		parse(all_B, writer1);
		writer1.close();
	}


	private static void parse(ArrayList<String> all, PrintWriter writer1){

		writer1.write("------ Path -------\n");
		ArrayList<String> temp = new ArrayList<>();
		for (int i = 0 ; i < all.size()-1; i ++){

			// if (all.get(i).contains("TRUE")) {continue;}

			String aName = "";
			String bName = "";
			int c = 0;
			for (int a = 0 ; a < all.get(i).length() ; a++){
				if (Character.isDigit(all.get(i).charAt(a)) && all.get(i).charAt(a-1) == '_' ){
					aName = all.get(i).substring(0, a);
					break;
				}
			}

			for (int b = 0 ; b < all.get(i+1).length() ; b++){
				if (Character.isDigit(all.get(i+1).charAt(b)) && all.get(i+1).charAt(b-1) == '_' ){
					bName = all.get(i+1).substring(0, b);
					break;
				}
			}


			if (aName.equals(bName) && (i != all.size()-2)){
				temp.add(all.get(i));
			}
			else {
				temp.add(all.get(i));

				// line separator for bettter visulizing the time steps
				int thistimestep = 0;
				String msg = temp.get(0);
				Matcher m = Pattern.compile("\\[([0-9]+)\\]").matcher(msg);
				while(m.find()){
					// System.out.println(m.groupCount() + "\n");
					thistimestep = Integer.parseInt(m.group(1));
					// System.out.print(timestep+"\n");
				}
				if(thistimestep > timestep){
					// System.out.println("\nTIME @ [" + thistimestep + "]");
					writer1.write("\n  ++++++ TIME @ [" + thistimestep + "] ++++++\n\n");
					timestep = thistimestep;
				}

				// System.out.println(temp);
				if (temp.size() == 1 || (i == all.size()-2)){
					writer1.write(temp.get(0)+"\n"); //booleans
				}

				else {

					// if(temp.size()==3){
						String binaryString = "";
						for (int y = temp.size()-1 ; y >= 0 ; y--){
							binaryString += temp.get(y).charAt(temp.get(y).length()-1);
						}
						//System.out.println(binaryString);
						int decimal = Integer.parseInt(binaryString,2);

						if (temp.get(0).contains("line")){ // for debug to easy see line num
							writer1.write(">>>>>>>>>>>>>>>>");
						}

						// System.out.println(temp);
						if (temp.get(0).contains("state")){
							writer1.write(">> s" + decimal + "\n"  );
						}
						else if (temp.get(0).length()!=temp.get(1).length()){ // Booleas
							for (int j = 0 ; j < temp.size() ; j++){
								writer1.write(temp.get(j) + "\n");
								// System.out.println(temp.get(j) + "\n");
							}
						}
						// else if (!(temp.get(0).substring(0, (temp.get(0).lastIndexOf("[")))).matches(".*\\d.*")){ // Booleans set
						// 	System.out.print("??");
						// 	for (int j = 0 ; j < temp.size() ; j++){
						// 		writer1.write(temp.get(j) + "\n");
						// 		System.out.println(temp.get(j) + "\n");
						// 	}
						// }
						// else if (!(temp.get(0).substring(0, (temp.get(0).lastIndexOf("[")))).contains("[")) {
						// 	// System.out.print("??");
						// 	for (int j = 0 ; j < temp.size() ; j++){
						// 		writer1.write(temp.get(j) + "\n");
						// 		// System.out.println(temp.get(j) + "\n");
						// 	}
						// }
						else{

							String name = temp.get(0).substring(0, (temp.get(0).lastIndexOf("[")));
							// System.out.println(name);
							name = name.substring(name.indexOf("-")+1);
							// System.out.println(name);

							if (!(name.matches(".*\\d.*"))) {
								// System.out.print("??");
								for (int j = 0 ; j < temp.size() ; j++){
									writer1.write(temp.get(j) + "\n");
									// System.out.println(temp.get(j) + "\n");
								}
							}
							else {
								// for (int j = 0 ; j < temp.size() ; j++){
									writer1.write(temp.get(0).substring(0,temp.get(0).length()-1) + decimal + "\n"  );
									// System.out.print("!!!");
									// System.out.println(temp.get(0).substring(0,temp.get(0).length()-1) + decimal + "\n" );
								// }
							}

						}

				}
				temp = new ArrayList<>();
			}
		}
		writer1.write("\n---END---\n\n");
	}

}
