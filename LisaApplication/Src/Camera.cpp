//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPivot()const
{
	return XMLoadFloat3(&mPivot);
}

XMVECTOR Camera::GetPosition()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition3f()const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	mViewDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
	mViewDirty = true;
}

XMVECTOR Camera::GetRight()const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight3f()const
{
	return mRight;
}

XMVECTOR Camera::GetUp()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp3f()const
{
	return mUp;
}

XMVECTOR Camera::GetLook()const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook3f()const
{
	return mLook;
}

float Camera::GetNearZ()const
{
	return mNearZ;
}

float Camera::GetFarZ()const
{
	return mFarZ;
}

float Camera::GetAspect()const
{
	return mAspect;
}

float Camera::GetFovY()const
{
	return mFovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mViewDirty = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mViewDirty = true;
}

XMMATRIX Camera::GetView()const
{
	assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProj()const
{
	return XMLoadFloat4x4(&mProj);
}


XMFLOAT4X4 Camera::GetView4x4f()const
{
	assert(!mViewDirty);
	return mView;
}

XMFLOAT4X4 Camera::GetProj4x4f()const
{
	return mProj;
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void Camera::PivotMoveY(float d)
{
	// Move the camera pivot.
	
	// mPivot += d*mUp
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR u = XMLoadFloat3(&mUp);
	XMVECTOR p = XMLoadFloat3(&mPivot);
	XMStoreFloat3(&mPivot, XMVectorMultiplyAdd(s, u, p));

	// Rotate the Z axis at the origin of coordinates at the given angles.

	DirectX::XMMATRIX rotationMatrixGeo =
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_cameraRotateX)) *
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_cameraRotateY)) *
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));

	DirectX::XMFLOAT4X4 m;

	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(0.0f, 0.0f, m_cameraTranslateZ) * rotationMatrixGeo);

	// Using the found coordinates and the pivot coordinates, we move the camera.

	SetPosition(mPivot.x + m._41, mPivot.y + m._42, mPivot.z + m._43);

	mViewDirty = true;
}

void Camera::PivotMoveX(float d)
{
	// Move the camera pivot.
	
	// mPivot += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPivot);
	XMStoreFloat3(&mPivot, XMVectorMultiplyAdd(s, r, p));

	// Rotate the Z axis at the origin of coordinates at the given angles.

	DirectX::XMMATRIX rotationMatrixGeo =
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_cameraRotateX)) *
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_cameraRotateY)) *
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));

	DirectX::XMFLOAT4X4 m;

	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(0.0f, 0.0f, m_cameraTranslateZ) * rotationMatrixGeo);

	// Using the found coordinates and the pivot coordinates, we move the camera.

	SetPosition(mPivot.x + m._41, mPivot.y + m._42, mPivot.z + m._43);

	mViewDirty = true;
}

void Camera::Scaling(this Camera& object, float d)
{
	object.m_cameraTranslateZ += d;

	// Rotate the Z axis at the origin of coordinates at the given angles.

	DirectX::XMMATRIX rotationMatrixGeo =
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(object.m_cameraRotateX)) *
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(object.m_cameraRotateY)) *
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));

	DirectX::XMFLOAT4X4 m;

	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(0.0f, 0.0f, object.m_cameraTranslateZ) * rotationMatrixGeo);

	// Move the camera.

	object.mPosition = XMFLOAT3({ object.mPivot.x + m._41, object.mPivot.y + m._42, object.mPivot.z + m._43 });
	object.mViewDirty = true;
}

void Camera::RotateX(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if(mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = false;
	}
}

void Camera::OnLButtonDown(this Camera& object, LPARAM lParam)
{
	object.m_cameraRotateHorizontalStart = GET_X_LPARAM(lParam);
	object.m_cameraRotateVerticalStart = GET_Y_LPARAM(lParam);
}

void Camera::OnMButtonDown(this Camera& object, LPARAM lParam)
{
	object.m_cameraMoveHorizontalStart = GET_X_LPARAM(lParam);
	object.m_cameraMoveVerticalStart = GET_Y_LPARAM(lParam);
}

void Camera::OnRButtonDown(this Camera& object, LPARAM lParam)
{
	object.m_cameraScaleStart = GET_Y_LPARAM(lParam);
}

void Camera::OnLButtonMove(this Camera& object, LPARAM lParam)
{
	object.m_cameraRotateHorizontalEnd = GET_X_LPARAM(lParam);
	object.m_cameraRotateVerticalEnd = GET_Y_LPARAM(lParam);

	auto const Util = [&]()
		{
			DirectX::XMFLOAT4X4 m;

			DirectX::XMMATRIX rotationMatrixGeo =
				DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(object.m_cameraRotateX)) *
				DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(object.m_cameraRotateY)) *
				DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));

			DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(0.0f, 0.0f, object.m_cameraTranslateZ) * rotationMatrixGeo);

			object.SetPosition(object.mPivot.x + m._41, object.mPivot.y + m._42, object.mPivot.z + m._43);
		};

	// The cursor moves to the right - the camera rotates clockwise.
	if (object.m_cameraRotateHorizontalEnd > object.m_cameraRotateHorizontalStart)
	{
		float r = static_cast<float>(object.m_cameraRotateHorizontalEnd - object.m_cameraRotateHorizontalStart) * 0.5f;

		object.m_cameraRotateY += r;

		Util();

		object.RotateY(DirectX::XMConvertToRadians(r));

		object.m_cameraRotateHorizontalStart = object.m_cameraRotateHorizontalEnd;
	}
	// The cursor moves to the left - the camera rotates counterclockwise.
	else if (object.m_cameraRotateHorizontalEnd < object.m_cameraRotateHorizontalStart)
	{
		float r = static_cast<float>(object.m_cameraRotateHorizontalEnd - object.m_cameraRotateHorizontalStart) * 0.5f;

		object.m_cameraRotateY += r;

		Util();

		object.RotateY(DirectX::XMConvertToRadians(r));

		object.m_cameraRotateHorizontalStart = object.m_cameraRotateHorizontalEnd;
	}


	if (object.m_cameraRotateVerticalEnd > object.m_cameraRotateVerticalStart)
	{
		float r = static_cast<float>(object.m_cameraRotateVerticalEnd - object.m_cameraRotateVerticalStart) * 0.5f;

		object.m_cameraRotateX += r;

		Util();

		object.RotateX(DirectX::XMConvertToRadians(r));

		object.m_cameraRotateVerticalStart = object.m_cameraRotateVerticalEnd;
	}
	else if (object.m_cameraRotateVerticalEnd < object.m_cameraRotateVerticalStart)
	{
		float r = static_cast<float>(object.m_cameraRotateVerticalEnd - object.m_cameraRotateVerticalStart) * 0.5f;

		object.m_cameraRotateX += r;

		Util();

		object.RotateX(DirectX::XMConvertToRadians(r));

		object.m_cameraRotateVerticalStart = object.m_cameraRotateVerticalEnd;
	}
}

