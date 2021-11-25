#define _CRT_SECURE_NO_WARNINGS
#define READ 0
#define WRITE 1
 #define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "FileHandling.h"
#include "ProcessHandling.h"
/*
HW2
Philip Dolav 322656273
Or Roob      212199970
*/


int weights[4]; // Global Parameter that each Thread gets from cmd

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

/// <summary>
/// This function extracts number of digits. for example 100/10 = 3
/// </summary>
/// <param name="num"> Grade getting smaller each time</param>
/// <returns> Returns Number of digits</returns>
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

/// <summary>
/// this function opens all files needed for the specific Thread with its index,calculates the average student by student and then
///writes all of the grades to Results#index file
/// </summary>
/// <param name="index"> School Number </param>
/// <returns> The function returns 0 if succeeded, 1 otherwise </returns>
DWORD WINAPI threadExecute(int index)
{

	HANDLE allHandles[5]; //{ RealFile , HumanFile , EngFile, EvalFile, ResultsFile }

	if (openAllFiles(allHandles, index) != 5)
	{
		//not all files were opened successfuly
		return 1;
	}

	char* allFilesData[4];// { RealFileData, HumanFileData, EngFileData, EvalFileData, ResultsFileData }
	int size = 0;

	for (int i = 0; i < 4; i++)
	{
		size = GetFileSize(allHandles[i], NULL);
		if (size == INVALID_FILE_SIZE)
			break;
		allFilesData[i] = malloc(sizeof(char*) * size);
		if (allFilesData[i] == NULL)
		{
			printf("error allocating memory\n");
			return 1;
		}
		if (readFileSimple(allHandles[i], allFilesData[i], size))
		{
			printf("Error reading from file\n");
			return 1;
		}
		
	}

	char* RealFileData = NULL, * HumanFileData = NULL, * EngFileData = NULL, * EvalFileData = NULL, * ResultsFileData = NULL;
	
	const char delim[] = "\n";
	int grades[4] = { 0 };
	int result = 0;
	char result_str[6] = { 0 };

	char* temp0 = allFilesData[0], * temp1 = allFilesData[1], * temp2 = allFilesData[2], * temp3 = allFilesData[3];

	do// While we didnt get to the end of our Files
	{	// Start parsing the file grade by grade with Specified Delimiter \n
		RealFileData = strtok_s(allFilesData[0], delim, &allFilesData[0]);
		HumanFileData = strtok_s(allFilesData[1], delim, &allFilesData[1]);
		EngFileData = strtok_s(allFilesData[2], delim, &allFilesData[2]);
		EvalFileData = strtok_s(allFilesData[3], delim, &allFilesData[3]);

		if (RealFileData == NULL)
			break;
		grades[0] = strtol(RealFileData, NULL, 10); // convert to int
		grades[1] = strtol(HumanFileData, NULL, 10);
		grades[2] = strtol(EngFileData, NULL, 10);
		grades[3] = strtol(EvalFileData, NULL, 10);

		result = calcAvg(weights, grades); // Calculate Average 

		sprintf(result_str, "%d\r\n", result);  //adding School Number to the end of file name.

		if (WriteToFile(allHandles[4], result_str, strlen(result_str))) // Free Memory and return if failed writing
		{
			free(temp0);
			free(temp1);
			free(temp2);
			free(temp3);
			return 1;
		}
	} while ((RealFileData) != NULL);
	
	free(temp0);
	free(temp1);
	free(temp2);
	free(temp3);

	return exitCode(allHandles, 5);
}

/// <summary>
/// This function reads data from a file and places it to a buffer, using winAPI's ReadFile function. Failure can be caused by ReadFile failure, or when the read data's length isn't equal to the given messageLen.
/// </summary>
/// <param name="hfile"> - a HANDLE object to the file that is read.</param>
/// <param name="buffer"> - a char* object containing the data that will be written.</param>
/// <param name="size"> - an int containing the message's length. (should be equal to strlen(buffer)).</param>
/// <returns> The function returns 0 if succeeded, 1 otherwise.</returns> 
int readFileSimple(HANDLE hfile, char* buffer, int size)
{
	if (ReadFromFile(hfile, buffer, size))
	{
		return 1;
	}
	return 0;
}

