/*

Made by: GatoLouco
Github: https://github.com/bwmsdroid/exitlag-hwid-bypass

*/

/*
	How to use?
		- Compile and execute this program. Then execute ExitLag.
		- Do login with a new account.
		- U will see that there is 3 days of trial.
		Yes, u need to do this every new account.

	How it works?
		- We hook some place in the assembly before it send the login request
		- Then we change the HWID to some other (rand letters)
		When the request is made to the server, the server will think it is a new pc
		and will give 3 days of trial.		

*/





#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <sstream>  
#include <string.h>
#include <Windows.h>

using std::cout;
using std::endl;
using std::cin;
using std::hex;
using std::string;
using std::vector;

#include "AuxFunctions.hpp"

// Getting a handle to the console, just to print with colors. Don't mind about this
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);



// Where the magic happens
int main_code()
{
	
	/* Now we start the bypass */


	/* Get the process identificator of the target and check if it were found*/
	DWORD Pid = GetPIdByProcessName((char*)"ExitLag.exe");
	if (!Pid) {
		/* Setting console collor */
		SetConsoleTextAttribute(console, 0x0b);
		cout << "Waiting for process \"ExitLag.exe\"..." << endl;
		Sleep(500);
		main_code();
	}

	/* Open a handle to the target process, with full access, then check if it worked. */
	HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, false, Pid);
	if (hprocess == INVALID_HANDLE_VALUE) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error in Handle..." << endl;
		Sleep(5000);
		return 1;
	}

	/* Give time to the program load everything. */
	Sleep(1000);

	/* The pattern that we want to find in the memory (string version)*/
	string pattern_str = "46 3B 73 08 72 89 8B 75 08 8D 8D 58 FF FF FF 56";
	/* An array where the pattern bytes will be writen */
	vector<byte> pattern_byte; // 
	/* An array where the mask will be write */
	string mask; 

	/* We call the function that will write pattern_byte and mask values.*/
	PatternStringToBytePatternAndMask(pattern_str, &pattern_byte, &mask);

	/* Get the module start address. This is where the function will start to look for the pattern. Then check if it worked*/
	auto main_module_address = GetModuleAddressByName(Pid, "ExitLag.exe");
	if (!main_module_address) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error on GetModuleAddress..." << endl;
		Sleep(5000);
		return 1;
	}

	/* The size of the memory that we will look for the pattern */
	auto size_to_scan = 4096 * 100;
	

	/* Now we call the function that will search for the pattern*/
	DWORD* address = (DWORD*)ExPatternScanByStartAddress(hprocess, main_module_address, size_to_scan, pattern_byte, mask);
	if (!address) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error. Address for hook wasn't found. This may be because you already executed this program or because somehow the bypass is outdate. Trying again anyway..." << endl;
		Sleep(10000);
		system("cls");
		return 1;
	}
	
	/* As the pattern that I scanned is -6 that the address that I want to modify, i am increasing 6  to this */
	address = (DWORD*)((DWORD)address + 6); 
	 cout << "The hook address was found. It is: 0x" << hex << address << endl;
	
	/* Now we need to know where the return address should be. Because we will hook the address with a jump */
	/* The return address is address + 8. This is where the jmp and nops are finisheds */
	DWORD* return_hook_address = (DWORD*)((DWORD)address + 8);

	/* Now we alloc memory to the target process. We will need this to hook. VirtualAllocEx  will return the address were it was allocated	*/
	DWORD* alloc_address = (DWORD*) VirtualAllocEx(hprocess, 0, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!alloc_address) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while allocating memory..." << endl;
		Sleep(3000);
		return 1;
	}

	 cout << "The memory has been allocated. The address is: 0x" << hex << alloc_address << endl;

	/* This is the solution that I found to read the addresses the way a need*/
	/* I am just saying that alloc_address_bytes should be interpreted as an byte array. The same to the other.*/
	BYTE* alloc_address_bytes		 = (BYTE*) &alloc_address;
	BYTE* return_hook_address_bytes  = (BYTE*) &return_hook_address;



	BYTE bytes_of_strings[] = { 0x72, 0x61, 0x6E, 0x64, 0x00, 0x75, 0x63, 0x72, 0x74, 0x62, 0x61, 0x73, 0x65, 0x00 }; //  rand\0ucrtbase\0 to use on writeprocessmemory 3

	/* converting; I gonna explain latter*/
	auto aux1{ reinterpret_cast<std::uint32_t*>(GetModuleHandleA) };
	auto GetModuleHandleA_addr{ reinterpret_cast<char*>(&aux1) };


	auto aux2{ reinterpret_cast<std::uint32_t*>(GetProcAddress) };
	auto GetProcAddress_addr{ reinterpret_cast<char*>(&aux2) };


	auto aux3{ reinterpret_cast<std::uint32_t*>((DWORD*)((DWORD)alloc_address + 0xff)) };
	auto push_rand_addr{ reinterpret_cast<char*>(&aux3) };


	auto aux4{ reinterpret_cast<std::uint32_t*>((DWORD*)((DWORD)alloc_address + 0x104)) };
	auto push_ucrtbase_addr{ reinterpret_cast<char*>(&aux4) };

	
	/* OK. It is an important thing. It is the assembly opcodes that will be writen in the allocated memory. Each bytes means a thing in the assembly.*/
	/* You can check what this mean in the end of this sourcecode. I let the assembly opcodes on there */

	BYTE assembly_opcodes[] = { 0x50, 0x53, 0x56, 0x51, 0x57, 0x52, 0x8B, 0x44, 0x24, 0x54, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x14, 0x08, 0x80, 0xFA, 0x00, 0x74, 0x3F, 
		0x0F, 0x1F, 0x40, 0x00, 0x83, 0xF9, 0x39, 0x41, 0x72, 0xEE, 0x8B, 0xF8, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x51, 0x68, push_rand_addr[0], push_rand_addr[1], 
		push_rand_addr[2], push_rand_addr[3], 0x68, push_ucrtbase_addr[0], push_ucrtbase_addr[1], push_ucrtbase_addr[2], push_ucrtbase_addr[3], 0xBA, GetModuleHandleA_addr[0],
		GetModuleHandleA_addr[1],GetModuleHandleA_addr[2],GetModuleHandleA_addr[3], 0xFF, 0xD2, 0x50, 0xBA, GetProcAddress_addr[0],GetProcAddress_addr[1],GetProcAddress_addr[2],
		GetProcAddress_addr[3], 0xFF, 0xD2, 0xFF, 0xD0, 0x59, 0x3C, 0x41, 0x72, 0xDF, 0x3C, 0x5A, 0x77, 0xDB, 0x88, 0x07, 0x47, 0x41, 0x83, 0xF9, 0x40,	0x72, 0xD2, 0x5A, 0x5F, 0x59,
		0x5E, 0x5B, 0x58, 0x8B, 0x75, 0x08, 0x8D, 0x8D, 0x58, 0xFF, 0xFF, 0xFF, 0x50, 0xB8,	return_hook_address_bytes[0], return_hook_address_bytes[1], return_hook_address_bytes[2], 
		return_hook_address_bytes[3], 0xFF, 0xE0};

	/* This is the assembly opcodes where the hook will be done. */
	BYTE hook_change_bytes[9] = { 0XBE, alloc_address_bytes[0], alloc_address_bytes[1], alloc_address_bytes[2], alloc_address_bytes[3], 0xFF, 0xE6, 0x90, 0x58};

	/* aux variable to confirm that all bytes were written*/
	SIZE_T bytes_writen;

	/* Writting all the opcodes to the allocated memory*/
	if (!WriteProcessMemory(hprocess, (LPVOID)(alloc_address), assembly_opcodes, sizeof(assembly_opcodes), &bytes_writen)) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while WriteProcessMemory (1)..." << endl;
		Sleep(3000);
		return 1;
	}
	if (bytes_writen != sizeof(assembly_opcodes)) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while WriteProcessMemory (1.1)..." << endl;
		Sleep(3000);
		return 1;
	}

	/* Doing the same, but this time to the address (place where the hook will be done) */
	if (!WriteProcessMemory(hprocess, (LPVOID)(address), hook_change_bytes, sizeof(hook_change_bytes), &bytes_writen)){
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while WriteProcessMemory (2)..." << endl;
		Sleep(3000);
		return 1;
	}
	if (bytes_writen != sizeof(hook_change_bytes)) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while WriteProcessMemory (2.1)..." << endl;
		Sleep(3000);
		return 1;
	}

	/* writing the strings 'rand' and 'ucrtbase' to alocated_memory+ff */
	if (!WriteProcessMemory(hprocess, (LPVOID)(aux3), bytes_of_strings, sizeof(bytes_of_strings), &bytes_writen)) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while WriteProcessMemory (3)..." << endl;
		Sleep(3000);
		return 1;
	}
	if (bytes_writen != sizeof(bytes_of_strings)) {
		SetConsoleTextAttribute(console, 0x0c);
		cout << "Error while WriteProcessMemory (3.1)..." << endl;
		Sleep(3000);
		return 1;
	}

	cout << "Success. You can close this program and loggin to your new exitlag account!" << endl;
	Sleep(3000);
	return 0;
}




