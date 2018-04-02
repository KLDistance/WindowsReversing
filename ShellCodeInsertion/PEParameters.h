#pragma once
#include <Windows.h>
#include <list>

#define MessageBoxAddr 0x7446D740
#define ShellCodeLen 18
#define ReserveBytes 10
#define TextSectionCharacter 0x60000020
#define DataSectionCharacter 0xC0000040

BYTE shellCode[ShellCodeLen] = 
{
	0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00,
	0xE8, 0x00, 0x00, 0x00, 0x00,
	0xE9, 0x00, 0x00, 0x00, 0x00
};

struct DOS_HEADER
{
	WORD e_magic;
	WORD e_cblp;
	WORD e_cp;
	WORD e_crlc;
	WORD e_cparhdr;
	WORD e_minalloc;
	WORD e_maxalloc;
	WORD e_ss;
	WORD e_sp;
	WORD e_csum;
	WORD e_ip;
	WORD e_cs;
	WORD e_lfarlc;
	WORD e_ovno;
	WORD e_res[4];
	WORD e_oemid;
	WORD e_oeminfo;
	WORD e_res2[10];
	DWORD e_lfanew;
};

struct NT_HEADER
{
	DWORD Signature;
	struct
	{
		WORD Machine;
		WORD NumberOfSections;
		DWORD TimeDateStamp;
		DWORD PointerToSymbolTable;
		DWORD NumberOfSymbols;
		WORD SizeOfOptionalHeader;
		WORD Characteristics;
	} FileHeader;
	struct
	{
		WORD magic;
		BYTE MajorLinkerVersion;
		BYTE MinorLinkerVersion;
		DWORD SizeOfCode;
		DWORD SizeOfInitializedData;
		DWORD SizeOfUninitializedData;
		DWORD AddressOfEntryPoint;
		DWORD BaseOfCode;
		DWORD BaseOfData;
		DWORD ImageBase;
		DWORD SectionAlignment;
		DWORD FileAlignment;
		WORD MajorOperatingSystemVersion;
		WORD MinorOperatingSystemVersion;
		WORD MajorImageVersion;
		WORD MinorImageVersion;
		WORD MajorSubsystemVersion;
		WORD MinroSubsystemVersion;
		DWORD Win32VersionValue;
		DWORD SizeOfImage;
		DWORD SizeOfHeaders;
		DWORD CheckSum;
		WORD Subsystem;
		WORD DllCharacteristics;
		DWORD SizeOfStackReserve;
		DWORD SizeOfStackCommit;
		DWORD SizeOfHeapReserve;
		DWORD SizeOfHeapCommit;
		DWORD LoaderFlags;
		DWORD NumberOfRvaAndSizes;
		_IMAGE_DATA_DIRECTORY DataDirectory[16];
	} OptionalHeader;
};

struct SECTION_LIST
{
	BYTE Name[8];
	DWORD VirtualSize;
	DWORD VirtualAddress;
	DWORD SizeOfRawData;
	DWORD PointerToRawData;
	DWORD PointerToRelocations;
	DWORD PointerToLinenumbers;
	WORD NumberOfRelocations;
	WORD NumberOfLinenumbers;
	DWORD Characteristics;
};

DOS_HEADER* DosHeader = NULL;
NT_HEADER* NTHeader = NULL;
std::list<SECTION_LIST*> SectionList;

DWORD readPEFile(IN LPSTR fileName, OUT LPVOID* pFileBuffer);
DWORD writePEFile(IN LPSTR fileName, IN DWORD fileSize, IN LPVOID pFileBuffer);
DWORD FileBufferToImageBuffer(IN LPSTR fileName, OUT LPVOID* pImageBuffer);
DWORD FileBufferToStubClearBuffer(IN LPSTR fileName, OUT LPVOID* pImageBuffer);
DWORD ImageBufferToNewBuffer(IN LPSTR newFileName, IN LPVOID pImageBuffer);

VOID readHeaderInfo(IN LPVOID pBuffer);
VOID RaiseNTHeaderToStubRegion(IN LPVOID pImageBuffer);
VOID InsertionToCodeSection(IN LPVOID pImageBuffer);
VOID InsertionToNewSection(IN LPSTR fileName, OUT LPVOID* pImageBuffer);
VOID InsertionToElongatedSection(IN LPSTR fileName, OUT LPVOID* pImageBuffer);
VOID AddNullNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewSection);