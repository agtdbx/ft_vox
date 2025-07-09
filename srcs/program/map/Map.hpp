#ifndef MAP_HPP
# define MAP_HPP

# include <program/map/Chunk.hpp>
# include <program/map/Cluster.hpp>
# include <program/map/PerlinNoise.hpp>

# include <unordered_map>
# include <thread>
# include <mutex>

using ChunkMap = std::unordered_map<std::size_t, Chunk>;


enum ThreadStatus
{
	THREAD_RUNNING,			// Waiting status
	THREAD_NEED_GENERATE,	// Ask generation
	THREAD_GENERATING,		// Generation in progress
	THREAD_NEED_MESH,		// Ask meshing
	THREAD_MESHING,			// Meshing in progress
	THREAD_MESH_END,		// Meshing asked finish
	THREAD_NEED_DESTROY,	// Ask destroying
	THREAD_DESTROYING,		// Destroying in progress
	THREAD_STOPPING,		// Ask stopping thread
	THREAD_STOP,			// Thread stopped
};


struct ThreadData
{
	std::mutex		mutex; // Mutex for the struct
	ThreadStatus	status;
	gm::Vec2i		minChunkId; // Min id of chunk to generate or mesh
	gm::Vec2i		maxChunkId; // Max id of chunk to generate or mesh
	gm::Vec2i		cameraChunkId; // Chunk id for the camera
	PerfLogger		perfLogger;
	int				threadId;
	ChunkMap		*chunks; // Chunks container
	std::mutex		*chunksMutex;
	Map				*map;
	Engine			*engine;
	Camera			*camera;
	ChunkShader		*chunkShader;

};


enum	MapStatus
{
	MAP_NONE,
	MAP_GENERATING_X,
	MAP_MESHING_X,
	MAP_DESTROYING_X,
	MAP_GENERATING_Y,
	MAP_MESHING_Y,
	MAP_DESTROYING_Y,
};


struct	MapView
{
	gm::Vec2i	minGenChunk;
	gm::Vec2i	maxGenChunk;
	gm::Vec2i	minMeshChunk;
	gm::Vec2i	maxMeshChunk;
	gm::Vec2i	tmpId;

	bool	operator==(const MapView &obj)
	{
		return (this->minGenChunk == obj.minGenChunk
				&& this->maxGenChunk == obj.maxGenChunk
				&& this->minMeshChunk == obj.minMeshChunk
				&& this->maxMeshChunk == obj.maxMeshChunk
				&& this->tmpId == obj.tmpId);
	}
};

/**
 * @brief Map class.
 */
class Map
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Map class.
	 *
	 * @return The default Map.
	 */
	Map(void);
	/**
	 * @brief Copy constructor of Map class.
	 *
	 * @param obj The Map to copy.
	 *
	 * @return The Map copied from parameter.
	 */
	Map(const Map &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Map class.
	 */
	~Map();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Get chunk by id.
	 *
	 * @param x Id x.
	 * @param y Id y.
	 *
	 * @return Pointer on chunk at this id, or NULL if the chunk doesn't exist.
	 */
	Chunk	*getChunk(int x, int y);
	/**
	 * @brief Get chunk cube bitmap by id.
	 *
	 * @param x Id x.
	 * @param y Id y.
	 *
	 * @return Pointer on chunk cube bitmap at this id, or NULL if the chunk doesn't exist.
	 */
	CubeBitmap	*getChunkBitmap(int x, int y);
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Map class.
	 *
	 * @param obj The Map to copy.
	 *
	 * @return The Map copied from parameter.
	 */
	Map	&operator=(const Map &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init chunks.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw chunk meshes.
	 */
	void	init(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader);
	/**
	 * @brief Update map according to camera pos.
	 *
	 * @param camera The camera.
	 */
	void	update(Engine &engine, Camera &camera);
	/**
	 * @brief Draw chunks.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw chunk meshes.
	 */
	void	draw(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader);
	/**
	 * @brief Destroy chunks.
	 */
	void	destroy(Engine &engine);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	ChunkMap				chunks;
	std::mutex				chunksMutex;
	std::vector<Cluster>	clusters;
	std::vector<gm::Vec2i>	clusterOffsets;
	gm::Vec2i				minChunkIdOffset, maxChunkIdOffset,
							minDelete, maxDelete, cameraChunkId;
	MapView					currentView, targetView;
	ThreadData				*threadsData;
	std::thread				*threads;
	StagingBuffer			stagingBuffer;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Method to prepare generation.
	 *
	 * @param camera The camera.
	 *
	 * @return The new status for map generation. Can be none if no generation is needed.
	 */
	MapStatus	prepareGeneration(Engine &engine, Camera &camera);
	/**
	 * @brief Method to order generation of chunk block to threads optimize of X axis.
	 *
	 * @return True is the generation is finish, false else.
	 */
	bool		generatingX(void);
	/**
	 * @brief Method to order generation of chunk block to threads optimize of Y axis.
	 *
	 * @return True is the generation is finish, false else.
	 */
	bool		generatingY(void);
	/**
	 * @brief Method to order meshing of chunk block to threads optimize of X axis.
	 *
	 * @return True is the meshing is finish, false else.
	 */
	bool		meshingX(void);
	/**
	 * @brief Method to order meshing of chunk block to threads optimize of Y axis.
	 *
	 * @return True is the meshing is finish, false else.
	 */
	bool		meshingY(void);
	/**
	 * @brief Method to order destroying of chunk block to threads optimize of X axis.
	 *
	 * @return True is the destroying is finish, false else.
	 */
	bool		destroyingX(void);
	/**
	 * @brief Method to order destroying of chunk block to threads optimize of Y axis.
	 *
	 * @return True is the destroying is finish, false else.
	 */
	bool		destroyingY(void);
	/**
	 * @brief Method to order destroying of chunk block to threads.
	 *
	 * @return True is the destroying is finish, false else.
	 */
	bool		destroyingChunks(void);
};

//**** FUNCTIONS ***************************************************************

#endif
