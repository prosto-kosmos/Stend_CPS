

#include <iostream>
#include <windows.h>
#include <winuser.h>
#include <conio.h>
#include <string>
#include <fstream>
#include "pugixml.hpp"
using namespace std;

HMODULE hLib;
typedef unsigned char(__stdcall *PTR_F1)(bool, char*);
typedef unsigned char(__stdcall *PTR_F2)(unsigned char, char*);
PTR_F1 GetDataCPS;
PTR_F2 PutDataCPS;

void CreateNewData(char* FSIFileName) {

	// парсим xml
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(FSIFileName);
	// std::cout << "Load result:" << result.description() << std::endl;
	pugi::xml_node find = doc.child("ie").child("cont").child("doc");

	// определяем количество документов, содержащихся в описании xml
	char i;
	for (i = 1; find; find = find.next_sibling("doc"), i++) {
		// cout << find.child("body").attribute("fn").value() << endl;
	}

	// копия FSIFileName
	string FSIFileNameCopy = FSIFileName;

	// вытаскиваем путь к директиве - FSIFileNameCopy
	int count = 0;
	for (int j = 0; ; j++) {
		if (FSIFileNameCopy[j] == '\0') {
			break;
		}
		count++;
	}
	for (int j = count; ; j--) {
		if (FSIFileNameCopy[j] == '\\') {
			break;
		}
		FSIFileNameCopy.erase(j);
	}

	// добавляем новый файл "newfile.txt"(или делаем что-то другое)
	string nfn = "newfile.txt";
	FSIFileNameCopy += nfn;
	ofstream outnewtxt(FSIFileNameCopy.c_str());
	outnewtxt << "New_string in " + nfn;
	outnewtxt.close();

	// изменяем xml-файл
	pugi::xml_node t1 = doc.child("ie").child("cont").append_child("doc");
	t1.append_attribute("proc_id") = i;
	t1.append_attribute("id") = i;
	t1.append_attribute("s_tm") = "2019-04-06T10:58:59";
	t1.append_attribute("f_tm") = "2019-04-06T10:33:45";
	pugi::xml_node t2 = t1.append_child("file");
	pugi::xml_node t3 = t1.append_child("body");
	t3.append_attribute("fn") = FSIFileNameCopy.c_str();
	t3.append_attribute("s") = "100";
	t3.append_attribute("ext") = "1";
	pugi::xml_node t4 = t2.append_child("inf");
	t4.append_attribute("orig_f") = "test1";
	t4.append_attribute("t") = "test2";
	t4.append_attribute("d") = "test3";
	t4.append_attribute("act") = "test4";
	pugi::xml_node t5 = doc.child("ie").append_child("proc");
	t5.append_attribute("res") = "0";
	t5.append_attribute("id") = i;
	t5.append_attribute("s_tm") = "2019-04-06T10:58:59";
	t5.append_attribute("f_tm") = "2019-04-06T10:33:45";
	t5.append_attribute("method") = "test_method";
	t5.append_attribute("exec_t") = "P";
	t5.append_attribute("exec") = "test_app";
	t5.append_attribute("div") = "P";
	t5.append_attribute("subdiv") = "00";
	t5.append_attribute("prty") = "0";
	t5.append_attribute("qlt") = "100";
	pugi::xml_attribute a1 = doc.child("ie").attribute("uie");

	// сохраняем xml в ту же директорию
	doc.save_file(FSIFileName);
	cout << FSIFileNameCopy << endl; // путь к новому файлу
	cout << FSIFileName << endl; // путь к новому xml
}



