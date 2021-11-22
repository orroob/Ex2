#define _CRT_SECURE_NO_WARNINGS
#define READ 0
#define WRITE 1

#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "FileHandling.h"
#include "ProcessHandling.h"


typedef struct openAllFiles_Arguments
{
	HANDLE allHandles[5];
	int index;
}Args;

/// <summary>
/// This function closes all open handles before exiting the code.
/// </summary>
/// <param name="hfiles"> - a HANDLEs array, holding 1-3 handles to be closed.</param>
/// <param name="handlesNum"> - the number of handles to be closed.</param>
/// <returns>Returns 1 if failed, 0 otherwise.</returns>
int exitCode(HANDLE hfiles[], int handlesNum)
{
	int exitCode = 0;
	for (int i = 0; i < handlesNum; i++)
	{
		exitCode |= closeFile(&hfiles[i]);
	}
	return exitCode;
}

int calcDigitsNum(int num)
{
	int count = 0;

	if (num == 0)
		return 1;

	while (num != 0)
	{
		num /= 10;
		count++;
	}
	return count;
}

int threadExecute(int index, int weights[])
{
	HANDLE allHandles[5]; // = { RealFile , HumanFile , EngFile, EvalFile, ResultsFile };

	if (openAllFiles(allHandles, index) != 5)
	{
		//not all files were opened successfuly
		return 1;
	}
	
	char* allFilesData[4];// = { RealFileData, HumanFileData, EngFileData, EvalFileData, ResultsFileData };
	int size = 0;

	for (int i = 0; i < 4; i++)
	{
		size = GetFileSize(allHandles[i], NULL);
		if (size == INVALID_FILE_SIZE)
			break;
		allFilesData[i] = malloc(sizeof(char*) * size);
		
		readFileSimple(allHandles[i], allFilesData[i], size);
	}

	char *RealFileData, *HumanFileData, *EngFileData, *EvalFileData, *ResultsFileData;
	RealFileData = calloc(5, sizeof(char));
	HumanFileData = calloc(5, sizeof(char));
	EngFileData = calloc(5, sizeof(char));
	EvalFileData = calloc(5, sizeof(char));
	int maxSize = 10;
	char* tok1 = NULL, tok2 = NULL, tok3 = NULL, tok4 = NULL;
	const char delim[] = "\n";
	int grades[4] = { 0 };
	/*
	RealFileData = strtok_s(allFilesData[0], delim, &tok1);
	HumanFileData = strtok_s(allFilesData[1], delim, &tok2);
	EngFileData = strtok_s(allFilesData[2], delim, &tok3);
	EvalFileData = strtok_s(allFilesData[3], delim, &tok4);

	grades[0] = strtol(RealFileData, NULL, 10);
	grades[1] = strtol(HumanFileData, NULL, 10);
	grades[2] = strtol(EngFileData, NULL, 10);
	grades[3] = strtol(EvalFileData, NULL, 10);

	int result = calcAvg(weights, grades);
	char result_str[6] = { 0 };
	sprintf(result_str, "%d\r\n", result);

	if (WriteToFile(allHandles[4], result_str, strlen(result_str)))
	{
		free(RealFileData);
		free(HumanFileData);
		free(EngFileData);
		free(EvalFileData);
		return 1;
	}
	*/
	int result = 0;
	char result_str[6] = { 0 };
	while ((RealFileData) != NULL)
	{	
		//RealFileData = strtok_s(NULL, delim, &tok1);
		//HumanFileData = strtok_s(NULL,delim, &tok2);
		//EngFileData = strtok_s(NULL, delim, &tok3);
		//EvalFileData = strtok_s(NULL, delim, &tok4);
		RealFileData = strtok_s(allFilesData[0], delim, &allFilesData[0]);
		HumanFileData = strtok_s(allFilesData[1], delim, &allFilesData[1]);
		EngFileData = strtok_s(allFilesData[2], delim, &allFilesData[2]);
		EvalFileData = strtok_s(allFilesData[3], delim, &allFilesData[3]);

		if (RealFileData == NULL)
			break;
		grades[0] = strtol(RealFileData, NULL, 10);
		grades[1] = strtol(HumanFileData, NULL, 10);
		grades[2] = strtol(EngFileData, NULL, 10);
		grades[3] = strtol(EvalFileData, NULL, 10);

		result = calcAvg(weights, grades);
		sprintf(result_str, "%d\r\n", result);

		if (WriteToFile(allHandles[4], result_str, strlen(result_str)))
		{
			free(RealFileData);
			free(HumanFileData);
			free(EngFileData);
			free(EvalFileData);
			return 1;
		}
	}
	free(RealFileData);
	free(HumanFileData);
	free(EngFileData);
	free(EvalFileData);

	//for (int i = 0; i < 4; i++)
	//{
	//	free(allFilesData[i]);
	//}
	return exitCode(allHandles, 5);
}



