#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "FileHandling.h"

#define READ 0
#define WRITE 1

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

int openAllFiles(HANDLE *allHandles, int index)
{
	if (openFile(&allHandles[0], ".\\Real\\Real0.txt", READ))
	{
		//first file failed
		return 0;
	}

	if (openFile(&allHandles[1], ".\\Human\\Human0.txt", READ))
	{
		//first file succeeded, second file failed
		return 1;
	}

	if (openFile(&allHandles[2], ".\\Eng\\Eng0.txt", READ))
	{
		//first and second file succeeded, third file failed
		return 2;
	}

	if (openFile(&allHandles[3], ".\\Eval\\Eval0.txt", READ))
	{
		//first and second file succeeded, third file failed
		return 3;
	}

	if (openFile(&allHandles[4], ".\\Results\\Results0.txt", WRITE))
	{
		//first and second file succeeded, third file failed
		return 4;
	}
	return 5;
}

int calcAvg(int weights[], int grades[])
{
	float avg = 0;
	for (int i = 0; i < 4; i++)
	{
		avg += weights[i] * grades[i];
	}
	return (int)(avg / 4);
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

	HANDLE allHandles[5] = { RealFile , HumanFile , EngFile, EvalFile, ResultsFile };

	createDir("Results");

	int count = openAllFiles(allHandles, 0);
	return (count!=5)?1:exitCode(allHandles, count);

	HANDLE hfiles[5] = { 0 }; //array of the handles to use when exitting the code

	int schoolNum = strtol(argv[1], NULL, 10);


	if (openFile(&RealFile, ".\\Real\\Real0.txt", READ))
	{
		//first file failed
		return(1);
	}
	hfiles[0] = RealFile;

	if (openFile(&HumanFile, ".\\Human\\Human0.txt", READ))
	{
		//first file succeeded, second file failed
		return 1 || exitCode(hfiles, 1);
	}
	hfiles[1] = HumanFile;

	if (openFile(&EngFile, ".\\Eng\\Eng0.txt", READ))
	{
		//first and second file succeeded, third file failed
		return 1 || exitCode(hfiles, 2);
	}
	hfiles[2] = EngFile;

	if (openFile(&EvalFile, ".\\Eval\\Eval0.txt", READ))
	{
		//first and second file succeeded, third file failed
		return 1 || exitCode(hfiles, 2);
	}
	hfiles[3] = EvalFile;

	if (openFile(&ResultsFile, ".\\Results\\Results0.txt", WRITE))
	{
		//first and second file succeeded, third file failed
		return 1 || exitCode(hfiles, 2);
	}
	hfiles[4] = ResultsFile;

	return exitCode(hfiles, 5);

	
	return 0;
}