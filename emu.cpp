// Name - Manuj Kumar Grover
// Roll no - 2301CS28

#include<bits/stdc++.h>
using namespace std;
#define pb push_back
const int N = 2e5;

string mnemonics[25] = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};
map<int,int> memory;
vector<int>mcode;
int pc = 0;
int A=0, B=0,sp = 0;
int cmd_run = 0;

map<string, int> commands{
    {"-trace", 1},{"-read", 4},
    {"-write", 5},{"-before", 2},
    {"-after", 3},{"-wipe", 6},
    {"-isa", 7}
};
map<string, int> opcode_table = {
    {"data", 1}, {"ldc", 1}, {"adc", 1}, {"ldl", 2},
    {"stl", 2}, {"ldnl", 2}, {"stnl", 2}, {"add", 0},
    {"sub", 0}, {"shl", 0}, {"shr", 0}, {"adj", 1},
    {"a2sp", 0}, {"sp2a", 0}, {"call", 2}, {"return", 0},
    {"brz", 2}, {"brlz", 2}, {"br", 2}, {"HALT", 0},
    {"SET", 1}
};
bool isvalid_command(string s){
    if(s=="-trace" || s=="-read" || s=="-write" || s=="-before" || s=="-after" ||s=="-isa") return true;
    return false;
}
string to_hex(int n) {		
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
void run_instruction(int mcode){
    int opcode = 0xFF & mcode;
    int operand = mcode>>8;
    switch(opcode){
        case 0: // ldc
            B = A;
	        A = operand;
            break;

        case 1: // adc
            A += operand;
            break;

        case 2: //ldl
            B = A;
	        A = memory[sp + operand];
            break;

        case 3: // stl
            memory[sp + operand] = A;
	        A = B;
            break;

        case 4: // ldnl
            A = memory[A + operand];
            break;

        case 5: //stnl
            memory[A + operand] = B;
            break;

        case 6: // add
            A += B; 
            break;

        case 7: // sub
            A = B - A;
            break;

        case 8: // shl
            A = B << A;
            break;

        case 9 : // shr
            A = B >> A;
            break;

        case 10 : // adj
            sp += operand;
            break;

        case 11 : // a2sp
            sp = A;
	        A = B;
            break;

        case 12: //sp2a
            B = A;
	        A = sp;
            break;

        case 13 : //call
            B = A;
            A = pc;
            pc += operand;
            break;

        case 14 : // return
            pc = A;
	        A = B;
            break;

        case 15 : // brz
            if (A == 0)  pc += operand;
            break;

        case 16 :  // brlz
            if (A < 0) pc += operand;
            break;

        case 17:  //br
            pc += operand;
            break;

        case 18 : // halt
            return;
    }

}

int main(){
    string command;
    cout<<"Enter the command to be executed : ";
    cin >> command;
    if(!isvalid_command(command)){
        cout<<"usage: emu [options] file.o\n";
        cout<<"-trace  show instruction trace\n";
        cout<<"-read   show memory reads\n";
        cout<<"-write  show memory writes\n";
        cout<<"-before show memory dump before execution\n";
        cout<<"-after  show memory dump after execution\n";
        cout<<"-wipe   wipe written flags before execution\n";
        cout<<"-isa    display ISA\n";
        return 0;
    }
    cout<<"Enter the machine code file name(with .o) : ";
    string file_name;
    cin >> file_name;
    FILE *file_ptr;
    file_ptr = fopen(file_name.c_str(), "rb");
    int buffer, i = 0;
    while(fread(&buffer,sizeof(unsigned int), 1,file_ptr) != 0){
        memory[i++] = buffer;
        mcode.pb(buffer);
    }
    while(pc < mcode.size()){
        pc++;
        cmd_run++;
        int opcode = 255 & mcode[pc-1];
        int operand = mcode[pc-1]>>8;
        if(!(pc>=0 && pc<=mcode.size()) || cmd_run > (2e5)){
            cout<<"Segmentation fault!!\n";
            exit(0);
        }

        run_instruction(mcode[pc-1]);

        if(command == "-trace"){
            cout<<to_hex(pc) <<" "<< to_hex(sp)<<" A = "<<to_hex(A)<<" B = "<<to_hex(B)<<" "<< mnemonics[opcode]<<" ";
            if (opcode_table[mnemonics[opcode]]> 0) cout<< to_hex(operand);
            cout << endl;
        }
        if(opcode >= 18) break;
    }
    if(command == "-isa"){
        cout << "Opcode Mnemonic Operand\n"
                << "0      ldc      value \n"
                << "1      adc      value \n"
                << "2      ldl      value \n"
                << "3      stl      value \n"
                << "4      ldnl     value \n"
                << "5      stnl     value \n"
                << "6      add      \n"
                << "7      sub      \n"
                << "9      shr      \n"
                << "10     adj      value \n"
                << "11     a2sp     \n"
                << "12     sp2a     \n"
                << "13     call     offset\n"
                << "14     return   \n"
                << "15     brz      offset\n"
                << "16     brlz     offset\n"
                << "17     br       offset\n"
                << "18     HALT     \n"
                << "       SET      value \n";
    }
    if(command == "-after" || command == "-before"){
        cout << "memory dump "<<command.substr(1)<<" execution" << endl;
        for (int i = 0; i < mcode.size(); i++) {
            if(i%4 == 0) cout << "\n"<<to_hex(i) << " ";
            cout << to_hex((command == "-after"?memory[i] : mcode[i])) << " ";
        }
    }
    printf("\nNumber of instructions executed : %d", cmd_run);
}