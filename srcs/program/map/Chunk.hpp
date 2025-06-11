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

using ChunkMesh = Mesh<VertexPosNrm>;
using ChunkBorderMesh = Mesh<VertexPos>;

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
	Cube	getCube(unsigned int x, unsigned int y, unsigned int z);
	/**
	 * @brief Getter of cube in a chunk.
	 *
	 * @param xpos Coordonate of the cube in the chunk.
	 *
	 * @return The cube at the coordonate, or air in case of invalid coordonates.
	 */
	Cube	getCube(const gm::Vec3u &pos);

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Setter of cube in a chunk. Does nothing in case of invalid coordonates.
	 *
	 * @param x X coordonate of the cube in the chunk.
	 * @param y Y coordonate of the cube in the chunk.
	 * @param z Z coordonate of the cube in the chunk.
	 * @param cube The new cube to set.
	 */
	void	setCube(unsigned int x, unsigned int y, unsigned int z, Cube cube);

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
	 * @param chunkId The chunk  id.
	 */
	void	init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader,
				const gm::Vec3i &chunkId);
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
	bool			empty;
	gm::Vec3i		chunkId;
	gm::Vec3f		chunkPosition;
	std::vector<gm::Vec3f>	positions;
	Cube			cubes[CHUNK_SIZE3]; // id = x + y * SIZE + z * SIZE2
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
	 */
	void	createMesh(void);
	/**
	 * @brief Create face up mesh.
	 */
	void	createFace(
				std::unordered_map<std::size_t, uint32_t> &vertexIndex,
				std::vector<VertexPosNrm> &vertices,
				std::vector<uint32_t> &indices,
				int &nbVertex,
				const gm::Vec3u &posCheck,
				const gm::Vec3f &posLU,
				const gm::Vec3f &posLD,
				const gm::Vec3f &posRD,
				const gm::Vec3f &posRU,
				const gm::Vec3f &normal);
	/**
	 * @brief Init blocks in chunk.
	 */
	void	initBlocks(void);
};

//**** FUNCTIONS ***************************************************************

#endif
