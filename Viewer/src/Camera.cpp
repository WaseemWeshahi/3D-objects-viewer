#include "Camera.h"

void Camera::init()
{
	cTransform = projection = glm::mat4(1.0);
	up = glm::vec3(0, 1, 0);
	eyeC = glm::vec3(1, 1, 1);
	 att = glm::vec3(0.0);
}


Camera::Camera()
{
	init();
}


Camera::~Camera()
{
}
void Camera::SetTransformation(const glm::mat4x4& transform)
{
	cTransform = transform;
}

glm::vec4 Camera::cross(const glm::vec4& v1, const glm::vec4& v2)
{
	glm::vec3 firstV(v1.x, v1.y, v1.z), secondV(v2.x, v2.y, v2.z);
	glm::vec4 result(glm::cross(firstV, secondV), 0);
	return result;
}

glm::mat4 Camera::Translate(glm::vec4 v1)
{

	glm::mat4 result(1.0);
	result[0][3] = v1.x;
	result[1][3] = v1.y;
	result[2][3] = v1.z;
	return glm::transpose(result);
}
void Camera::focus(const glm::vec3 orig) {
	att = orig;
//	LookAt(glm::vec4(eyeC, 1.0), glm::vec4(orig, 1.0), glm::vec4(up, 1.0));
}
void Camera::LookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	glm::vec4 n = normalize(eye - at);
	glm::vec4 u = normalize(cross(up, n));
	glm::vec4 v = normalize(cross(n, u));
	glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::mat4x4 c = glm::mat4x4(u, v, n, t);
	c = glm::transpose(c);
	cTransform = c*Translate(-eye);
	/****Watch out****/
//	cTransform = glm::transpose(cTransform);
}

void Camera::Ortho(const float left, const float right, const float bottom,
	const float top, const float zNear, const float zFar)
{
	projection = glm::mat4(0.0);
	projection[0][0] = 2 / (right - left);
	projection[0][3] = -(right + left) / (right - left);
	projection[1][1] = 2 / (top - bottom);
	projection[2][2] = 2 / (zNear - zFar);
	projection[3][3] = 1;
	projection[1][3] = -(top + bottom) / (top - bottom);
	projection[2][3] = -(zFar + zNear) / (zNear - zFar);

	/*****Watch Out****/
	projection = glm::transpose(projection);
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top,
	const float zNear, const float zFar)
{
	projection = glm::mat4(0.0);

	projection[0][0] = 2 * zNear / (right - left);
	projection[0][2] = (right + left) / (right - left);
	projection[1][1] = 2 * zNear / (top - bottom);
	projection[1][2] = (top + bottom) / (top - bottom);
	projection[2][3] = -2 * zFar*zNear / (zFar - zNear);
	projection[2][2] = -(zFar + zNear) / (-zNear + zFar);
	projection[3][2] = -1;
	projection = glm::transpose(projection);
}

glm::mat4x4 Camera::Perspective(const float fovy, const float aspect,
	const float zNear, const float zFar)
{
	float height = zNear*tan(fovy/2);
	float width = aspect*height;
	this->Frustum(-width , width , -height, height, zNear, zFar); //as explained in http://www.songho.ca/opengl/gl_transform.html
	return projection;

}
const glm::mat4x4 Camera::get_projection()
{
	return projection;
}
const glm::mat4x4 Camera::get_transformation()
{
	return cTransform;
}
