// Name - Manuj Kumar Grover
// Roll no - 2301CS28

#include <bits/stdc++.h>
#define pb push_back
using namespace std;
bool is_halt = false;
int line = 0, p_cntr = 0;
map<string, bool> islabel_exist;
map<int, vector<string>> error_table, warnings; // {line number, list ofwarning msg/ error msg}
map<string, vector<int>>label_table; // storing declararion of label : {its program counter,line number}
map<string, int>used_label; // keeping track of labels used in the terms of line number
map<string, string>SET; // mapping label to the value
vector<pair<string, string>>list_file; // storing data for {list_file}
vector<pair< vector<string> , int>>instr_final; // storing instruction info after pass1 in format: {vector of label, mneumonic, operand,  program counter}
vector<string>error_msg = {
    "extra on end of line", // 0
    "Invalid label name", // 1
    "label being used more than one time", // 2 
    "bogus mneumonic ", // 3
    "Missing operand",// 4
    "Invalid format: not a valid label or a number", // 5
    "unexpected operand detected", // 6
    "label/operand name missing/invalid", // 7
    "no such label",// 8
    "label declared but not used", // 9 : warning not a error
    "invalid label declaration" // 10
};
map<string, pair<string, int>> opcode_table = {
    {"data", {"", 1}}, {"ldc", {"00", 1}}, 
    {"adc", {"01", 1}}, {"ldl", {"02", 2}},
	{"stl", {"03", 2}}, {"ldnl", {"04", 2}}, 
    {"stnl", {"05", 2}}, {"add", {"06", 0}},
    {"sub", {"07", 0}}, {"shl", {"08", 0}}, 
    {"shr", {"09", 0}}, {"adj", {"0A", 1}},
    {"a2sp", {"0B", 0}}, {"sp2a", {"0C", 0}}, 
    {"call", {"0D", 2}}, {"return", {"0E", 0}},
    {"brz", {"0F", 2}}, {"brlz", {"10", 2}}, 
    {"br", {"11", 2}}, {"HALT", {"12", 0}},
    {"SET", {"", 1}}
};
string to_int(string str,int type) {    // to convert any datatype to decimal
    int ans = 0; 
    int p = 1;
    int i = str.size()-1;
    while(i>=0){
        int digit;
        if(str[i]>='0' && str[i]<='9') digit = str[i] - '0';
        else digit = 10+ str[i] - ((str[i]>='A' && str[i]<='Z')?'A':'a');
        ans+=p*(digit);
        p*=type;
        i--;
    }
    return to_string(ans);
}
bool is_oct(string str) {
    for(int i = 0;i<str.size();i++){
        if(i==0){
            if(str[i]!='0') return false;
        }
        else {
            if(!(str[i]<'8' && str[i]>='0')) return false;
        }
    }
    if(str.size()<=1) return false;
	return true;
}
bool is_hex(string str) {
    for(int i = 0;i<str.size();i++){
        if(i==0){
            if(str[i]!='0') return false;
        }
        else if(i==1){
            if(str[i]!='x') return false;
        }
        else {
            if(!((str[i]<'8' && str[i]>='0') || (str[i]<='f' && str[i]>='a'))) return false;
        }
    }
    if(str.size()<=2) return false;
	return true;
}
bool isdec(string str) {  
    for(auto ch : str){
        if(!(ch<='9' && ch >='0')) return false;
    }    
    return true;    
}
bool handle_label(string label){
    bool flag = true;
    if(!((label[0] <= 'z' && label[0] >= 'a') || (label[0] <= 'Z' && label[0] >= 'A') || label[0] == '_' )) flag = false;
    for(int i = 1;i < label.size();i++){
        if(!( (label[i] <= 'z' && label[i] >= 'a') || (label[i] <= 'Z' && label[i] >= 'A') || (label[i] == '_' ) || (label[i] <= '9' && label[i] >= '0') ) )flag = false;
    }
    if(!flag){
        error_table[line].pb(error_msg[1]);
    }
    else{
        if(islabel_exist[label]) {
            error_table[line].pb(error_msg[2]);
            flag = false;
        }
        else {
            islabel_exist[label] = true;
            label_table[label] = {p_cntr, line};
        }
    }
    return flag;
}

