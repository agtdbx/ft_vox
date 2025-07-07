#include <program/map/Chunk.hpp>

#include <program/map/Map.hpp>

#include <unordered_map>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Chunk::Chunk(void)
{
	this->chunkId = gm::Vec2i(0);
	this->chunkPosition = gm::Vec3f(0);
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = CUBE_AIR;
	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		this->cubeBitmap.axisX[i] = 0;
		this->cubeBitmap.axisZ[i] = 0;
	}
	this->initDone = false;
	this->generationDone = false;
	this->meshCreate = false;
	this->bufferCreate = false;
	this->canBeDraw = false;

	int	halfChunkSize = CHUNK_SIZE / 2;
	int	halfChunkHeight = CHUNK_HEIGHT / 2;
	this->boundingCube.center = gm::Vec3f(halfChunkSize, halfChunkHeight, halfChunkSize);
	this->boundingCube.extents = gm::Vec3f(halfChunkSize, halfChunkHeight, halfChunkSize);
}


Chunk::Chunk(const Chunk &obj)
{
	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = obj.cubes[i];
	this->cubeBitmap = obj.cubeBitmap;
	this->generationDone = false;
	this->meshCreate = false;
	this->bufferCreate = false;
	this->canBeDraw = false;
	this->mesh = obj.mesh;
	this->boundingCube = obj.boundingCube;
}

//---- Destructor --------------------------------------------------------------

Chunk::~Chunk()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

const BoundingCube	&Chunk::getBoundingCube(void) const
{
	return (this->boundingCube);
}


CubeBitmap	&Chunk::getCubeBitmap(void)
{
	return (this->cubeBitmap);
}


Cube	Chunk::getCube(int x, int y, int z) const
{
	if (x < 0 || x >= CHUNK_SIZE
		|| y < 0 || y >= CHUNK_HEIGHT
		|| z < 0 || z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2]);
}


Cube	Chunk::getCube(const gm::Vec3i &pos) const
{
	if (pos.x < 0 || pos.x >= CHUNK_SIZE
		|| pos.y < 0 || pos.y >= CHUNK_HEIGHT
		|| pos.z < 0 || pos.z >= CHUNK_SIZE)
		return (CUBE_AIR);
	return (this->cubes[pos.x + pos.z * CHUNK_SIZE + pos.y * CHUNK_SIZE2]);
}


const Cube	&Chunk::at(int x, int y, int z) const
{
	return (this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2]);
}


const Cube	&Chunk::at(const gm::Vec3i &pos) const
{
	return (this->cubes[pos.x + pos.z * CHUNK_SIZE + pos.y * CHUNK_SIZE2]);
}


bool	Chunk::isMeshCreated(void) const
{
	return (this->meshCreate);
}


VkDeviceSize	Chunk::getBufferSize(void) const
{
	return (this->mesh.getBufferSize()
				+ this->borderMesh.getBufferSize()
				+ this->waterMesh.getBufferSize());
}

//---- Setters -----------------------------------------------------------------

void	Chunk::setCube(int x, int y, int z, Cube cube)
{
	if (x < 0 || x >= CHUNK_SIZE
		|| y < 0 || y >= CHUNK_HEIGHT
		|| z < 0 || z >= CHUNK_SIZE)
		return ;

	this->cubes[x + z * CHUNK_SIZE + y * CHUNK_SIZE2] = cube;
	if (cube == CUBE_AIR || cube == CUBE_WATER)
		this->cubeBitmap.set(x, y, z, false);
	else
		this->cubeBitmap.set(x, y, z, true);
}


void	Chunk::setDrawable(bool canBeDraw)
{
	this->canBeDraw = canBeDraw;
}

//---- Operators ---------------------------------------------------------------

