#ifndef SKYBOX_HPP
# define SKYBOX_HPP

# include <engine/engine.hpp>
# include <engine/mesh/Mesh.hpp>
# include <engine/mesh/VertexPosTex.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/camera/Camera.hpp>
# include <program/shaderStruct.hpp>

/**
 * @brief Class for a skybox.
 */
class Skybox
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Skybox class.
	 *
	 * @return The default Skybox.
	 */
	Skybox();
//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Skybox class.
	 */
	~Skybox();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init the skybox.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param skyboxShader Shader used to draw skybox.
	 */
	void init(Engine &engine, Camera &camera, Shader &skyboxShader);
	/**
	 * @brief Draw the skybox.
	 *
	 * @param engine Engine struct.
	 * @param camera The camera.
	 * @param skyboxShader Shader used to draw skybox.
	 */
	void draw(Engine &engine, Camera &camera, Shader &skyboxShader);
	/**
	 * @brief Destroy skybox.
	 *
	 * @param engine Engine struct.
	 */
	void destroy(Engine &engine);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	Mesh<VertexPosTex>	mesh;
	ShaderParam			shaderParam;
	UBO3DChunkPos		uboPos;
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