void Camera::OnMButtonMove(this Camera& object, LPARAM lParam)
{
	object.m_cameraMoveHorizontalEnd = GET_X_LPARAM(lParam);
	object.m_cameraMoveVerticalEnd = GET_Y_LPARAM(lParam);

	// Dividing by 1000 gives a acceptable value for the
	// multiplier to reduce the value entering the function.

	float multiply{};

	// If the Z axis is located with a minus sign.
	if (object.m_cameraTranslateZ < 0)
		multiply = object.m_cameraTranslateZ / -1000.0f;

	// If the Z axis is located with a plus sign.
	if (object.m_cameraTranslateZ > 0)
		multiply = object.m_cameraTranslateZ / 1000.0f;


	// The cursor moves to the right - the camera moves to the left.
	if (object.m_cameraMoveHorizontalEnd > object.m_cameraMoveHorizontalStart)
	{
		object.PivotMoveX(static_cast<float>(object.m_cameraMoveHorizontalStart - object.m_cameraMoveHorizontalEnd) * multiply);

		object.m_cameraMoveHorizontalStart = object.m_cameraMoveHorizontalEnd;
	}
	// The cursor moves to the left - the camera moves to the right.
	else if (object.m_cameraMoveHorizontalEnd < object.m_cameraMoveHorizontalStart)
	{
		object.PivotMoveX(static_cast<float>(object.m_cameraMoveHorizontalStart - object.m_cameraMoveHorizontalEnd) * multiply);

		object.m_cameraMoveHorizontalStart = object.m_cameraMoveHorizontalEnd;
	}

	// The cursor moves up - the camera moves down.
	if (object.m_cameraMoveVerticalEnd > object.m_cameraMoveVerticalStart)
	{
		object.PivotMoveY(static_cast<float>(object.m_cameraMoveVerticalEnd - object.m_cameraMoveVerticalStart) * multiply);

		object.m_cameraMoveVerticalStart = object.m_cameraMoveVerticalEnd;
	}
	// The cursor moves down - the camera moves up.
	else if (object.m_cameraMoveVerticalEnd < object.m_cameraMoveVerticalStart)
	{
		object.PivotMoveY(static_cast<float>(object.m_cameraMoveVerticalEnd - object.m_cameraMoveVerticalStart) * multiply);

		object.m_cameraMoveVerticalStart = object.m_cameraMoveVerticalEnd;
	}
}

void Camera::OnRButtonMove(this Camera& object, LPARAM lParam)
{
	object.m_cameraScaleEnd = GET_Y_LPARAM(lParam);

	// Dividing by 1000 gives a acceptable value for the
	// multiplier to reduce the value entering the function.

	float multiply{};

	// If the Z axis is located with a minus sign.
	if (object.m_cameraTranslateZ < 0)
		multiply = object.m_cameraTranslateZ / -1000.0f;

	// If the Z axis is located with a plus sign.
	if (object.m_cameraTranslateZ > 0)
		multiply = object.m_cameraTranslateZ / 1000.0f;

	// The cursor moves down - the camera approaches the object.
	if (object.m_cameraScaleEnd > object.m_cameraScaleStart)
	{
		object.Scaling(static_cast<float>(object.m_cameraScaleEnd - object.m_cameraScaleStart) * multiply);

		object.m_cameraScaleStart = object.m_cameraScaleEnd;
	}
	// The cursor moves up - the camera moves away from the object.
	else if (object.m_cameraScaleEnd < object.m_cameraScaleStart)
	{
		object.Scaling(static_cast<float>(object.m_cameraScaleEnd - object.m_cameraScaleStart) * multiply);

		object.m_cameraScaleStart = object.m_cameraScaleEnd;
	}
}

// The function moves the camera to the selected object.
// If the object is not selected, the camera will move to the origin.
void Camera::BringCloserToObject(this Camera& object, DirectX::XMFLOAT3 pivot)
{
	// Move the camera pivot.

	object.mPivot = pivot;

	// Rotate the Z axis at the origin of coordinates at the given angles.

	DirectX::XMMATRIX rotationMatrixGeo =
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(object.m_cameraRotateX)) *
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(object.m_cameraRotateY)) *
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));

	DirectX::XMFLOAT4X4 m;

	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(0.0f, 0.0f, object.m_cameraTranslateZ) * rotationMatrixGeo);

	// Using the found coordinates and the pivot coordinates, we move the camera.

	object.SetPosition(object.mPivot.x + m._41, object.mPivot.y + m._42, object.mPivot.z + m._43);

	object.mViewDirty = true;
}