Chunk	&Chunk::operator=(const Chunk &obj)
{
	if (this == &obj)
		return (*this);

	this->chunkId = obj.chunkId;
	this->chunkPosition = obj.chunkPosition;

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		this->cubes[i] = obj.cubes[i];

	this->mesh = obj.mesh;

	for (int i = 0; i < CHUNK_MASK_SIZE; i++)
	{
		this->cubeBitmap.axisX[i] = obj.cubeBitmap.axisX[i];
		this->cubeBitmap.axisZ[i] = obj.cubeBitmap.axisZ[i];
	}

	this->boundingCube = obj.boundingCube;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Chunk::init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader)
{
	if (this->initDone)
		return ;
	this->initDone = true;

	chunkShader.shader.initShaderParam(engine, this->shaderParam, {"cubes"});
	chunkShader.shaderWater.initShaderParam(engine, this->shaderParamWater, {"cubes"});
	chunkShader.shaderFdf.initShaderParam(engine, this->shaderParamFdf);
	chunkShader.shaderFdf.initShaderParam(engine, this->shaderParamFdfWater);
	chunkShader.shaderBorder.initShaderParam(engine, this->shaderParamBorder);

	this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;
}


void	Chunk::createMeshes(Map &map, PerfLogger &perfLogger)
{
	if (this->meshCreate)
		return ;
	this->meshCreate = true;

	perflogStart(perfLogger.chunkMeshing);
	this->createBorderMesh(perfLogger);
	this->createMesh(map, perfLogger);
	this->createWaterMesh(perfLogger);
	perflogEnd(perfLogger.chunkMeshing);

	this->mesh.setPosition(this->chunkPosition);
	this->uboPos.model = this->mesh.getModel();
	this->uboPos.pos = gm::Vec4f(this->mesh.getPosition());
}


void	Chunk::createBuffers(
				VulkanCommandPool &commandPool,
				StagingBuffer &stagingBuffer,
				VkCommandBuffer &commandBuffer,
				PerfLogger &perfLogger)
{
	if (this->bufferCreate)
		return ;
	this->bufferCreate = true;
	this->canBeDraw = true;

	perflogStart(perfLogger.createBuffer);

	this->mesh.createBuffers(commandPool, stagingBuffer, commandBuffer, perfLogger);
	this->borderMesh.createBuffers(commandPool, stagingBuffer, commandBuffer, perfLogger);
	this->waterMesh.createBuffers(commandPool, stagingBuffer, commandBuffer, perfLogger);

	perflogEnd(perfLogger.createBuffer);
}


void	Chunk::updateMesh(Engine &engine, Map &map)
{
	PerfLogger    perfLogger;

	this->mesh.destroy();
	this->createMesh(map, perfLogger);
	this->mesh.createBuffers(engine.commandPool);
}


void	Chunk::draw(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	this->uboPos.view = camera.getView();

	if (!this->canBeDraw)
		return ;

	// Draw mesh
	if (!chunkShader.shaderFdfEnable)
	{
		this->shaderParam.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->mesh, chunkShader.shader, this->shaderParam);
	}
	else
	{
		this->shaderParamFdf.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->mesh, chunkShader.shaderFdf, this->shaderParamFdf);
	}

	if (chunkShader.shaderBorderEnable)
	{
		this->shaderParamBorder.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->borderMesh, chunkShader.shaderBorder, this->shaderParamBorder);
	}
}


void	Chunk::drawWater(Engine &engine, Camera &camera, ChunkShader &chunkShader)
{
	if (this->waterMesh.getNbIndex() == 0)
		return ;

	// Draw mesh
	if (!chunkShader.shaderFdfEnable)
	{
		this->shaderParamWater.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->waterMesh, chunkShader.shaderWater, this->shaderParamWater);
	}
	else
	{
		this->shaderParamFdfWater.updateBuffer(engine.window, &this->uboPos, 0);
		engine.window.drawMesh(this->waterMesh, chunkShader.shaderFdf, this->shaderParamFdfWater);
	}
}


void	Chunk::destroy(Engine &engine)
{
	this->shaderParam.destroy(engine);
	this->shaderParamWater.destroy(engine);
	this->shaderParamFdf.destroy(engine);
	this->shaderParamFdfWater.destroy(engine);
	this->shaderParamBorder.destroy(engine);
	this->mesh.destroy();
	this->waterMesh.destroy();
	this->borderMesh.destroy();
	this->canBeDraw = false;
	this->bufferCreate = false;
	this->meshCreate = false;
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
