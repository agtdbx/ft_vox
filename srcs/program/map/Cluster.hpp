#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <program/map/Chunk.hpp>

/**
 * @brief Cluster class.
 */
class Cluster
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
	Chunk	*chunks[CLUSTER_SIZE3];

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
	 * @brief Draw chunks.
	 *
	 * @param engine Engine struct.
	 * @param chunkShader Shader used to draw chunk meshes.
	 * @param camera The camera.
	 */
	void	draw(Engine &engine, Camera &camera, Shader &chunkShader);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
