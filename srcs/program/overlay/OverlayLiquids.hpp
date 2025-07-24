#ifndef OVERLAY_LIQUIDS_HPP
# define OVERLAY_LIQUIDS_HPP

# include <engine/engine.hpp>
# include <engine/mesh/Mesh.hpp>
# include <engine/mesh/VertexPos.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/camera/Camera.hpp>
# include <program/shaderStruct.hpp>

/**
 * @brief OverlayLiquids class.
 */
class OverlayLiquids
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of OverlayLiquids class.
	 *
	 * @return The default OverlayLiquids.
	 */
	OverlayLiquids(void);
	/**
	 * @brief Copy constructor of OverlayLiquids class.
	 *
	 * @param obj The OverlayLiquids to copy.
	 *
	 * @return The OverlayLiquids copied from parameter.
	 */
	OverlayLiquids(const OverlayLiquids &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of OverlayLiquids class.
	 */
	~OverlayLiquids();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of OverlayLiquids class.
	 *
	 * @param obj The OverlayLiquids to copy.
	 *
	 * @return The OverlayLiquids copied from parameter.
	 */
	OverlayLiquids	&operator=(const OverlayLiquids &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init the overlay.
	 *
	 * @param engine Engine struct.
	 * @param overlayWaterShader Shader used to draw the overlay of the water.
	 * @param overlayLavaShader Shader used to draw the overlay of the lava.
	 */
	void init(Engine &engine, Shader &overlayWaterShader, Shader &overlayLavaShader);
	/**
	 * @brief Draw the overlay of the water.
	 *
	 * @param engine Engine struct.
	 * @param overlayWaterShader Shader used to draw the overlay of the water.
	 */
	void drawWater(Engine &engine, Shader &overlayWaterShader);
	/**
	 * @brief Draw the overlay of the lava.
	 *
	 * @param engine Engine struct.
	 * @param overlayLavaShader Shader used to draw the overlay of the lava.
	 */
	void drawLava(Engine &engine, Shader &overlayLavaShader);
	/**
	 * @brief Destroy skybox.
	 *
	 * @param engine Engine struct.
	 */
	void destroy(Engine &engine);
//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	Mesh<VertexPos>		mesh;
	ShaderParam			shaderParamWater;
	ShaderParam			shaderParamLava;
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