int main()
{
	/* Just setting some configs to make the console looks better */

	/* cleaning console */
	system("cls");
	/* Changing the Window size */
	system("MODE con cols=100 lines=10");
	/* Allowing utf8 on the console */
	setlocale(LC_ALL, ""); //utf-8
	/* Setting console collor */
	SetConsoleTextAttribute(console, 0x0a);
	
	/* Trying to execute exitlag. If it is on the same folder, tries the usual one*/
	if (GetPIdByProcessName((char*)"ExitLag.exe") == 0) {

		// trying to open exitlag.exe (same folder)
		FILE* fo = fopen("ExitLag.exe", "r");
		if (fo) {
			fclose(fo);
			system("@echo off & start ExitLag.exe");
			
		}
			
		

		// trying to read usual exitlag's path
		fo = fopen("C:\\Program Files (x86)\\ExitLag\\ExitLag.exe", "r");
		if (fo){
			fclose(fo);
			if (GetPIdByProcessName((char*)"ExitLag.exe") == 0) {
				system("@echo off & start \"\" \"C:\\Program Files (x86)\\ExitLag\\ExitLag.exe\" > nul");
			}
			
		}
		
		// C:\\Program Files (x86)\\ExitLag\\ExitLag.exe


		
	}
	/* execute the bypass */
	main_code();

}


