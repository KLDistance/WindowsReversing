	// ShellCodeInsertion.cpp : 
	//

	#include "stdafx.h"
	#include "PEParameters.h"

DWORD readPEFile(IN LPSTR fileName, OUT LPVOID* pFileBuffer)
{
	FILE* pFile = NULL;
	LPVOID pTempBuffer = NULL;
	DWORD fileSize = 0;

	//Open PE file
	pFile = fopen(fileName, "rb");
	if (!pFile)
	{
		printf("Unable to open the exe file!\n");
		return 0;
	}
	//Find the FileSize in disk
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);

	pTempBuffer = malloc(fileSize);
	if (!pTempBuffer)
	{
		printf("Unable to assign the space for PE file!\n");
		fclose(pFile);
		return 0;
	}
	memset(pTempBuffer, 0, fileSize);

	size_t n = fread(pTempBuffer, 1, fileSize, pFile);
	if (!n)
	{
		printf("Unable to read the data from disk!\n");
		free(pTempBuffer);
		fclose(pFile);
		return 0;
	}
	*pFileBuffer = pTempBuffer;
	pTempBuffer = NULL;
	fclose(pFile);
	return fileSize;
}

DWORD writePEFile(IN LPSTR fileName,IN DWORD fileSize, IN LPVOID pFileBuffer)
{
	FILE *pFile = NULL;

	pFile = fopen(fileName, "wb");
	if (!pFile)
	{
		printf("Unable to setup a new file!\n");
		return 0;
	}
	fwrite(pFileBuffer, 1, fileSize, pFile);
	fclose(pFile);
	return fileSize;
}

VOID readHeaderInfo(IN LPVOID pBuffer)
{
	//Initialize the parameters
	DosHeader = NULL;
	NTHeader = NULL;
	SectionList.clear();

	LPVOID pointer = pBuffer;
	DWORD i;

	//Read DosHeader
	DosHeader = (DOS_HEADER*)pointer;

	//Read NTHeader
	pointer = (LPVOID)(DosHeader->e_lfanew + (DWORD)pBuffer);
	NTHeader = (NT_HEADER*)pointer;
	printf("%x\n", ((DWORD)&NTHeader->Signature) - (DWORD)pBuffer);
	//Read SectionList
	pointer = ((LPBYTE)pointer + 24 + NTHeader->FileHeader.SizeOfOptionalHeader);
	for (i = 0; i < NTHeader->FileHeader.NumberOfSections; i++)
	{
		SECTION_LIST* Section = (SECTION_LIST*)pointer;
		SectionList.push_back(Section);
		pointer = ((LPBYTE)pointer + 40);
	}
}

DWORD FileBufferToImageBuffer(IN LPSTR fileName, OUT LPVOID* pImageBuffer)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pTempBuffer = NULL;
	LPVOID pointer = NULL;
	DWORD fileSize = 0;
	DWORD imageSize = 0;
	DWORD i;

	//Initialize parameters
	SectionList.clear();

	//read PE file
	fileSize = readPEFile(fileName, &pFileBuffer);
	if (!fileSize || !pFileBuffer)
	{
		printf("Fail to read the PE file!\n");
		return 0;
	}

	//Read the headers
	//To DosHeader
	pointer = pFileBuffer;
	DosHeader = (DOS_HEADER*)pointer;
	
	//To NTHeader
	pointer = (LPBYTE)((DWORD)DosHeader->e_lfanew + (DWORD)pFileBuffer);
	NTHeader = (NT_HEADER*)pointer;

	//To SectionList
	pointer = ((LPBYTE)pointer + 24 + NTHeader->FileHeader.SizeOfOptionalHeader);
	for (i = 0; i < NTHeader->FileHeader.NumberOfSections; i++)
	{
		SECTION_LIST* Section = (SECTION_LIST*)pointer;
		SectionList.push_back(Section);
		pointer = ((LPBYTE)pointer + 40);
	}

	//Assign the space for the ImageBuffer
	*pImageBuffer = malloc(NTHeader->OptionalHeader.SizeOfImage);
	if (!*pImageBuffer)
	{
		printf("Unable to assign the space for the ImageBuffer!\n");
		return 0;
	}
	imageSize = NTHeader->OptionalHeader.SizeOfImage;
	memset(*pImageBuffer, 0, imageSize);

	//Copy the headers to the ImageBuffer
	for (i = 0; i < NTHeader->OptionalHeader.SizeOfHeaders; i++)
	{
		*((LPBYTE)((DWORD)*pImageBuffer + i)) = *((LPBYTE)((DWORD)pFileBuffer + i));
	}

	//Copy the sections to the ImageBuffer
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		pointer = (LPBYTE)((*it)->PointerToRawData + (DWORD)pFileBuffer);
		pTempBuffer = (LPBYTE)((*it)->VirtualAddress + (DWORD)*pImageBuffer);
		for (i = 0; i < (*it)->SizeOfRawData; i++)
		{
			*((LPBYTE)((DWORD)pTempBuffer + i)) = *((LPBYTE)((DWORD)pointer + i));
		}
	}
	pointer = NULL;
	SectionList.clear();
	free(pFileBuffer);
	return imageSize;
}

