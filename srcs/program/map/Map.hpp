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
	THREAD_RUNNING,
	THREAD_NEED_GENERATE,
	THREAD_GENERATING,
	THREAD_GENERATE_END,
	THREAD_NEED_MESH,
	THREAD_MESHING,
	THREAD_MESH_END,
	THREAD_STOPPING,
	THREAD_STOP,
};


struct ThreadData
{
	std::mutex		mutex; // Mutex for the struct
	ThreadStatus	status;
	gm::Vec2i		minChunkId; // Min id of chunk to generate or mesh
	gm::Vec2i		maxChunkId; // Max id of chunk to generate or mesh
	gm::Vec2i		cameraChunkId; // Chunk id for the camera
	ChunkMap		*chunks; // Chunks container
	Map				*map;
	Engine			*engine;
	Camera			*camera;
	ChunkShader		*chunkShader;
};


enum	MapStatus
{
	MAP_NONE,
	MAP_GENERATING,
	MAP_MESHING,
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
	std::vector<Cluster>	clusters;
	std::vector<gm::Vec2i>	clusterOffsets;
	gm::Vec2i				minChunkIdOffset, maxChunkIdOffset, cameraChunkId;
	MapView					currentView, targetView;
	int						nbThread;
	ThreadData				*threadsData;
	std::thread				*threads;

//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
