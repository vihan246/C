#include "bigint.h"
#include <iostream>
#include <fstream>
using namespace std;


int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout<<"\033[1;41m----Missing an argument----\033[0m"<<endl;
	  cout << "Usage: ./decipher <file>"<<endl;
	  cout << "Examples:" << endl;
	  cout << "\t./decipher secret/message1.txt \033[1;90m//to decipher message 1\033[0m" << endl;
	  cout << "\t./decipher secret/message2.txt \033[1;90m//to decipher message 2\033[0m"<<endl;
		return -1;
	}

	/************* You complete *************/

 ifstream ifile(argv[1]);
  int b = 0;
  
  if (ifile.fail())
  {
    cout << "Failed to open file" << endl;
    return -1;
  }
  
  ifile >> b;
  try
  {
    string strD;
    string strN;
    ifile >> strD >> strN;
    BigInt d(strD, b);
    BigInt n(strN, b);
    string str;
    while (getline(ifile, str))
    {
      if (str.compare("") != 0)
      {
        BigInt secret_code(str, b);
        secret_code.modulusExp(d, n);
        int out = secret_code.to_int();
        char c = (char)out;
        cout << c;
      }
    }
    cout << endl;
  }
  catch (const exception& e)
  {
    cout << e.what() << endl;
    return -1;
  }
  
	return 0;
}
