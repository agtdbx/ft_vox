#include <engine/camera/Camera.hpp>

// TODO : Remove
#include <engine/shader/Shader.hpp>
#include <engine/window/Window.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************

static bool	isAABBForwardPlane(
				const FrustumPlane &plane,
				const BoundingCube &cube);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Camera::Camera(void)
{
	this->winRatio = (float)WIN_W / (float)WIN_H;
	this->fov = FOV;

	this->projection = gm::Mat4f::perspective(gm::radians(this->fov), this->winRatio, NEAR, FAR);

	this->planeHeight = tan(gm::radians(this->fov * 0.5f)) * 2.0f;
	this->planeWidth = this->planeHeight * this->winRatio;

	this->position = gm::Vec3f(0.0f, 0.0f, 0.0f);
	this->pitch = 0.0f;
	this->yaw = 0.0f;
	this->roll = 0.0f;

	this->computeRotation();
	this->computeView();
	this->computeFrustum();
}


Camera::Camera(const Camera &obj)
{
	this->view = obj.view;
	this->projection = obj.projection;
	this->position = obj.position;
	this->front = obj.front;
	this->up = obj.up;
	this->right = obj.right;
	this->pitch = obj.pitch;
	this->yaw = obj.yaw;
	this->roll = obj.roll;
	this->planeWidth = obj.planeWidth;
	this->planeHeight = obj.planeHeight;
	this->winRatio = obj.winRatio;
	this->frustum = obj.frustum;
}

//---- Destructor --------------------------------------------------------------

Camera::~Camera()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

const gm::Mat4f	&Camera::getProjection(void) const
{
	return (this->projection);
}


const gm::Mat4f	&Camera::getView(void) const
{
	return (this->view);
}


const gm::Vec3f	&Camera::getPosition(void) const
{
	return (this->position);
}


const gm::Vec3f	&Camera::getFront(void) const
{
	return (this->front);
}


const gm::Vec3f	&Camera::getUp(void) const
{
	return (this->up);
}


const gm::Vec3f	&Camera::getRight(void) const
{
	return (this->right);
}


float	Camera::getPitch(void) const
{
	return (this->pitch);
}


float	Camera::getYaw(void) const
{
	return (this->yaw);
}


float	Camera::getRoll(void) const
{
	return (this->roll);
}

//---- Setters -----------------------------------------------------------------

void	Camera::setPosition(const gm::Vec3f &position)
{
	this->position = position;
	this->computeView();
	this->computeFrustum();
}


void	Camera::setRotation(const float pitch, const float yaw, const float roll)
{
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	this->computeRotation();
	this->computeView();
	this->computeFrustum();
}

//---- Operators ---------------------------------------------------------------