DWORD FileBufferToStubClearBuffer(IN LPSTR fileName, OUT LPVOID* pImageBuffer)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pTempBuffer = NULL;
	LPVOID pointer = NULL;
	DWORD stubSize = 0;
	DWORD fileSize = 0;
	DWORD i;

	//Read the file from disk to ram
	fileSize = readPEFile(fileName, &pFileBuffer);
	if (!fileSize || !pFileBuffer)
	{
		printf("Unable to open the PE file!\n");
		return 0;
	}

	//Read the headers info
	readHeaderInfo(pFileBuffer);

	//Check the stub and modify the e_lfanew and SizeOfHeaders
	if ((int)DosHeader->e_lfanew - 64 <= 0)
	{
		printf("The stub has already been cleared!\n");
		return 0;
	}
	else if ((int)DosHeader->e_lfanew - 64 < 40)
	{
		printf("Unable to put up a new SectionList!\n");
		return 0;
	}
	stubSize = DosHeader->e_lfanew - 64;
	DosHeader->e_lfanew -= stubSize;
	
	//Assign the size for ImageBuffer without stub

}

DWORD ImageBufferToNewBuffer(IN LPSTR newFileName, IN LPVOID pImageBuffer)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pTempFileBuffer = NULL;
	LPVOID pTempImageBuffer = NULL;
	DWORD fileSize = 0;
	DWORD i;

	//Read headers info (Abort when code is inserted)
	readHeaderInfo(pImageBuffer);

	//Assign new space for NewFileBuffer
	fileSize += NTHeader->OptionalHeader.SizeOfHeaders;
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		fileSize += (*it)->SizeOfRawData;
	}
	pFileBuffer = malloc(fileSize);
	if (!pFileBuffer)
	{
		printf("Unable to assign the new space for NewFileBuffer.\n");
		return 0;
	}
	pTempFileBuffer = pFileBuffer;
	memset(pFileBuffer, 0, fileSize);

	//Copy headers from ImageBuffer to NewFileBuffer
	for (i = 0; i < NTHeader->OptionalHeader.SizeOfHeaders; i++)
	{
		*((LPBYTE)pTempFileBuffer + i) = *((LPBYTE)pImageBuffer + i);
	}

	//Copy the Sections from ImageBuffer to NewFileBuffer
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		pTempFileBuffer = (LPBYTE)((*it)->PointerToRawData + (DWORD)pFileBuffer);
		pTempImageBuffer = (LPBYTE)((*it)->VirtualAddress + (DWORD)pImageBuffer);
		for (i = 0; i < (*it)->SizeOfRawData; i++)
		{
			*((LPBYTE)((DWORD)pTempFileBuffer + i)) = *((LPBYTE)((DWORD)pTempImageBuffer + i));
		}
	}
	
	//Write contents of NewFileBuffer to Disk
	writePEFile(newFileName, fileSize, pFileBuffer);

	free(pFileBuffer);
	pTempFileBuffer = NULL;
	pTempImageBuffer = NULL;
	return fileSize;
}