// checking opernad is valid or not, if not then report required errors
string handle_operand(string operand){
        string res = "";
        bool flag = true;
        if(!((operand[0] <= 'z' && operand[0] >= 'a') || (operand[0] <= 'Z' && operand[0] >= 'A') || operand[0] == '_' )) flag = false;
        for(int i = 1;i< operand.size();i++){
            if(!((operand[i] <= 'z' && operand[i] >= 'a') || (operand[i] <= 'Z' && operand[i] >= 'A') || operand[i] == '_'  || (operand[i] <= '9' && operand[i] >= '0'))) flag = false;
        }

        if(flag){
            used_label[operand] = line;
            return operand;
        }
        string abs_oper = operand;
        if(operand[0] == '+' || operand[0] == '-' ) {
            res+= operand[0];
            abs_oper = operand.substr(1);
        }
        if (is_oct(abs_oper))  res += to_int(abs_oper.substr(1), 8);
        else if (is_hex(abs_oper)) res += to_int(abs_oper.substr(2), 16);
        else if (isdec(abs_oper)) res += abs_oper;
        else  res = "";
        return res;
}

// checking if instruction is valid or not, if not then report errors
bool handle_instr(string mneumonic, string &operand, bool islabel, string label){
    bool isInstr_valid = true;
    if(!opcode_table.count(mneumonic)){ // detecting bogus mneumonics
        error_table[line].pb(error_msg[3]);
        isInstr_valid = false;
    }
    if(mneumonic == "HALT"){
        is_halt = true;
    }
    int type = opcode_table[mneumonic].second;
    if(type == 0){
        if(!operand.empty()) {
            error_table[line].pb(error_msg[6]);
            isInstr_valid = false;
        }
    }
    else {
        if(operand.empty()){
            isInstr_valid = false;
            error_table[line].pb(error_msg[4]);
        }
        else{
            string new_oper = handle_operand(operand);
            if(new_oper.empty()){
                error_table[line].pb(error_msg[7]);
                isInstr_valid = false;
            }
            else{
                operand = new_oper;
                if(mneumonic == "SET"){
                    if(islabel){
                        SET[label] = new_oper;
                    }
                    else{
                        error_table[line].pb(error_msg[7]);
                        isInstr_valid = false;
                    }
                }
            }
        }
    }
    return isInstr_valid;
}
void pass1(vector<string>instructions){
    for(int i = 0;i<instructions.size();i++){
        line++;
        string instr_with_comments = instructions[i];
        string instr = "", temp;

        // Removing Comments
        for(auto ch : instr_with_comments){
            if(ch==';') break;
            if(ch==':') instr.pb(' ');
            instr.pb(ch);
        }

        bool isLabel = false;
        stringstream curr(instr);
        vector<string>splitted_instr;
        string label = "";
        int size = 0;
        int cnt = 0;

        while(curr >> temp){
            if(temp == ":") {
                isLabel = true;
                if(!splitted_instr.empty()) {
                    label = splitted_instr.back();
                    splitted_instr.pop_back();
                    size--;
                }
                cnt++;
                continue;
            }
            if(temp.back() == ':'){
                isLabel = true;
                temp.pop_back();
                label = temp;
                continue;
            }
            if(temp[0] == ':'){
                isLabel =true;
                if(!splitted_instr.empty()) label = splitted_instr.back();
                temp = temp.substr(1);
                splitted_instr.pop_back();
                size--;
            }
            size++;
            splitted_instr.pb(temp);
        }
        bool isInstr_valid =true;
        if(cnt>1){
            isInstr_valid = false;
            error_table[line].pb(error_msg[10]);
        }
        if(isLabel){
            if(label.empty()) error_table[line].pb(error_msg[7]);
            else isInstr_valid &= handle_label(label);
        }
        
        if(size ==  1){
            string str = "";
            isInstr_valid &= handle_instr(splitted_instr[0],str, (!label.empty()), label);
        }
        else if(size == 2){
            isInstr_valid &= handle_instr(splitted_instr[0], splitted_instr[1], (!label.empty()), label);
            }
        else if(size>2){
            error_table[line].pb(error_msg[0]);
            isInstr_valid = false;
        }

        // if it is valid instruction, store it
        if(isInstr_valid){
            vector<string>temp;
            temp.pb(label);
            for(auto str : splitted_instr) temp.pb(str);
            while(temp.size()!= 3) temp.pb("");

            if(!(temp[0]== "" && temp[1] == "" && temp[2] == "")){
                instr_final.pb({temp, p_cntr});
                if(temp[1]!="") p_cntr++;
            } 
        }
    }

    // storing warnings
    for(auto labels_info : label_table){
        auto label = labels_info.first;
        auto line_no = labels_info.second[1];
        if(used_label[label]==0){
            warnings[line_no].pb(error_msg[9]);
        }
    }

    for(auto used_label_info : used_label){
        auto label = used_label_info.first;
        auto line_no = used_label_info.second;

        if(!islabel_exist[label] ){
            error_table[line_no].pb(error_msg[8]);
        }
    }
}
string to_hex(int n) {		// convert to hex
	unsigned int num = n;
	string str = "        ";
    int i = 0;
    while(i<8){
        int digit = num%16;
        if(digit < 10) str[8-i-1] = '0' + digit;
        else str[8-i-1] = 'A' + char(digit - 10);
        num/=16;
        i++;
    }
	return str;
}
void generate_machine_code(int type,int pc, string label, string mneumonic, string operand){
    string machine_code = "        ";
    if (type == 0) machine_code = "000000" +opcode_table[mneumonic].first;
    else if (type == 2) { 	
        if(islabel_exist[operand]){
            machine_code = to_hex(label_table[operand][0] -  (pc + 1)).substr(2);
        }
        else{
            machine_code = to_hex((int)stoi(operand)).substr(2);
        }
        machine_code += opcode_table[mneumonic].first;
    } 
    else if (type == 1){		
        if(mneumonic != "data" && mneumonic != "SET"){
            int value = (islabel_exist[operand] ? label_table[operand][0]: (int)stoi(operand));
            machine_code = to_hex( ( SET.count(operand) ? stoi(SET[operand] ):value ) ).substr(2) +opcode_table[mneumonic].first;
        }
        else  machine_code = to_hex(stoi(operand));
    } 

    string proper = to_hex(pc)+" "+machine_code + " "+ label+ (label.empty()?mneumonic:(": "+mneumonic)) + " " + operand; 
    list_file.pb({machine_code, proper});
}
void pass2(){
    for(auto instr_info : instr_final){
        int pc = instr_info.second;
        string label = instr_info.first[0];
        string mneumonic = instr_info.first[1];
        string operand = instr_info.first[2];

        int type = (mneumonic.empty()) ? -3 : opcode_table[mneumonic].second;
		generate_machine_code(type, pc, label, mneumonic, operand);
    }
}
int main( ) {
    string str;
    cout << "Enter the file name (with .asm extension): ";
    cin >> str;  // User types input like "bubble_sort.asm"

    // Open the file for reading
    ifstream inputFile(str);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << str << endl;
        return 1; // Exit if file cannot be opened
    }

    // Reading each instruction from the file and storing it
    string s;
    vector<string> instructions;
    while (getline(inputFile, s)) {
        instructions.push_back(s); // Store each line in the vector
    }

    inputFile.close(); 
    pass1(instructions);

    string file = str.substr(0,str.size()-4) ;

    string error_file = file + ".log";
    ofstream coutErrors(error_file);
    if(!error_table.empty()){
        for (auto error : error_table) {
            int line_no = error.first;
            for(auto str : error.second){
                coutErrors << "line --> " << line_no << " error--> " <<str<< endl;
            }
        }
        cout<<"log file generated\n";
        coutErrors.close();
    }
    else{
        pass2();

        // generating warnings and errors
        coutErrors << "No errors!" << endl;
        if(!is_halt){
            coutErrors << "HALT has not been used to end the program!!" << endl;
        }
		for (auto warning_info : warnings) {
            int line_no = warning_info.first;
            for(auto str : warning_info.second){
                coutErrors << "line --> " << line_no << " warning --> " <<str<< endl;
            }
            
		}
        coutErrors.close();

        //generating list file
        string list = file + ".lst";
        string mcode = file + ".o";
        ofstream coutList(list);
        ofstream coutMCode;
        coutMCode.open(mcode, ios::binary | ios::out);
        for (const auto &cur : list_file) {
            coutList<< cur.second << "\n";  // Write the second element of each pair
            if (cur.first.empty() or cur.first == "        ") continue;
            unsigned int temp = (unsigned int)stoi(to_int(cur.first,16));
            static_cast<int>(temp);
            coutMCode.write((const char*)&temp, sizeof(unsigned int));
        }
        coutList.close();
        coutMCode.close();
        cout << "Machine code object(.o) file generated" << endl;
        cout << "Listing (.lst) file generated" << endl;
    }
}

