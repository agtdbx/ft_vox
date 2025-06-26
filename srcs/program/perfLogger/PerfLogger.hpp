#ifndef PERF_LOGGER
#  define PERF_LOGGER

#include <chrono>

// TODO : REMOVE
struct PerfField
{
	std::clock_t	start = 0;
	float			total = 0.0f;
	int				nbCall = 0;

	PerfField	&operator+=(const PerfField &obj)
	{
		this->total += obj.total;
		this->nbCall += obj.nbCall;

		return (*this);
	}
};

void	perflogStart(PerfField &perfField);
void	perflogEnd(PerfField &perfField);
float	perflogAvg(PerfField &perfField);
void	perflogReset(PerfField &perfField);
void	perflogPrint(PerfField &perfField, const char *msg);
void	perflogPrint(PerfField &perfField);
void	perflogPrintCsv(PerfField &perfField, const char *msg);

struct PerfLogger
{
	// Generation
	PerfField	generation;
	PerfField	generateChunk;

	// Mesh
	PerfField	createMesh;
	PerfField	chunkMeshing;
	// Mesh border
	PerfField	meshChunk;
	// Mesh block
	PerfField	meshBlock;
	PerfField	meshBlockCopyBitmap;
	PerfField	meshBlockXaxis;
	PerfField	meshBlockYaxis;
	PerfField	meshBlockZaxis;
	// Mesh water
	PerfField	meshWater;

	// Buffer
	unsigned long int	chunkBufferSize = 0;
	PerfField	createBuffer;
	PerfField	createUpdateStaging;
	// Buffer vertex
	PerfField	mapVertexBuffer;
	PerfField	createVertexBuffer;
	PerfField	copyVertexBuffer;
	// Buffer index
	PerfField	mapIndexBuffer;
	PerfField	createIndexBuffer;
	PerfField	copyIndexBuffer;

	PerfLogger	&operator+=(const PerfLogger &obj)
	{
		// Generation
		this->generation += obj.generation;
		this->generateChunk += obj.generateChunk;

		// Mesh
		this->createMesh += obj.createMesh;
		this->chunkMeshing += obj.chunkMeshing;
		// Mesh border
		this->meshChunk += obj.meshChunk;
		// Mesh block
		this->meshBlock += obj.meshBlock;
		this->meshBlockCopyBitmap += obj.meshBlockCopyBitmap;
		this->meshBlockXaxis += obj.meshBlockXaxis;
		this->meshBlockYaxis += obj.meshBlockYaxis;
		this->meshBlockZaxis += obj.meshBlockZaxis;
		// Mesh water
		this->meshWater += obj.meshWater;

		// Buffer
		this->createBuffer += obj.createBuffer;
		this->createUpdateStaging += obj.createUpdateStaging;
		// Buffer vertex
		this->mapVertexBuffer += obj.mapVertexBuffer;
		this->createVertexBuffer += obj.createVertexBuffer;
		this->copyVertexBuffer += obj.copyVertexBuffer;
		// Buffer index
		this->mapIndexBuffer += obj.mapIndexBuffer;
		this->createIndexBuffer += obj.createIndexBuffer;
		this->copyIndexBuffer += obj.copyIndexBuffer;

		return (*this);
	}
};

#endif