VOID InsertionToCodeSection(IN LPVOID pImageBuffer)
{
	LPVOID pInsertionBegin = NULL;
	LPVOID pTempCode = NULL;
	DWORD tempCharacter = 0;
	DWORD JMP;
	DWORD i;

	//Get the header info
	readHeaderInfo(pImageBuffer);
	
	//Move to the end of code section
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		tempCharacter = (~((*it)->Characteristics)) & TextSectionCharacter;
		if (!tempCharacter && ((*it)->SizeOfRawData - (*it)->VirtualSize > ShellCodeLen + ReserveBytes))
		{
			pInsertionBegin = (LPVOID)((*it)->VirtualAddress + (*it)->VirtualSize + (DWORD)pImageBuffer + ReserveBytes);
			break;
		}
	}
	if (!pInsertionBegin)
	{
		printf("Unable to find a proper insertion entry!\n");
		return;
	}
	pTempCode = pInsertionBegin;

	//Insertion:
	//---------------------------------------------------------------------------------
	//TODO the code insertion here. This may vary among different types of insert codes

	//Copy the format shellcode in position
	for (i = 0; i < ShellCodeLen; i++)
	{
		*((LPBYTE)((DWORD)pInsertionBegin + i)) = shellCode[i];
	}

	//Calculate and fill in the MessageBox JMP address
	JMP = MessageBoxAddr - ((DWORD)pTempCode + (DWORD)13 - (DWORD)pImageBuffer + NTHeader->OptionalHeader.ImageBase);
	pTempCode = (LPVOID)((DWORD)pTempCode + 9);
	*((PDWORD)pTempCode) = JMP;
	pTempCode = pInsertionBegin;

	//Calculate and fill in the OEP JMP address
	JMP = NTHeader->OptionalHeader.AddressOfEntryPoint - ((DWORD)pTempCode + (DWORD)18 - (DWORD)pImageBuffer);
	pTempCode = (LPVOID)((DWORD)pTempCode + 14);
	*((PDWORD)pTempCode) = JMP;
	pTempCode = pInsertionBegin;

	//Change the OEP and let it point to the beginning of the shellcode
	NTHeader->OptionalHeader.AddressOfEntryPoint = ((DWORD)pInsertionBegin - (DWORD)pImageBuffer);

	//End the code insertion
	//---------------------------------------------------------------------------------

}

VOID RaiseNTHeaderToStubRegion(IN LPVOID pImageBuffer)
{
	LPVOID pLfanew = NULL;
	LPVOID pBeginningOfNTHeader = NULL;
	LPVOID pLastPosOfSectionList = NULL;
	LPVOID pTemp = NULL;
	DWORD deltaSize = 0;
	DWORD copySize = 0;
	DWORD i;
	do
	{
		//Clear the structures
		DosHeader = NULL;
		NTHeader = NULL;
		SectionList.clear();

		//Points to the DosHeader
		pTemp = pImageBuffer;
		DosHeader = (DOS_HEADER*)pImageBuffer;
		
		//pLfanew points to the end of DosHeader
		pLfanew = &(DosHeader->e_lfanew);

		//Points to the NTHeader
		NTHeader = (NT_HEADER*)(DosHeader->e_lfanew + (DWORD)pImageBuffer);
		pBeginningOfNTHeader = (LPVOID)NTHeader;
		pTemp = (LPVOID)NTHeader;

		deltaSize = (DWORD)pTemp - (DWORD)&DosHeader->e_lfanew - 4;
		if (deltaSize < 80)
		{
			printf("The stub between DOS_HEADER and NT_HEADER is fewer than 80 bytes!\nCannot execute a raise operation.\n");
			DosHeader = NULL;
			break;
		}
		
		//Calculate the copy size
		copySize = 24 + NTHeader->FileHeader.SizeOfOptionalHeader + NTHeader->FileHeader.NumberOfSections * 40;

		//Copy
		for (i = 0; i < copySize; i++)
		{
			*((PBYTE)((DWORD)pLfanew + 4 + i)) = *((PBYTE)((DWORD)pBeginningOfNTHeader + i));
		}

		//Modify the value to e_lfanew
		*((PDWORD)pLfanew) = (DWORD)pLfanew - (DWORD)pImageBuffer + 4;

		//Get the last pos of new SectionList
		pTemp = (LPVOID)((DWORD)&DosHeader->e_lfanew + 4 + copySize);

		//Clear the rest garbage in ram
		memset(pTemp, 0, deltaSize);

	} while (0);
}

