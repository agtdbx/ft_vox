#ifndef MAP_HPP
# define MAP_HPP

# include <program/map/Chunk.hpp>

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
	 * @param commandPool The engine command pool.
	 * @param camera The camera.
	 */
	void	init(VulkanCommandPool &commandPool, Camera &camera);
	/**
	 * @brief Draw chunks.
	 *
	 * @param engine Engine struct.
	 * @param chunkShaders Shader used to draw chunk meshes.
	 * @param camera The camera.
	 */
	void	draw(Engine &engine, Camera &camera, Shader *chunkShaders);
	/**
	 * @brief Destroy chunks.
	 */
	void	destroy(void);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	Chunk	chunk;
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
