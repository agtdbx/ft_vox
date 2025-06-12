#ifndef CHUNK_HPP
# define CHUNK_HPP

# include <engine/engine.hpp>
# include <engine/mesh/Mesh.hpp>
# include <engine/mesh/VertexPos.hpp>
# include <engine/mesh/VertexPosNrm.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/camera/Camera.hpp>
# include <program/map/Cube.hpp>
# include <program/shaderStruct.hpp>
# include <program/map/Perlin.hpp>

#include <chrono>

using ChunkMesh = Mesh<VertexPosNrm>;
using ChunkBorderMesh = Mesh<VertexPos>;

class Map;
#define SEED 42

struct ChunkShader
{
	Shader	shader;
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
		this->shaderFdf.destroy(engine);
		this->shaderBorder.destroy(engine);
	}
};


struct Face
{
	int	x;
	int	y;
	int	w;
	int	h;
	int	axis;
};


// TODO : REMOVE
struct PerfField
{
	std::clock_t	start;
	int				total;
	int				nbCall;
};

void	startLog(PerfField &perfField);
void	endLog(PerfField &perfField);
void	resetLog(PerfField &perfField);
void	printLog(PerfField &perfField, const char *msg);

struct PerfLogger
{
	PerfField	generation;
	PerfField	createMesh;
	int			nbTriangles;
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
	void	generate(const gm::Vec2i &chunkId, PerfLogger &perfLogger);
	/**
	 * @brief Update chunk meshs.
	 *
	 * @param map Map containing other chunks.
	 */
	void	updateMeshes(Map &map, PerfLogger &perfLogger);
	/**
	 * @brief Draw chunk meshes.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw meshes.
	 */
	void	draw(Engine &engine, Camera &camera, ChunkShader &chunkShader);
	/**
	 * @brief Destroy chunk.
	 */
	void	destroy(Engine &engine);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//---- Chunk properties --------------------------------------------------------
	gm::Vec2i		chunkId;
	gm::Vec3f		chunkPosition;
	std::vector<gm::Vec3f>	positions;
	Cube			cubes[CHUNK_TOTAL_SIZE]; // id = x + z * SIZE + y * SIZE2
	int32_t			cubesMaskLeft[CHUNK_MASK_SIZE];  // id = z + y * SIZE, 1 << x
	int32_t			cubesMaskRight[CHUNK_MASK_SIZE]; // id = z + y * SIZE, 1 << 31 - x
	int32_t			cubesMaskFront[CHUNK_MASK_SIZE]; // id = x + y * SIZE, 1 << z
	int32_t			cubesMaskBack[CHUNK_MASK_SIZE];  // id = x + y * SIZE, 1 << 31 - z
	ChunkMesh		mesh;
	ChunkBorderMesh	borderMesh;
	UBO3DChunkPos	uboPos;
	UBO3DChunkCubes	uboCubes;
	ShaderParam		shaderParam, shaderParamFdf, shaderParamBorder;
//---- Copy --------------------------------------------------------------------
	VulkanCommandPool	*copyCommandPool;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create borderMesh.
	 */
	void	createBorderMesh(void);
	/**
	 * @brief Create meshes.
	 *
	 * @param map Map that contain chunks.
	 */
	void	createMesh(Map &map);
};

//**** FUNCTIONS ***************************************************************

#endif
