#ifndef CHUNK_HPP
# define CHUNK_HPP

# include <engine/engine.hpp>
# include <engine/mesh/Mesh.hpp>
# include <engine/mesh/VertexPos.hpp>
# include <engine/mesh/VertexVoxel.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/camera/Camera.hpp>
# include <engine/camera/BoundingCube.hpp>
# include <program/shaderStruct.hpp>
# include <program/map/Cube.hpp>
# include <program/map/CubeBitmap.hpp>
# include <program/map/PerlinNoise.hpp>

using ChunkMesh = Mesh<VertexVoxel>;
using ChunkBorderMesh = Mesh<VertexPos>;

class Map;

struct ChunkShader
{
	Shader	shader;
	Shader	shaderWater;
	Shader	shaderFdf;
	Shader	shaderBorder;
	bool	shaderFdfEnable;
	bool	shaderBorderEnable;

	/**
	 * @brief Destroy chunk shaders
	 *
	 * @param engine Engine struct.
	 */
	void	destroy(Engine &engine)
	{
		this->shader.destroy(engine);
		this->shaderWater.destroy(engine);
		this->shaderFdf.destroy(engine);
		this->shaderBorder.destroy(engine);
	}
};

/**
 * @brief Chunk class.
 */
class Chunk
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Chunk class.
	 *
	 * @return The default Chunk.
	 */
	Chunk(void);
	/**
	 * @brief Copy constructor of Chunk class.
	 *
	 * @param obj The Chunk to copy.
	 *
	 * @return The Chunk copied from parameter.
	 */
	Chunk(const Chunk &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Chunk class.
	 */
	~Chunk();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of bounding cube.
	 *
	 * @return The const reference to bounding cube.
	 */
	const BoundingCube	&getBoundingCube(void) const;
	/**
	 * @brief Getter of cube bitmap.
	 *
	 * @return The reference to cube bitmap.
	 */
	CubeBitmap	&getCubeBitmap(void);
	/**
	 * @brief Getter of cube in a chunk.
	 *
	 * @param x X coordonate of the cube in the chunk.
	 * @param y Y coordonate of the cube in the chunk.
	 * @param z Z coordonate of the cube in the chunk.
	 *
	 * @return The cube at the coordonate, or air in case of invalid coordonates.
	 */
	Cube	getCube(int x, int y, int z) const;
	/**
	 * @brief Getter of cube in a chunk.
	 *
	 * @param xpos Coordonate of the cube in the chunk.
	 *
	 * @return The cube at the coordonate, or air in case of invalid coordonates.
	 */
	Cube	getCube(const gm::Vec3i &pos) const;
	/**
	 * @brief Fast unsafe getter of cube in a chunk.
	 *
	 * @param x X coordonate of the cube in the chunk.
	 * @param y Y coordonate of the cube in the chunk.
	 * @param z Z coordonate of the cube in the chunk.
	 *
	 * @warning Will crash if coordonates are wrong.
	 *
	 * @return Const cube reference at the coordonate.
	 */
	const Cube	&at(int x, int y, int z) const;
	/**
	 * @brief Fast unsafe getter of cube in a chunk.
	 *
	 * @param xpos Coordonate of the cube in the chunk.
	 *
	 * @warning Will crash if coordonates are wrong.
	 *
	 * @return Const cube reference at the coordonate.
	 */
	const Cube	&at(const gm::Vec3i &pos) const;
	/**
	 * @brief Method to know if the mesh is create or not.
	 *
	 * @return true if the mesh is create, false else.
	 */
	bool	isMeshCreated(void) const;
	/**
	 * @brief Getter of buffer size of all meshes.
	 *
	 * @return Buffer size.
	 */
	VkDeviceSize	getBufferSize(void) const;

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Setter of cube in a chunk. Does nothing in case of invalid coordonates.
	 *
	 * @param x X coordonate of the cube in the chunk.
	 * @param y Y coordonate of the cube in the chunk.
	 * @param z Z coordonate of the cube in the chunk.
	 * @param cube The new cube to set.
	 */
	void	setCube(int x, int y, int z, Cube cube);
	/**
	 * @brief Setter of cube draw safeness.
	 *
	 * @param canBeDraw New draw status.
	 */
	void	setDrawable(bool canBeDraw);

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Chunk class.
	 *
	 * @param obj The Chunk to copy.
	 *
	 * @return The Chunk copied from parameter.
	 */
	Chunk	&operator=(const Chunk &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init the chunk meshes.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw meshes.
	 * @param chunkId The chunk id.
	 */
	void	init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader);
	/**
	 * @brief Init blocks in chunk.
	 *
	 * @param chunkId Id of the chunk.
	 */
	void	generate(const gm::Vec2i &chunkId);
	/**
	 * @brief Create chunk meshes.
	 *
	 * @param map Map containing other chunks.
	 */
	void	createMeshes(Map &map);
	/**
	 * @brief Create chunk meshes buffers.
	 *
	 * @param commandPool Command class for vulkan.
	 * @param stagingBuffer Common tempory buffer for avoid buffer recreation.
 	 * @param commandBuffer The command buffer used for group copy commands.
	 */
	void	createBuffers(
				VulkanCommandPool &commandPool,
				StagingBuffer &stagingBuffer,
				VkCommandBuffer &commandBuffer);
	/**
	 * @brief Update mesh according to chunk blocks.
	 *
	 * @param engine Engine struct.
	 * @param map Map containing other chunks.
	 */
	void	updateMesh(Engine &engine, Map &map);
	/**
	 * @brief Draw chunk meshes.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw meshes.
	 */
	void	draw(Engine &engine, Camera &camera, ChunkShader &chunkShader);
	/**
	 * @brief Draw chunk liquid mesh.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw meshes.
	 */
	void	drawLiquid(Engine &engine, Camera &camera, ChunkShader &chunkShader);
	/**
	 * @brief Destroy chunk.
	 *
	 * @param engine Engine struct.
	 */
	void	destroy(Engine &engine);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Vec2i		chunkId;
	gm::Vec3f		chunkPosition;
	Cube			cubes[CHUNK_TOTAL_SIZE]; // id = x + z * SIZE + y * SIZE2
	CubeBitmap		cubeBitmap;
	ChunkMesh		mesh, liquidMesh;
	ChunkBorderMesh	borderMesh;
	UBO3DChunkPos	uboPos;
	ShaderParam		shaderParam, shaderParamLiquid, shaderParamFdf,
					shaderParamFdfLiquid, shaderParamBorder;
	BoundingCube	boundingCube;
	bool			initDone, generationDone, meshCreate, bufferCreate, canBeDraw;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create borderMesh.
	 */
	void	createBorderMesh(void);
	/**
	 * @brief Create mesh.
	 *
	 * @param map Map that contain chunks.
	 */
	void	createMesh(Map &map);
	/**
	 * @brief Create liquid mesh.
	 */
	void	createLiquidMesh(void);
};

//**** FUNCTIONS ***************************************************************

#endif