VOID InsertionToNewSection(IN LPSTR fileName, OUT LPVOID* pImageBuffer)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pTempBuffer = NULL;
	LPVOID pointer = NULL;
	LPVOID pInsertionBegin = NULL;
	LPVOID pTempCode = NULL;
	LPVOID pLastSection = NULL;
	DWORD fileSize = 0;
	DWORD imageSize = 0;
	DWORD previousImageSize = 0;
	DWORD insertSize = 0;
	DWORD JMP = 0;
	DWORD i;

	//Initialize parameters
	SectionList.clear();

	//read PE file
	fileSize = readPEFile(fileName, &pFileBuffer);
	if (!fileSize || !pFileBuffer)
	{
		printf("Fail to read the PE file!\n");
		return;
	}

	//Read the headers
	//To DosHeader
	pointer = pFileBuffer;
	DosHeader = (DOS_HEADER*)pointer;

	//To NTHeader
	pointer = (LPBYTE)((DWORD)DosHeader->e_lfanew + (DWORD)pFileBuffer);
	NTHeader = (NT_HEADER*)pointer;

	//To SectionList
	pointer = ((LPBYTE)pointer + 24 + NTHeader->FileHeader.SizeOfOptionalHeader);
	for (i = 0; i < NTHeader->FileHeader.NumberOfSections; i++)
	{
		SECTION_LIST* Section = (SECTION_LIST*)pointer;
		SectionList.push_back(Section);
		pointer = ((LPBYTE)pointer + 40);
	}

	//Calculate the resized ImageBuffer
	insertSize = (ShellCodeLen / NTHeader->OptionalHeader.SectionAlignment + 1) * NTHeader->OptionalHeader.SectionAlignment;
	previousImageSize = NTHeader->OptionalHeader.SizeOfImage;
	imageSize = NTHeader->OptionalHeader.SizeOfImage + insertSize;

	//Assign the space for the resized ImageBuffer
	*pImageBuffer = malloc(imageSize);
	if (!*pImageBuffer)
	{
		printf("Unable to assign the space for the ImageBuffer!\n");
		return;
	}
	memset(*pImageBuffer, 0, imageSize);
	NTHeader->OptionalHeader.SizeOfImage = imageSize;

	//Copy the headers to the ImageBuffer
	for (i = 0; i < NTHeader->OptionalHeader.SizeOfHeaders; i++)
	{
		*((LPBYTE)((DWORD)*pImageBuffer + i)) = *((LPBYTE)((DWORD)pFileBuffer + i));
	}

	//Copy the sections to the ImageBuffer
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		pointer = (LPBYTE)((*it)->PointerToRawData + (DWORD)pFileBuffer);
		pTempBuffer = (LPBYTE)((*it)->VirtualAddress + (DWORD)*pImageBuffer);
		for (i = 0; i < (*it)->SizeOfRawData; i++)
		{
			*((LPBYTE)((DWORD)pTempBuffer + i)) = *((LPBYTE)((DWORD)pointer + i));
		}
		pLastSection = (LPVOID)((DWORD)pTempBuffer + i);
	} 

	//Read and change the focus on ImageBuffer
	readHeaderInfo(*pImageBuffer);

	//Modify the shellcode entry and imageSize
	pInsertionBegin = (LPVOID)((DWORD)*pImageBuffer + previousImageSize);
	pTempCode = pInsertionBegin;
	printf("%x, %x \n", previousImageSize, (DWORD)*pImageBuffer + previousImageSize);

	//Insert the new section list
	i = 0;
	pointer = NULL;
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		if (i == NTHeader->FileHeader.NumberOfSections - 1)
		{
			pointer = (LPVOID)((DWORD)(*it) + 40);
			if (NTHeader->OptionalHeader.SizeOfHeaders + (DWORD)pFileBuffer - (DWORD)pointer < 80)
			{
				printf("Unable to insert the new section list!\n");
				pointer = NULL;
				i = 0;
				return;
			}
			SECTION_LIST* Section = (SECTION_LIST*)pointer;
			SectionList.push_back(Section);
			NTHeader->FileHeader.NumberOfSections++;
			break;
		}
		i++;
	}

	//Clear the stub in the following SectionList 
	//(NTHeader->OptionalHeader.SizeOfHeaders + (DWORD)*pImageBuffer - (DWORD)pointer) is the old version and aborted now
	for (i = 0; i < 80; i++)
	{
		*((LPBYTE)((DWORD)pointer + i)) = 0;
	}

	//Insertion:
	//---------------------------------------------------------------------------------
	//TODO the code insertion here. This may vary among different types of insert codes

	//Copy the format shellcode in position
	for (i = 0; i < ShellCodeLen; i++)
	{
		*((LPBYTE)((DWORD)pInsertionBegin + i)) = shellCode[i];
	}

	//Calculate and fill in the MessageBox JMP address
	JMP = MessageBoxAddr - ((DWORD)pTempCode + 13 - (DWORD)*pImageBuffer + NTHeader->OptionalHeader.ImageBase);
	pTempCode = (LPVOID)((DWORD)pTempCode + 9);
	*((PDWORD)pTempCode) = JMP;
	pTempCode = pInsertionBegin;

	//Calculate and fill in the OEP JMP address
	JMP = NTHeader->OptionalHeader.AddressOfEntryPoint - ((DWORD)pTempCode + 18 - (DWORD)*pImageBuffer);
	pTempCode = (LPVOID)((DWORD)pTempCode + 14);
	*((PDWORD)pTempCode) = JMP;
	pTempCode = pInsertionBegin;
	printf("%x\n",(DWORD)pTempCode - (DWORD)*pImageBuffer);

	//Change the OEP and let it point to the beginning of the shellcode
	NTHeader->OptionalHeader.AddressOfEntryPoint = ((DWORD)pInsertionBegin - (DWORD)*pImageBuffer);

	//End the code insertion
	//---------------------------------------------------------------------------------

	//Set Characteristics for the new section list
	i = 0;
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		if (i == NTHeader->FileHeader.NumberOfSections - 1)
		{
			//Set Name
			strncpy((CHAR*)(*it)->Name, ".shell", strlen(".shell"));

			//Set VirtualSize
			(*it)->VirtualSize = ShellCodeLen;

			//Set VirtualAddress
			(*it)->VirtualAddress = ((DWORD)pInsertionBegin - (DWORD)*pImageBuffer);

			//Set SizeOfRawData
			(*it)->SizeOfRawData = (ShellCodeLen / NTHeader->OptionalHeader.FileAlignment + 1) * NTHeader->OptionalHeader.FileAlignment;

			//Set PointerToRawData
			(*it)->PointerToRawData = (DWORD)pInsertionBegin - (DWORD)*pImageBuffer - (NTHeader->OptionalHeader.SectionAlignment - NTHeader->OptionalHeader.FileAlignment) * (NTHeader->FileHeader.NumberOfSections);

			//Set PointerToRelocations
			(*it)->PointerToRelocations = 0;

			//Set PointerToLinenumbers
			(*it)->PointerToLinenumbers = 0;

			//Set NumberOfRelocations
			(*it)->NumberOfRelocations = 0;

			//Set NumberOfLinenumbers
			(*it)->NumberOfLinenumbers = 0;

			//Set Characteristics (Executable, writable, readable)
			(*it)->Characteristics = TextSectionCharacter;
		}
		i++;
	}
	pFileBuffer = NULL;
}

