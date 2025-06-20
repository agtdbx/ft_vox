#ifndef PERF_LOGGER
#  define PERF_LOGGER

#include <chrono>

// TODO : REMOVE
struct PerfField
{
	std::clock_t	start = 0;
	float			total = 0.0f;
	int				nbCall = 0;
};

void	perflogStart(PerfField &perfField);
void	perflogEnd(PerfField &perfField);
float	perflogAvg(PerfField &perfField);
void	perflogReset(PerfField &perfField);
void	perflogPrint(PerfField &perfField, const char *msg);
void	perflogPrint(PerfField &perfField);

struct PerfLogger
{
	PerfField	generation;
	PerfField	generateInMap;
	PerfField	generateInit;
	PerfField	generateChunk;


	PerfField	createMesh;
	PerfField	chunkMeshing;

	PerfField	meshChunk;

	PerfField	meshBlock;
	PerfField	meshBlockXaxis;
	PerfField	meshBlockYaxis;
	PerfField	meshBlockZaxis;

	PerfField	meshWater;


	PerfField	createBuffer;
};

#endif