int readFileSimple(HANDLE hfile, char *buffer, int size)
{
	//DWORD size = GetFileSize(hfile, NULL);
	if (ReadFromFile(hfile, buffer, size))
	{
		return 1;
	}

	return 0;
}

int openFileSimple(HANDLE *hfile, char* fileName, int index, int format)
{

	int a = (strlen(".////.txt") + 2*strlen(fileName) + calcDigitsNum(index))+1;
	char* buffer;
	buffer = malloc(sizeof(char*) * (strlen(".////.txt") + 2*strlen(fileName) + calcDigitsNum(index))+1);
	if (buffer == NULL)
	{
		//printf("error allocating memory\n");
		return 1;
	}
	sprintf_s(buffer, (strlen(".////.txt") + 2*strlen(fileName) + calcDigitsNum(index)), ".\\%s\\%s%d.txt", fileName,fileName, index);
	
	if (openFile(hfile, buffer, format))
	{
		//first file failed
		free(buffer);
		return 1;
	}
	free(buffer);
	return 0;
}

int openAllFiles(HANDLE *allHandles, int index)
{
	if (openFileSimple(&(allHandles[0]), "Real", index, READ))
	{
		return 0;
	}

	if (openFileSimple(&(allHandles[1]), "Human", index, READ))
	{
		return 1;
	}
	
	if (openFileSimple(&(allHandles[2]), "Eng", index, READ))
	{
		return 2;
	}

	if (openFileSimple(&(allHandles[3]), "Eval", index, READ))
	{
		return 3;
	}
	if (openFileSimple(&(allHandles[4]), "Results", index, WRITE))
	{
		return 4;
	}

	return 5;
}

int calcAvg(int weights[], int grades[])
{
	float avg = 0;
	float weight_sum=0;
	for (int i = 0; i < 4; i++)
	{
		avg += weights[i] * grades[i];
		weight_sum += weights[i];
	}
	return (int)(avg / weight_sum);
}

int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		printf("Arguments Error\n");
		return 1;
	}

	HANDLE RealFile = NULL;				// handle to the plain text file
	HANDLE HumanFile = NULL;			// handle to the key text file
	HANDLE EngFile = NULL;				// handle to the plain text file
	HANDLE EvalFile = NULL;				// handle to the plain text file
	HANDLE ResultsFile = NULL;			// handle to the plain text file

	
	//if (createDir("Results"))
	//{
	//	return 1;
	//}

	HANDLE threadHandle = NULL;
	HANDLE allHandles[5] = { RealFile , HumanFile , EngFile, EvalFile, ResultsFile };
	Args arguments = { allHandles, 0 };
	DWORD arr[10];

	int weights[] = { 40, 35, 20, 5 };
	//int count = openAllFiles(allHandles, 0);
	threadExecute(0, weights);

	//openThread(&threadHandle, &openAllFiles, &arguments, &arr[0]);
	//WaitForMultipleObjects(1, &threadHandle, 1, INFINITE);
	//WaitForSingleObject(threadHandle, 10000000);
	//closeFile(&threadHandle);
	
	//return (count!=5)?1:exitCode(allHandles, count);

	//HANDLE hfiles[5] = { 0 }; //array of the handles to use when exitting the code

	int schoolNum = strtol(argv[1], NULL, 10);


	//if (openFile(&RealFile, ".\\Real\\Real0.txt", READ))
	//{
	//	//first file failed
	//	return(1);
	//}
	//hfiles[0] = RealFile;
	//
	//if (openFile(&HumanFile, ".\\Human\\Human0.txt", READ))
	//{
	//	//first file succeeded, second file failed
	//	return 1 || exitCode(hfiles, 1);
	//}
	//hfiles[1] = HumanFile;
	//
	//if (openFile(&EngFile, ".\\Eng\\Eng0.txt", READ))
	//{
	//	//first and second file succeeded, third file failed
	//	return 1 || exitCode(hfiles, 2);
	//}
	//hfiles[2] = EngFile;
	//
	//if (openFile(&EvalFile, ".\\Eval\\Eval0.txt", READ))
	//{
	//	//first and second file succeeded, third file failed
	//	return 1 || exitCode(hfiles, 2);
	//}
	//hfiles[3] = EvalFile;
	//
	//if (openFile(&ResultsFile, ".\\Results\\Results0.txt", WRITE))
	//{
	//	//first and second file succeeded, third file failed
	//	return 1 || exitCode(hfiles, 2);
	//}
	//hfiles[4] = ResultsFile;

	//return exitCode(allHandles, 5);

	
	return 0;
}