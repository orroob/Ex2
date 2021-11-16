#include <string.h>
#include <Windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		printf("Arguments Error\n");
		return 1;
	}

	int schoolNum = atoi(argv[1]);

	return 0;
}