VOID AddNullNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewSection)
{

}

VOID InsertionToElongatedSection(IN LPSTR fileName, OUT LPVOID* pImageBuffer)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pTempBuffer = NULL;
	LPVOID pointer = NULL;
	LPVOID pInsertionBegin = NULL;
	LPVOID pTempCode = NULL;
	LPVOID pLastSectionInFile = NULL;
	LPVOID pLastSectionInRam = NULL;
	DWORD sizeOfLastSectionInFile = 0;
	DWORD sizeOfLastSectionInRam = 0;
	DWORD deltaSizeOfBuiltSection = 0;
	DWORD previousImageSize = 0;
	DWORD newSizeOfImage = 0;
	DWORD fileSize = 0;
	DWORD JMP = 0;
	DWORD i;
	DWORD j;

	//Clear the headers and sections info
	DosHeader = NULL;
	NTHeader = NULL;
	SectionList.clear();

	//Read the file from disk to ram
	fileSize = readPEFile(fileName, &pFileBuffer);
	if (!fileSize || !pFileBuffer)
	{
		printf("Unable to open the PE file!\n");
		return;
	}

	//Read the headers
	//To DosHeader
	pointer = pFileBuffer;
	DosHeader = (DOS_HEADER*)pointer;

	//To NTHeader
	pointer = (LPBYTE)((DWORD)DosHeader->e_lfanew + (DWORD)pFileBuffer);
	NTHeader = (NT_HEADER*)pointer;

	//To SectionList
	pointer = ((LPBYTE)pointer + 24 + NTHeader->FileHeader.SizeOfOptionalHeader);
	for (i = 0; i < NTHeader->FileHeader.NumberOfSections; i++)
	{
		SECTION_LIST* Section = (SECTION_LIST*)pointer;
		SectionList.push_back(Section);
		pointer = ((LPBYTE)pointer + 40);
	}

	//Get the size of last section
	i = 0;
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		if (i == NTHeader->FileHeader.NumberOfSections - 1)
		{
			sizeOfLastSectionInFile = (*it)->SizeOfRawData;
			sizeOfLastSectionInRam = (*it)->VirtualSize;
			pLastSectionInFile = (LPVOID)(*it)->PointerToRawData;
			pLastSectionInRam = (LPVOID)(*it)->VirtualAddress;
		}
		i++;
	}

	//Calculate the resized ImageBase (This is somehow aborted for SizeOfImage does not require an elongation)
	/*
	deltaSizeOfBuiltSection = ((ShellCodeLen + sizeOfLastSectionInRam + ReserveBytes) / NTHeader->OptionalHeader.SectionAlignment + 1) *
		NTHeader->OptionalHeader.SectionAlignment - sizeOfLastSectionInRam;
	previousImageSize = NTHeader->OptionalHeader.SizeOfImage;
	NTHeader->OptionalHeader.SizeOfImage += deltaSizeOfBuiltSection;
	newSizeOfImage = NTHeader->OptionalHeader.SizeOfImage;
	*/

	//Assign the space for ImageBuffer
	*pImageBuffer = malloc(NTHeader->OptionalHeader.SizeOfImage);
	if (!*pImageBuffer)
	{
		printf("Unable to assign the space for the imagebuffer!\n");
		return;
	}
	memset(*pImageBuffer, 0, NTHeader->OptionalHeader.SizeOfImage);

	//Copy the headers and sections into the ImageBuffer
	for (i = 0; i < NTHeader->OptionalHeader.SizeOfHeaders; i++)
	{
		*((LPBYTE)((DWORD)*pImageBuffer + i)) = *((LPBYTE)((DWORD)pFileBuffer + i));
	}

	//Locate the insertion position of shellcode
	i = 0;
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		for (j = 0; j < (*it)->SizeOfRawData; j++)
		{
			*((LPBYTE)((DWORD)*pImageBuffer + (*it)->VirtualAddress + j)) = *((LPBYTE)((DWORD)pFileBuffer + (*it)->PointerToRawData + j));
		}
		if (i == NTHeader->FileHeader.NumberOfSections - 1)
		{
			pInsertionBegin = (LPVOID)((DWORD)*pImageBuffer + (*it)->VirtualAddress + (*it)->VirtualSize + ReserveBytes);
			pTempCode = pInsertionBegin;
		}
		i++;
	}

	readHeaderInfo(*pImageBuffer);

	//Insertion:
	//---------------------------------------------------------------------------------
	//TODO the code insertion here. This may vary among different types of insert codes

	//Copy the format shellcode in position
	for (i = 0; i < ShellCodeLen; i++)
	{
		*((LPBYTE)((DWORD)pInsertionBegin + i)) = shellCode[i];
	}

	//Calculate and fill in the MessageBox JMP address
	JMP = MessageBoxAddr - ((DWORD)pTempCode + 13 - (DWORD)*pImageBuffer + NTHeader->OptionalHeader.ImageBase);
	pTempCode = (LPVOID)((DWORD)pTempCode + 9);
	*((PDWORD)pTempCode) = JMP;
	pTempCode = pInsertionBegin;

	//Calculate and fill in the OEP JMP address
	JMP = NTHeader->OptionalHeader.AddressOfEntryPoint - ((DWORD)pTempCode + 18 - (DWORD)*pImageBuffer);
	pTempCode = (LPVOID)((DWORD)pTempCode + 14);
	*((PDWORD)pTempCode) = JMP;
	pTempCode = pInsertionBegin;

	//Change the OEP and let it point to the beginning of the shellcode
	NTHeader->OptionalHeader.AddressOfEntryPoint = ((DWORD)pInsertionBegin - (DWORD)*pImageBuffer);

	//End the code insertion
	//---------------------------------------------------------------------------------


	//Set Characteristics for the new section list
	i = 0;
	for (std::list<SECTION_LIST*>::iterator it = SectionList.begin(); it != SectionList.end(); it++)
	{
		if (i == NTHeader->FileHeader.NumberOfSections - 1)
		{
			//Set VirtualSize
			(*it)->VirtualSize = sizeOfLastSectionInRam + ReserveBytes + ShellCodeLen;

			//Set SizeOfRawData
			(*it)->SizeOfRawData = ((sizeOfLastSectionInFile + ShellCodeLen + ReserveBytes) / NTHeader->OptionalHeader.FileAlignment + 1) * NTHeader->OptionalHeader.FileAlignment;
			
			//Set Characteristics (Executable, writable, readable)
			(*it)->Characteristics |= TextSectionCharacter;
		}
		i++;
	}
	free(pFileBuffer);
}

int main()
{
	LPVOID pImageBuffer = NULL;

//	FileBufferToImageBuffer("C:\\FileTest\\Changer\\winmine.exe", &pImageBuffer);

//	RaiseNTHeaderToStubRegion(pImageBuffer);

//	InsertionToCodeSection(pImageBuffer);

	InsertionToNewSection("C:\\FileTest\\Changer\\AcidBurn.exe", &pImageBuffer);

//	InsertionToElongatedSection("C:\\FileTest\\Changer\\AcidBurn.exe" ,&pImageBuffer);

	ImageBufferToNewBuffer("C:\\FileTest\\Changer\\Output.exe", pImageBuffer);

	free(pImageBuffer);

	return 0;
}

