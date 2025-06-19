#include <program/perfLogger/PerfLogger.hpp>

#include  <iostream>

const float	INV_CLOCKS_PER_USEC = 1.0 / (float)CLOCKS_PER_SEC * 1000000.0;

// TODO : Remove
void	perflogStart(PerfField &perfField)
{
	perfField.start = std::clock();
}

void	perflogEnd(PerfField &perfField)
{
	perfField.total += (std::clock() - perfField.start) * INV_CLOCKS_PER_USEC;
	perfField.nbCall++;
}


float	perflogAvg(PerfField &perfField)
{
	if (perfField.nbCall == 0)
		return  (0.0f);
	return (perfField.total / (float)perfField.nbCall);
}


void	perflogReset(PerfField &perfField)
{
	perfField.start = 0.0;
	perfField.total = 0.0;
	perfField.nbCall = 0;
}


void	perflogPrint(PerfField &perfField, const char *msg)
{
	if (perfField.nbCall == 0)
	{
		printf("total 0 us\n");
		return ;
	}

	if (perfField.nbCall == 1)
	{
		printf("total %.2f us\n", perfField.total);
		return ;
	}

	float	avg = perfField.total / (float)perfField.nbCall;
	printf("%s : total %.2f us, nb call %i, avg %.2f us\n",
			msg, perfField.total, perfField.nbCall, avg);
}


void	perflogPrint(PerfField &perfField)
{
	if (perfField.nbCall == 0)
	{
		printf("total 0 us\n");
		return ;
	}

	if (perfField.nbCall == 1)
	{
		printf("total %.2f us\n", perfField.total);
		return ;
	}

	float	avg = perfField.total / (float)perfField.nbCall;
	printf("total %.2f us, nb call %i, avg %.2f us\n",
			perfField.total, perfField.nbCall, avg);
}
