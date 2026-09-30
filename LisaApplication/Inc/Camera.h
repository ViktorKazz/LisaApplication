//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple first person style camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be constructed.  
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

#include "pch.h"

class Camera
{
public:

	Camera();
	~Camera();

	// Get/Set world camera position.
	DirectX::XMVECTOR GetPivot()const;
	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);
	
	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight()const;
	DirectX::XMFLOAT3 GetRight3f()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMFLOAT3 GetUp3f()const;
	DirectX::XMVECTOR GetLook()const;
	DirectX::XMFLOAT3 GetLook3f()const;

	// Move to Viewport class!!!
	DirectX::XMVECTOR GetPerspectiveDefaultPos() const noexcept { return XMLoadFloat3(&m_perDefautPos); };

	//// Find the distance between the initial position of the perspective camera and the zero vector.
	//// It is convenient to do these calculations during the initialization of the class member, 
	//// rather than in the shader itself.
	//// The resulting value will standard for pivot scaling.
	//float GetPerspectiveDefaultLength() const noexcept 
	//{
	//	using namespace DirectX;
	//	XMVECTOR v = XMVector3Length(XMVectorSubtract(XMVectorZero(), XMLoadFloat3(&m_perDefautPos)));
	//	
	//	return XMVectorGetX(v);
	//};

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;
	
	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	float GetCameraTranslateZ(this Camera& object) noexcept { return object.m_cameraTranslateZ; };
	float GetCameraRotateX(this Camera& object) noexcept { return object.m_cameraRotateX; };
	float GetCameraRotateY(this Camera& object) noexcept { return object.m_cameraRotateY; };

	void SetCameraTranslateZ(this Camera& object, float value) noexcept { object.m_cameraTranslateZ = value; };
	void SetCameraRotateX(this Camera& object, float value) noexcept { object.m_cameraRotateX = value; };
	void SetCameraRotateY(this Camera& object, float value) noexcept { object.m_cameraRotateY = value; };

	// Moving the camera pivot.
	void PivotMoveY(float d);
	void PivotMoveX(float d);

	// Strafe/Walk the camera a distance d.
	void Walk(float d);

	// Viewport scaling (zooming the camera in/out).
	void Scaling(this Camera& object, float d);

	// Rotate the camera along X axis.
	void RotateX(float angle);
	// Rotate the camera along Y axis.
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

	// Functions for capturing the initial coordinates when a mouse click occurs.
	void OnLButtonDown(this Camera& object, LPARAM lParam);
	void OnMButtonDown(this Camera& object, LPARAM lParam);
	void OnRButtonDown(this Camera& object, LPARAM lParam);

	// Functions for rotating, moving and scaling the viewport.
	void OnLButtonMove(this Camera& object, LPARAM lParam);
	void OnMButtonMove(this Camera& object, LPARAM lParam);
	void OnRButtonMove(this Camera& object, LPARAM lParam);

	// The function moves the camera to the selected object.
	// If the object is not selected, the camera will move to the origin.
	void BringCloserToObject(this Camera& object, DirectX::XMFLOAT3 pivot);

	bool mViewDirty = true;

private:

	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3 mPivot = { 0.0f, 0.0f, 0.0f };
	
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

	// Initial position of the perspective camera.
	DirectX::XMFLOAT3 m_perDefautPos{ 28.0f, 21.0f, -28.0f };

	// Cache frustum properties.
	float mNearZ{ 0.0f };
	float mFarZ{ 0.0f };
	float mAspect{ 0.0f };
	float mFovY{ 0.0f };
	float mNearWindowHeight{ 0.0f };
	float mFarWindowHeight{ 0.0f };

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	

	float m_cameraTranslateZ{};
	
	float m_cameraRotateX{};
	float m_cameraRotateY{};

	float m_cameraPivotX{};
	float m_cameraPivotY{};

	INT m_cameraScaleStart{};
	INT m_cameraScaleEnd{};

	INT m_cameraMoveHorizontalStart{};
	INT m_cameraMoveHorizontalEnd{};
	INT m_cameraMoveVerticalStart{};
	INT m_cameraMoveVerticalEnd{};

	INT m_cameraRotateHorizontalStart{};
	INT m_cameraRotateHorizontalEnd{};
	INT m_cameraRotateVerticalStart{};
	INT m_cameraRotateVerticalEnd{};
};

#endif // CAMERA_H