/*
// this address (that we found with pattern scan) is before the hook

ExitLag.exe+5D497 - 8B 75 08              - mov esi,[ebp+08]
ExitLag.exe+5D49A - 8D 8D 58FFFFFF        - lea ecx,[ebp-000000A8]

*/

/*
// this is how the address looks like after we hook

ExitLag.exe+5D497 - BE 00004501           - mov esi, allocated_memory
ExitLag.exe+5D49C - FF E6                 - jmp esi
ExitLag.exe+5D49E - 90                    - nop
ExitLag.exe+5D49F - 58                    - pop eax
*/


/*
// and this is how the  memory that we allocated and wrote is.
allocated_memory + 00 - 50                    - push eax
allocated_memory + 01 - 53                    - push ebx
allocated_memory + 02 - 56                    - push esi
allocated_memory + 03 - 51                    - push ecx
allocated_memory + 04 - 57                    - push edi
allocated_memory + 05 - 52                    - push edx
allocated_memory + 06 - 8B 44 24 54           - mov eax,[esp+54]
allocated_memory + 0A - B9 00000000           - mov ecx,00000000
allocated_memory + 0F - 8A 14 08              - mov dl,[eax+ecx]
allocated_memory + 12 - 80 FA 00              - cmp dl,00 { 0 }
allocated_memory + 15 - 74 3F                 - je allocated_memory + 56
allocated_memory + 17 - 0F1F 40 00            - nop dword ptr [eax+00]
allocated_memory + 1B - 83 F9 39              - cmp ecx,39 { 57 }
allocated_memory + 1E - 41                    - inc ecx
allocated_memory + 1F - 72 EE                 - jb allocated_memory + 0F
allocated_memory + 21 - 8B F8                 - mov edi,eax
allocated_memory + 23 - B9 00000000           - mov ecx,00000000 { 0 }
allocated_memory + 28 - 51                    - push ecx
allocated_memory + 29 - 68 FF009F01           - push allocated_memory + FF { ("rand") }
allocated_memory + 2E - 68 04019F01           - push 019F0104 { ("ucrtbase") }
allocated_memory + 33 - BA 600A6276           - mov edx,KERNEL32.GetModuleHandleA { (-1957298293) }
allocated_memory + 38 - FF D2                 - call edx
allocated_memory + 3A - 50                    - push eax
allocated_memory + 3B - BA 50F56176           - mov edx,KERNEL32.GetProcAddress { (-1957298293) }
allocated_memory + 40 - FF D2                 - call edx
allocated_memory + 42 - FF D0                 - call eax
allocated_memory + 44 - 59                    - pop ecx
allocated_memory + 45 - 3C 41                 - cmp al,41 { 65 }
allocated_memory + 47 - 72 DF                 - jb allocated_memory + 28
allocated_memory + 49 - 3C 5A                 - cmp al,5A { 90 }
allocated_memory + 4B - 77 DB                 - ja allocated_memory + 28
allocated_memory + 4D - 88 07                 - mov [edi],al
allocated_memory + 4F - 47                    - inc edi
allocated_memory + 50 - 41                    - inc ecx
allocated_memory + 51 - 83 F9 39              - cmp ecx,39 { 57 }
allocated_memory + 54 - 72 D2                 - jb allocated_memory + 28
allocated_memory + 56 - 5A                    - pop edx
allocated_memory + 57 - 5F                    - pop edi
allocated_memory + 58 - 59                    - pop ecx
allocated_memory + 59 - 5E                    - pop esi
allocated_memory + 5A - 5B                    - pop ebx
allocated_memory + 5B - 58                    - pop eax
allocated_memory + 5C - 8B 75 08              - mov esi,[ebp+08]
allocated_memory + 5F - 8D 8D 58FFFFFF        - lea ecx,[ebp-000000A8]
allocated_memory + 65 - 50                    - push eax
allocated_memory + 66 - B8 9FD49D00           - mov eax,ExitLag.exe+5D49F { (88) }
allocated_memory + 6B - FF E0                 - jmp eax

...

allocated_memory + 6B - 72 61 6E 64 00 75 63 72 74 62 61 73 65 // rand ucrtbase
*/

/*
one more thing: you can not read the

*/
/*

*/
/*
Some questions that you may ask:
- How did you know what pattern use?
	I analised and reverse engineered the exitlag with cheat engine debugger.

- Did you wrote the assembly code?
	Yes, I did.

*/
