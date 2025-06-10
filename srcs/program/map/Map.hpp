#ifndef MAP_HPP
# define MAP_HPP

# include <program/map/Chunk.hpp>
# include <program/map/Cluster.hpp>

# define NB_CHUNK 9 // TODO : REMOVE

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
	Chunk	chunks[NB_CHUNK];
	Cluster	cluster;
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