/// <summary>
/// allocating memmory for the buffer and reading the file
/// </summary>
/// <param name="hfile"> a HANDLE* object to which the function will place the created file handle. should be initiated to NULL.</param>
/// <param name="fileName">a char* object containing the file's name</param>
/// <param name="index">School Number</param>
/// <param name="format">desired access mode to the file. 0-read; 1-write</param>
/// <returns>returns 0 if secceeded, 1 otherwise</returns>
int openFileSimple(HANDLE* hfile, char* fileName, int index, int format)
{
	char* buffer;
	buffer = malloc(sizeof(char*) * (strlen(".////.txt") + 2 * strlen(fileName) + calcDigitsNum(index)) + 1);
	if (buffer == NULL)
	{
		printf("error allocating memory\n");
		return 1;
	}

	//length of the file's directory
	sprintf_s(buffer, (strlen(".////.txt") + 2 * strlen(fileName) + calcDigitsNum(index)), ".\\%s\\%s%d.txt", fileName, fileName, index);

	if (openFile(hfile, buffer, format))
	{
		//first file failed
		free(buffer);
		return 1;
	}
	free(buffer);
	return 0;
}

/// <summary>
/// Calling Function that reads/writes to/from a file
/// </summary>
/// <param name="allHandles">  a HANDLE* object containing Handles to all Files</param>
/// <param name="index"> School Number</param>
/// <returns>Returns number of files opened. Value less than 5 isnt good</returns>
int openAllFiles(HANDLE* allHandles, int index)
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

/// <summary>
/// Calculating the Weighted arithmetic mean
/// </summary>
/// <param name="weights"> weights of each subject from cmd</param>
/// <param name="grades">array of grades for a specific student </param>
/// <returns>returns an int Average to be Written into Results file</returns>
int calcAvg(int weights[], int grades[])
{
	float avg = 0;
	float weight_sum = 0;
	for (int i = 0; i < 4; i++)
	{
		avg += (weights[i] * 0.01) * grades[i];
		weight_sum += weights[i];
	}
	return (int)(avg); // Converting from float to int.
}

/// <summary>
/// Creating Results Directory, initializing all Handles to NULL, extracting values from cmd and Invoking our main func which opens threads
/// </summary>
/// <param name="argc"> - number of arguments </param>
/// <param name="argv"> - a Char* type containing all of the arguments given</param>
/// <returns>returns 0 if secceeded, 1 otherwise</returns>
int main(int argc, char* argv[])
{
	if (argc != 6) //checking enough arguments
	{
		printf("Arguments Error\n");
		return 1;
	}

	//Create specified directory
	if (createDir(".\\Results"))
	{
		printf("Error while creating directory\n");
		return 1;
	}

	HANDLE RealFile = NULL;				// handle to the "RealFile" text file
	HANDLE HumanFile = NULL;			// handle to the "HumanFile" text file
	HANDLE EngFile = NULL;				// handle to the "EngFile" text file
	HANDLE EvalFile = NULL;				// handle to the "EvalFile" text file
	HANDLE ResultsFile = NULL;			// handle to the "ResultsFile" text file

	HANDLE threadHandles[10] = { NULL };
	HANDLE allHandles[5] = { RealFile , HumanFile , EngFile, EvalFile, ResultsFile };
	DWORD threadIDs[10];

	// put all arguments (the weights) in global variables for the threads' usage.
	weights[0] = strtol(argv[2], NULL, 10);		//converting String to int
	weights[1] = strtol(argv[3], NULL, 10);		
	weights[2] = strtol(argv[4], NULL, 10);
	weights[3] = strtol(argv[5], NULL, 10);

	int* indexes;									//array holding the thread's indexes
	int schoolNum = strtol(argv[1], NULL, 10);		//converting String to int
	indexes = malloc(schoolNum * sizeof(int));
	if (indexes == NULL)
	{
		printf("error allocating memory\n");
		return 1;
	}

	int i, j;
	for (i = 0; i < schoolNum; i += 10) // first loop each time add 10 Threads, until all results are calculated
	{
		for (j = 0; j < 10; j++) // Open Max 10 Threads 
		{
			if (i + j >= schoolNum) // in case the number of schools does not divide by 10, stop opening threads
				break;

			indexes[i + j] = i + j;
			if (openThread(&threadHandles[j], &threadExecute, indexes[i + j], &threadIDs[j]))
			{
				printf("error opening Thread %d\n", j);	//Thread won't open, Inform the User and Continue
				continue;
			}
		}
		WaitForMultipleObjects(j, threadHandles, 1, 10000000);		// Wait for all threads to finish before continuing

		for (int k = 0; k < j; k++)	// Close all opened threads
		{
			if (closeProcess(&(threadHandles[k])))
			{
				printf("error closing Thread%d\n", k);	//Thread wont close, Inform the User and Continue
				continue;
			}
		}
	}

	free(indexes);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int a =  _CrtDumpMemoryLeaks();
	return 0;
}