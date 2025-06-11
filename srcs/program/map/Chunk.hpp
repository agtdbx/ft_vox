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


// struct Face
// {
// 	gm::Vec3f	pos;
// 	gm::Vec3f	size;
// };


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
	Cube	getCube(int x, int y, int z);
	/**
	 * @brief Getter of cube in a chunk.
	 *
	 * @param xpos Coordonate of the cube in the chunk.
	 *
	 * @return The cube at the coordonate, or air in case of invalid coordonates.
	 */
	Cube	getCube(const gm::Vec3i &pos);

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
	int32_t			cubesMask[CHUNK_MASK_SIZE];
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
	/**
	 * @brief Create face up mesh.
	 *
	 * @param vertexIndex Hash map of vertex for avoid vertex duplication.
	 * @param vertices Vector of vertex.
	 * @param indices Vector of index.
	 * @param nbVertex Number of vertex.
	 * @param posCheck Position of check if there is air here.
	 * @param leftChunk Chunk at the left.
	 * @param rightChunk Chunk at the right.
	 * @param frontChunk Chunk at the front.
	 * @param backChunk Chunk at the back.
	 * @param posLU Position of top left vertex of face.
	 * @param posLD Position of bottom left vertex of face.
	 * @param posRD Position of bottom right vertex of face.
	 * @param posRU Position of top right vertex of face.
	 * @param normal Normal of face.
	 */
	void	createFace(
				std::unordered_map<std::size_t, uint32_t> &vertexIndex,
				std::vector<VertexPosNrm> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				gm::Vec3i posCheck,
				Chunk *leftChunk,
				Chunk *rightChunk,
				Chunk *frontChunk,
				Chunk *backChunk,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal);
};

//**** FUNCTIONS ***************************************************************

#endif