int main() {
	system("chcp 1251");
	hLib = LoadLibraryA("CPSData_1");
	if (hLib == 0) {
		cout << "CPSData_1.dll download Error!\n";
		return 0;
	}
	else {
		cout << "CPSData_1.dll has been download!\n";
	}
	if (hLib) {
		GetDataCPS = (PTR_F1)GetProcAddress(hLib, "GetDataCPS");
		if (!GetDataCPS) {
			cout << "function GetDataCPS download Error!" << endl;
			return 0;
		}
		cout << "function GetDataCPS download Success!" << endl;
		PutDataCPS = (PTR_F2)GetProcAddress(hLib, "PutDataCPS");
		if (!PutDataCPS) {
			cout << "function PutDataCPS download Error!" << endl;
			return 0;
		}
		cout << "function PutDataCPS download Success!" << endl;
	}

	int resultGetDataCPS(0);
	int resultPutDataCPS(0);
	//char FSIFileName[1024];
	char *FSIFileName = new char[1024];
	while (1) {
		try {
			resultGetDataCPS = (int)GetDataCPS(false, FSIFileName);
			if (resultGetDataCPS == 0) {
				cout << "Data download!\n";
				cout << "Path: " <<FSIFileName;
				cout << endl;
				try {
					CreateNewData(FSIFileName);
				}
				catch (...) {
					cout << "Error in function CreateNewData!\n";
					Sleep(3000);
					continue;
				}
				cout << "Path after FSIFileName: " << FSIFileName << endl;
				try {
				label_put:
					resultPutDataCPS = (int)PutDataCPS(0, FSIFileName);
					if (resultPutDataCPS == 0) {
						cout << "PutDataCPS work Success!\n";
					}
					else if (resultPutDataCPS == 1) { // code==1
						cout << "cpNoData! code:" << resultPutDataCPS << "\n";
						Sleep(3000);
						continue;
					}
					else if (resultPutDataCPS == 2) { // code==2
						cout << "cpParameters_Error! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}
					else if (resultPutDataCPS == 3) { // code==3
						cout << "cpCommand_Error! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}
					else if (resultPutDataCPS == 4) { // code==4
						cout << "cpConnect_Error! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}
					else if (resultPutDataCPS == 6) { // code==6
						cout << "cpCloseProcess! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}
					else if (resultPutDataCPS == 7) { // code==7
						cout << "cpNoProcess! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}
					else if (resultPutDataCPS == 8) { // code==8
						cout << "cpErrorContext! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}
					else if (resultPutDataCPS == 9) { // code==9
						cout << "cpWaitAndCallAgain! code:" <<
							resultPutDataCPS << "\n";
						Sleep(1000);
						goto label_put;
					}
					else if (resultPutDataCPS == 11) { // code==11
						cout << "cpWaitForConnect! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						continue;
					}
					else { // code==all_another
						cout << "cpAnother_Error! code:" <<
							resultPutDataCPS << "\n";
						Sleep(3000);
						return 0;
					}

				}
				catch (...) {
					cout << "Error in blok PutDataCPS!\n";
					Sleep(3000);
					continue;
				}

			}
			else if (resultGetDataCPS == 1) { // code==1
				cout << "cpNoData! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				continue;

			}
			else if (resultGetDataCPS == 2) { // code==2
				cout << "cpParameters_Error! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
			else if (resultGetDataCPS == 3) { // code==3
				cout << "cpCommand_Error! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
			else if (resultGetDataCPS == 4) { // code==4
				cout << "cpConnect_Error! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
			else if (resultGetDataCPS == 6) { // code==6
				cout << "cpCloseProcess! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
			else if (resultGetDataCPS == 7) { // code==7
				cout << "cpNoProcess! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
			else if (resultGetDataCPS == 8) { // code==8
				cout << "cpErrorContext! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
			else if (resultGetDataCPS == 9) { // code==9
				cout << "cpWaitAndCallAgain! code:" << resultGetDataCPS << "\n";
				Sleep(1000);
				continue;
			}
			else if (resultGetDataCPS == 11) { // code==11
				cout << "cpWaitForConnect! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				continue;
			}
			else { // code==all_another
				cout << "cpAnother_Error! code:" << resultGetDataCPS << "\n";
				Sleep(3000);
				return 0;
			}
		}
		catch (...) {
			cout << "Error in blok GetDataCPS!\n";
			Sleep(3000);
			continue;
		}
		Sleep(3000);
	}


}
//---------------------------------------------------------------------------

