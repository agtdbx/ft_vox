#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <program/map/Chunk.hpp>

class	Map;

/**
 * @brief Cluster class.
 */
class Cluster
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Cluster class.
	 *
	 * @return The default Cluster.
	 */
	Cluster(void);
	/**
	 * @brief Copy constructor of Cluster class.
	 *
	 * @param obj The Cluster to copy.
	 *
	 * @return The Cluster copied from parameter.
	 */
	Cluster(const Cluster &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Cluster class.
	 */
	~Cluster();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of bounding cube.
	 *
	 * @return The const reference to bounding cube.
	 */
	const BoundingCube	&getBoundingCube(void) const;
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Cluster class.
	 *
	 * @param obj The Cluster to copy.
	 *
	 * @return The Cluster copied from parameter.
	 */
	Cluster	&operator=(const Cluster &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Set min and max chunks of cluster.
	 *
	 * @param middle Middle position of the cluster.
	 */
	void	setPosition(const gm::Vec2i &middle);
	/**
	 * @brief Move chunks inside.
	 *
	 * @param map Map that contains chunks.
	 * @param movement Vector for movement.
	 */
	void	move(Map &map, const gm::Vec2i &movement);
	/**
	 * @brief Give a chunk completed to the cluster.
	 *
	 * @param chunkPos The position of the chunk.
	 * @param chunk The chunk to add.
	 *
	 * @return True if the chunk was put in cluster, false else.
	 */
	bool	giveChunk(const gm::Vec2i &chunkPos, Chunk *chunk);
	/**
	 * @brief Draw chunks.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw chunk meshes.
	 * @param nbDrawCall Counter for draw call.
	 */
	void	draw(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader);
	/**
	 * @brief Draw water on chunks.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param chunkShader Shaders used to draw chunk meshes.
	 */
	void	drawLiquid(
				Engine &engine,
				Camera &camera,
				ChunkShader &chunkShader);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Vec2i	minChunk, maxChunk;
	Chunk		*chunks[CLUSTER_SIZE2];
	BoundingCube	boundingCube;

//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