Camera	&Camera::operator=(const Camera &obj)
{
	if (this == &obj)
		return (*this);

	this->view = obj.view;
	this->projection = obj.projection;
	this->position = obj.position;
	this->front = obj.front;
	this->up = obj.up;
	this->right = obj.right;
	this->pitch = obj.pitch;
	this->yaw = obj.yaw;
	this->roll = obj.roll;
	this->planeWidth = obj.planeWidth;
	this->planeHeight = obj.planeHeight;
	this->winRatio = obj.winRatio;
	this->frustum = obj.frustum;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************
//---- move --------------------------------------------------------------------

void	Camera::move(const gm::Vec3f &direction, const float speed)
{
	this->position += this->right * direction.x * speed
						+ this->up * direction.y * speed
						+ this->front * direction.z * speed;
	this->computeView();
	this->computeFrustum();
}


void	Camera::moveFront(const float speed)
{
	this->position += this->front * speed;
	this->computeView();
	this->computeFrustum();
}


void	Camera::moveUp(const float speed)
{
	this->position += this->up * speed;
	this->computeView();
	this->computeFrustum();
}


void	Camera::moveRight(const float speed)
{
	this->position += this->right * speed;
	this->computeView();
	this->computeFrustum();
}

//---- rotate ------------------------------------------------------------------

void	Camera::rotateX(const float degrees)
{
	this->pitch += degrees;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	this->computeRotation();
	this->computeView();
	this->computeFrustum();
}


void	Camera::rotateY(const float degrees)
{
	this->yaw += degrees;
	this->computeRotation();
	this->computeView();
	this->computeFrustum();
}


void	Camera::rotateZ(const float degrees)
{
	this->roll += degrees;
	this->computeRotation();
	this->computeView();
	this->computeFrustum();
}

//---- update ------------------------------------------------------------------

void	Camera::updateFromWindow(const int windowWidth, const int windowHeight)
{
	this->winRatio = (float)windowWidth / (float)windowHeight;

	this->projection = gm::Mat4f::perspective(gm::radians(this->fov), this->winRatio, NEAR, FAR);

	this->planeHeight = tan(gm::radians(this->fov * 0.5f)) * 2.0f;
	this->planeWidth = this->planeHeight * this->winRatio;
}


void	Camera::updateFOV(const float fov)
{
	this->fov = fov;

	if (this->fov < 0.0f)
		this->fov = 0.0f;
	if (this->fov > 360.0f)
		this->fov = 360.0f;

	this->projection = gm::Mat4f::perspective(gm::radians(this->fov), this->winRatio, NEAR, FAR);

	this->planeHeight = tan(gm::radians(this->fov * 0.5f)) * 2.0f;
	this->planeWidth = this->planeHeight * this->winRatio;
}

//---- status ------------------------------------------------------------------

bool	Camera::isCubeInFrutum(const BoundingCube &cube)
{
	return (isAABBForwardPlane(this->frustum.nearFace, cube)
			&& isAABBForwardPlane(this->frustum.farFace, cube)
			&& isAABBForwardPlane(this->frustum.leftFace, cube)
			&& isAABBForwardPlane(this->frustum.rightFace, cube)
			&& isAABBForwardPlane(this->frustum.botFace, cube)
			&& isAABBForwardPlane(this->frustum.topFace, cube));
}

//---- status ------------------------------------------------------------------

void	Camera::printStatus(void)
{
	std::cout << "Camera status\n  position : " << this->position
				<< "\n  pitch : " << this->pitch
				<< ", yaw : " << this->yaw
				<< ", roll : " << this->roll
				<< "\n  front : " << this->front
				<< "\n  right : " << this->right
				<< "\n  up    : " << this->up << std::endl;
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Camera::computeRotation(void)
{
	this->front.x = cos(gm::radians(this->pitch)) * cos(gm::radians(this->yaw));
	this->front.y = sin(gm::radians(this->pitch));
	this->front.z = cos(gm::radians(this->pitch)) * sin(gm::radians(this->yaw));
	this->front = gm::normalize(this->front);

	this->right = gm::normalize(gm::cross(this->front,gm::Vec3f(0.0f, 1.0f, 0.0f)));
	this->up = gm::normalize(gm::cross(this->right, this->front));
}


void	Camera::computeView(void)
{
	this->view = gm::Mat4f::lookAt(this->position,
									this->position + this->front,
									this->up);
}


void	Camera::computeFrustum(void)
{
	float		heightNear, widthNear,
				dstNearFar, heightFar, widthFar;
	gm::Vec3f	frontNear, rightNear, upNear,
				frontFar, rightFar, upFar;

	// Near
	frontNear = this->front * NEAR;
	heightNear = this->planeHeight / 2.0f;
	widthNear = heightNear * this->winRatio;
	rightNear = this->right * widthNear;
	upNear = this->up * heightNear;

	// Far
	dstNearFar = FAR - NEAR;
	frontFar = frontNear + this->front * dstNearFar;
	heightFar = gm::abs(tanf(FOV / 2.0f) * dstNearFar);
	widthFar = heightFar * this->winRatio;
	rightFar = this->right * widthFar;
	upFar = this->up * heightFar;

	// Points
	const gm::Vec3f pLUN = this->position - rightNear + upNear + frontNear;
	const gm::Vec3f pLDN = this->position - rightNear - upNear + frontNear;
	const gm::Vec3f pRDN = this->position + rightNear - upNear + frontNear;
	const gm::Vec3f pRUN = this->position + rightNear + upNear + frontNear;
	const gm::Vec3f pLUF = this->position - rightFar  + upFar  + frontFar;
	const gm::Vec3f pLDF = this->position - rightFar  - upFar  + frontFar;
	const gm::Vec3f pRDF = this->position + rightFar  - upFar  + frontFar;
	const gm::Vec3f pRUF = this->position + rightFar  + upFar  + frontFar;

	gm::Vec3f	tmp1, tmp2;

	// Compute frustum plane near
	this->frustum.nearFace.position = this->position + frontNear;
	this->frustum.nearFace.normal = this->front;

	// Compute frustum plane far
	this->frustum.farFace.position = this->position + frontFar;
	this->frustum.farFace.normal = -this->front;

	// Compute frustum plane left
	tmp1 = gm::lerp(pLUN, pLUF, 0.5f);
	tmp2 = gm::lerp(pLDN, pLDF, 0.5f);
	this->frustum.leftFace.position = gm::lerp(tmp1, tmp2, 0.5f);
	// pLUF pLDF pLUN
	tmp1 = pLUN - pLUF;
	tmp2 = pLDF - pLUF;
	this->frustum.leftFace.normal = gm::normalize(gm::cross(tmp1, tmp2));

	// Compute frustum plane right
	tmp1 = gm::lerp(pRUN, pRUF, 0.5f);
	tmp2 = gm::lerp(pRDN, pRDF, 0.5f);
	this->frustum.rightFace.position = gm::lerp(tmp1, tmp2, 0.5f);
	// pRUN pRDN pLUF
	tmp1 = pLUF - pRUN;
	tmp2 = pRDN - pRUN;
	this->frustum.rightFace.normal = gm::normalize(gm::cross(tmp1, tmp2));

	// Compute frustum plane bottom
	tmp1 = gm::lerp(pLDN, pLDF, 0.5f);
	tmp2 = gm::lerp(pRDN, pRDF, 0.5f);
	this->frustum.botFace.position = gm::lerp(tmp1, tmp2, 0.5f);
	// pLDN pLDF pRDF
	tmp1 = pRDF - pLDN;
	tmp2 = pLDF - pLDN;
	this->frustum.botFace.normal = gm::normalize(gm::cross(tmp1, tmp2));

	// Compute frustum plane top
	tmp1 = gm::lerp(pLUN, pLUF, 0.5f);
	tmp2 = gm::lerp(pRUN, pRUF, 0.5f);
	this->frustum.topFace.position = gm::lerp(tmp1, tmp2, 0.5f);
	// pLUF pLUN pRUN
	tmp1 = pRUN - pLUF;
	tmp2 = pLUN - pLUF;
	this->frustum.topFace.normal = gm::normalize(gm::cross(tmp1, tmp2));
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static bool	isAABBForwardPlane(
				const FrustumPlane &plane,
				const BoundingCube &cube)
{
	for (int i = 0; i < 8; i++)
	{
		gm::Vec3f	dir = cube.points[i] - plane.position;

		if (gm::dot(dir, plane.normal) > 0.0f)
			return (true);
	}

	return (false);
}
