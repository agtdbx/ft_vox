#ifndef CAMERA_HPP
# define CAMERA_HPP

# include <define.hpp>
# include <engine/camera/BoundingCube.hpp>
# include <engine/mesh/VertexPos.hpp> // TODO : Remove
# include <engine/mesh/Mesh.hpp> // TODO : Remove
# include <program/shaderStruct.hpp> // TODO : Remove
# include <engine/shader/ShaderParam.hpp> // TODO : Remove

# include <gmath.hpp>

// TODO: Remove
class Window;
class Shader;

struct FrustumPlane
{
	gm::Vec3f	position;
	gm::Vec3f	normal;
};

struct Frustum
{
	bool	compute = false;
	FrustumPlane	topFace;
	FrustumPlane	botFace;

	FrustumPlane	leftFace;
	FrustumPlane	rightFace;

	FrustumPlane	nearFace;
	FrustumPlane	farFace;
};

/**
 * @brief Camera class.
 */
class Camera
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Camera class.
	 *
	 * @return The default Camera.
	 */
	Camera(void);
	/**
	 * @brief Copy constructor of Camera class.
	 *
	 * @param obj The Camera to copy.
	 *
	 * @return The Camera copied from parameter.
	 */
	Camera(const Camera &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Camera class.
	 */
	~Camera();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of projection matrix.
	 *
	 * @return Projection matrix as Mat4f.
	 */
	const gm::Mat4f	&getProjection(void) const;
	/**
	 * @brief Getter of view matrix.
	 *
	 * @return Viw matrix as Mat4f.
	 */
	const gm::Mat4f	&getView(void) const;
	/**
	 * @brief Getter of position.
	 *
	 * @return Position as Vec3f.
	 */
	const gm::Vec3f	&getPosition(void) const;
	/**
	 * @brief Getter of front direction.
	 *
	 * @return Front direction as Vec3f.
	 */
	const gm::Vec3f	&getFront(void) const;
	/**
	 * @brief Getter of up direction.
	 *
	 * @return Up direction as Vec3f.
	 */
	const gm::Vec3f	&getUp(void) const;
	/**
	 * @brief Getter of right direction.
	 *
	 * @return Right direction as Vec3f.
	 */
	const gm::Vec3f	&getRight(void) const;
	/**
	 * @brief Getter of pitch.
	 *
	 * @return Pitch as float.
	 */
	float	getPitch(void) const;
	/**
	 * @brief Getter of yaw.
	 *
	 * @return Yaw as float.
	 */
	float	getYaw(void) const;
	/**
	 * @brief Getter of roll.
	 *
	 * @return Roll as float.
	 */
	float	getRoll(void) const;

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Setter for position.
	 *
	 * @param position New position.
	 */
	void	setPosition(const gm::Vec3f &position);
	/**
	 * @brief Setter for rotation.
	 *
	 * @param pitch Pitch of rotation.
	 * @param yaw Yaw of rotation.
	 * @param roll Roll of rotation.
	 */
	void	setRotation(const float pitch, const float yaw, const float roll);

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Camera class.
	 *
	 * @param obj The Camera to copy.
	 *
	 * @return The Camera copied from parameter.
	 */
	Camera	&operator=(const Camera &obj);

//**** PUBLIC METHODS **********************************************************
//---- move --------------------------------------------------------------------
	/**
	 * @brief Move camera with a vector.
	 *
	 * @param direction A vector to tell which direction to go. x for right, y for up, z for front.
	 * @param speed The distance to move.
	 */
	void	move(const gm::Vec3f &direction, const float speed);
	/**
	 * @brief Move camera in front direction.
	 *
	 * @param speed The distance to move.
	 */
	void	moveFront(const float speed);
	/**
	 * @brief Move camera in up direction.
	 *
	 * @param speed The distance to move.
	 */
	void	moveUp(const float speed);
	/**
	 * @brief Move camera in right direction.
	 *
	 * @param speed The distance to move.
	 */
	void	moveRight(const float speed);

//---- rotate ------------------------------------------------------------------
	/**
	 * @brief Rotate camera on x axis.
	 *
	 * @param degrees Angle of rotation in degrees.
	 */
	void	rotateX(const float degrees);
	/**
	 * @brief Rotate camera on y axis.
	 *
	 * @param degrees Angle of rotation in degrees.
	 */
	void	rotateY(const float degrees);
	/**
	 * @brief Rotate camera on z axis.
	 *
	 * @param degrees Angle of rotation in degrees.
	 */
	void	rotateZ(const float degrees);

//---- update ------------------------------------------------------------------
	/**
	 * @brief Update camera when window size change.
	 *
	 * @param windowWidth New width of window.
	 * @param windowHeight New height of window.
	 */
	void	updateFromWindow(const int windowWidth, const int windowHeight);
	/**
	 * @brief Update camera when fov change.
	 *
	 * @param fov New fov.
	 */
	void	updateFOV(const float fov);

//---- checks ------------------------------------------------------------------

	bool	isCubeInFrutum(const BoundingCube &cube);

//---- status ------------------------------------------------------------------
	/**
	 * @brief Print camera position and rotation on standard output.
	 */
	void	printStatus(void);
	void	computeFrustum(Engine &engine, Shader &shader);
	void	draw(Window &window, Shader &shader);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Mat4f	view, projection;
	gm::Vec3f	position, front, up, right;
	float		pitch, yaw, roll, planeWidth, planeHeight, winRatio, fov;
	Frustum		frustum;
	Mesh<VertexPos>	frustumMesh;
	ShaderParam		shaderParam;
	UBO3DChunkPos	uboPos;

//**** PRIVATE METHODS *********************************************************
	void	computeRotation(void);
	void	computeView(void);
	// void	computeFrustum(void);
};

//**** FUNCTIONS ***************************************************************

#endif
