// -------------------------------------------------------------------------------------- 
// 
// LisaApplication. Creating 3D primitives and editing their attributes. 
// Copyright (C) 18.8.2024 - 30.9.2026 Deputatov Viktor Maxwellrender@yandex.ru 
// 
// This program is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details. 
// 
// You should have received a copy of the GNU General Public License 
// along with this program. If not, see https://www.gnu.org/licenses/. 
// 
// Project blog https://lisaapplicationblog.blogspot.com/. 
// 
// --------------------------------------------------------------------------------------

#ifndef HELPER_MATH_H
#define HELPER_MATH_H

#include "pch.h"

namespace LisaApp
{
    namespace HelperMath
	{
        inline std::pair<float, float> ConvertWorldSpaceToViewSpace(
            const DirectX::XMMATRIX& getViev,
            const DirectX::XMFLOAT4X4& proj4x4f,
            std::int32_t screenWidth,
            std::int32_t screenHeight,
            float x,
            float y,
            float z
        )
        {
            std::pair<float, float> output;

            float ScreenX{};
            float ScreenY{};
            DirectX::XMMATRIX V = getViev;
            DirectX::XMFLOAT4X4 P = proj4x4f;
            DirectX::XMMATRIX Pp = DirectX::XMMatrixSet(P._11, P._12, P._13, P._14, P._21, P._22, P._23, P._24, P._31, P._32, P._33, P._34, P._41, P._42, P._43, P._44);
            DirectX::XMMATRIX VP = DirectX::XMMatrixMultiply(V, Pp);

            DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1);

            DirectX::XMVECTOR result = DirectX::XMVector3TransformCoord(pos, VP);

            if (result.m128_f32[2] < 1) {
                ScreenX = (result.m128_f32[0] + 1.0f) * screenWidth / 2.0f;
                ScreenY = (1.0f - result.m128_f32[1]) * screenHeight / 2.0f;
            }
            output = { ScreenX, ScreenY };

            return output;
        }

        inline void CalculatingRays(
            const DirectX::XMMATRIX& getViev,
            const DirectX::XMFLOAT4X4& proj4x4f,
            const DirectX::XMMATRIX& matrix,
            DirectX::XMVECTOR& origin,
            DirectX::XMVECTOR& direction,
            std::int32_t sx,
            std::int32_t sy,
            std::int32_t screenWidth,
            std::int32_t screenHeight
        )
        {
            using namespace DirectX;

            DirectX::XMFLOAT4X4 P = proj4x4f;

            // Compute picking ray in view space.
            float vx = (+2.0f * sx / screenWidth - 1.0f) / P(0, 0);
            float vy = (-2.0f * sy / screenHeight + 1.0f) / P(1, 1);

            DirectX::XMMATRIX V = getViev;
            DirectX::XMVECTOR VV = XMMatrixDeterminant(V);
            DirectX::XMMATRIX invView = XMMatrixInverse(&VV, V);

            DirectX::XMMATRIX W = matrix;
            DirectX::XMVECTOR WW = XMMatrixDeterminant(W);
            DirectX::XMMATRIX invWorld = XMMatrixInverse(&WW, W);

            // Tranform ray to view space of Mesh.
            DirectX::XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

            // Ray definition in view space.
            DirectX::XMVECTOR rayOrigin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
            DirectX::XMVECTOR rayDir = DirectX::XMVectorSet(vx, vy, 1.0f, 0.0f);

            rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
            rayDir = XMVector3TransformNormal(rayDir, toLocal);

            // Make the ray direction unit length for the intersection tests.
            rayDir = DirectX::XMVector3Normalize(rayDir);

            origin = rayOrigin;
            direction = rayDir;
        }

        // Function for finding a ray and a plane.
        inline DirectX::XMVECTOR IntersectRayPlane(
            DirectX::FXMVECTOR rayOrigin,
            DirectX::FXMVECTOR rayDirection,
            DirectX::FXMVECTOR plane)
        {
            using namespace DirectX;

            XMVECTOR denom = XMVector3Dot(rayDirection, plane);
            
            float threshold = 0.03f;
            
            if (fabs(XMVectorGetX(denom)) < threshold)
            {
                // The ray is parallel to the plane - no intersection.
                return XMVectorZero();
            }

            XMVECTOR t = XMVectorDivide(
                XMVectorNegate(XMVector3Dot(rayOrigin, plane)),
                denom
            );

            // Intersection point.
            return XMVectorMultiplyAdd(rayDirection, t, rayOrigin);
        }

        // Function for projecting an intersection point onto a plane.
        inline DirectX::XMVECTOR ProjectPointOnPlane(DirectX::FXMVECTOR point, DirectX::FXMVECTOR plane)
        {
            using namespace DirectX;

            // Check the distance from a point to a plane.
            // > 0: the point is "above" the plane
            // < 0: the point is "below" the plane
            // = 0: the point lies on the plane
            float distance = XMVectorGetX(XMVector3Dot(point, plane));

            XMVECTOR normal = XMVectorSwizzle<0, 1, 2, 0>(plane);  // (A,B,C,0)
            return XMVectorSubtract(point, XMVectorMultiply(normal, XMVectorReplicate(distance)));
        }

        // The function result is returned in radians.
        inline float FindingTheAngleBetweenRayAndPlane(DirectX::XMVECTOR rayDirection, DirectX::XMVECTOR plane)
        {
            using namespace DirectX;

            // Normal to the plane.
            XMVECTOR normalFromPlane = XMVectorSet(XMVectorGetX(plane), XMVectorGetY(plane), XMVectorGetZ(plane), 0.0f);

            // Normalize direction and normal.
            XMVECTOR n = XMVector3Normalize(normalFromPlane);
            XMVECTOR dir = XMVector3Normalize(rayDirection);

            // The angle between the ray and the normal. 
            // The angle between two vectors is determined through the scalar product.
            XMVECTOR cosAlphaVec = XMVector3Dot(dir, n);
            float cosAlpha = XMVectorGetX(cosAlphaVec);

            // Limitation in [-1,1]
            cosAlpha = (std::max)(-1.0f, (std::min)(1.0f, cosAlpha));
            // In radians.
            float alpha = acosf(cosAlpha);
            // Angle between the ray and the plane.
            float beta = DirectX::XM_PIDIV2 - alpha;

            //float degrees = XMConvertToDegrees(beta);

            return beta;
        }

		//-----------------------------------------------------------------------------
		// Return true if the vector is a unit vector (length == 1).
		//-----------------------------------------------------------------------------
		inline bool XMVector3IsUnit(_In_ DirectX::FXMVECTOR V) noexcept
		{
			using namespace DirectX;

			XMVECTOR Difference = XMVectorSubtract(XMVector3Length(V), XMVectorSplatOne());

            // DirectX::Internal::g_UnitVectorEpsilon replaced with DirectX::XMVECTORF32{ {{(1.0E-4f), (1.0E-4f), (1.0E-4f), (1.0E-4f)}} }
            // because the Release build produces the following error: g_UnitVectorEpsilon is not a member of namespace DirectX::Internal.
            return XMVector4Less(XMVectorAbs(Difference), DirectX::XMVECTORF32{ {{(1.0E-4f), (1.0E-4f), (1.0E-4f), (1.0E-4f)}} });
		}

        // The function result is returned in radians. (1.57f - beta)
        inline float FindingTheAngleBetweenVectors(DirectX::XMVECTOR v1, DirectX::XMVECTOR v2)
        {
            using namespace DirectX;

            XMVECTOR nv0 = XMVector3Normalize(v1);
            XMVECTOR nv1 = XMVector3Normalize(v2);

            // The angle between the ray and the normal. 
            // The angle between two vectors is determined through the scalar product.
            XMVECTOR cosAlphaVec = XMVector3Dot(nv0, nv1);
            float cosAlpha = XMVectorGetX(cosAlphaVec);

            // Limitation in [-1,1]
            cosAlpha = (std::max)(-1.0f, (std::min)(1.0f, cosAlpha));
            // In radians.
            float alpha = acosf(cosAlpha);
            // Angle between the ray and the plane.
            float beta = DirectX::XM_PIDIV2 - alpha;

            float radians = 1.57f - beta;

            return radians;
        }

        inline bool XM_CALLCONV IsPointNearRay(
            const DirectX::XMVECTOR& point,
            const DirectX::XMVECTOR& Origin,
            const DirectX::XMVECTOR& Direction,
            float threshold,
            float& Dist
        ) noexcept
        {
            using namespace DirectX;


            XMVECTOR toPoint = XMVectorSubtract(point, Origin);

            XMVECTOR t = XMVector3Dot(toPoint, Direction);

            XMVECTOR closestPoint = XMVectorAdd(Origin, XMVectorMultiply(t, Direction));

            XMVECTOR distance = XMVector3LengthSq(XMVectorSubtract(closestPoint, point));


            DirectX::XMStoreFloat(&Dist, distance);

            return (distance.m128_f32[0] < threshold) ? true : false;
        }

        inline bool XM_CALLCONV RayIntersectsSegment(
            const DirectX::XMVECTOR& Origin,
            const DirectX::XMVECTOR& Direction,
            const DirectX::XMVECTOR& V0,
            const DirectX::XMVECTOR& V1,
            float threshold,
            float& Dist
        )
        {
            using namespace DirectX;

            if (DirectX::XMVector3IsNaN(Origin) || DirectX::XMVector3IsNaN(Direction))
            {
                return false;
            }

            assert(LisaApp::HelperMath::XMVector3IsUnit(Direction));

            bool ret{};

            XMVECTOR v1 = XMVectorSubtract(V0, Origin);
            XMVECTOR v2 = XMVectorSubtract(V1, Origin);  


            XMVECTOR nv1 = XMVector3Normalize(v1);
            XMVECTOR nv2 = XMVector3Normalize(v2);

            XMVECTOR S = XMVector3Length(XMVector3Cross(nv1, nv2));

            XMVECTOR s1 = XMVector3Length(XMVector3Cross(nv1, Direction));
            XMVECTOR s2 = XMVector3Length(XMVector3Cross(nv2, Direction));

            XMVECTOR sum = XMVectorAdd(s1, s2);


            if (XMVectorGetX(sum) <= XMVectorGetX(S) + threshold)
            {
                XMVECTOR e = XMVectorSubtract(V1, V0);
                XMVECTOR s = XMVectorSubtract(Origin, V0);

                XMVECTOR cross = XMVector3Cross(e, s);

                XMVECTOR V0a = XMVectorAdd(cross, V0);
                XMVECTOR V2 = XMVectorAdd(XMVectorNegate(cross), V0);

                XMVECTOR e1 = XMVectorSubtract(V0a, V1);
                XMVECTOR e2 = XMVectorSubtract(V2, V1);

                // p = Direction ^ e2;
                XMVECTOR p = XMVector3Cross(Direction, e2);

                // det = e1 * p;
                XMVECTOR det = XMVector3Dot(e1, p);

                // q = s ^ e1;
                XMVECTOR q = XMVector3Cross(s, e1);

                // t = e2 * q;
                XMVECTOR t = XMVector3Dot(e2, q);

                t = XMVectorDivide(t, det);


                Dist = XMVectorGetX(t);


                /*wchar_t msg[128]{};
                swprintf_s(msg, L"intersection x: % f\n", intersection.m128_f32[0]);
                OutputDebugString(msg);
                swprintf_s(msg, L"intersection y: % f\n", intersection.m128_f32[1]);
                OutputDebugString(msg);
                swprintf_s(msg, L"intersection z: % f\n", intersection.m128_f32[2]);
                OutputDebugString(msg);*/


                ret = true;
            }
            else
            {
                ret = false;
            }

            return ret;
        }

        //-----------------------------------------------------------------------------
        // Compute the intersection of a ray (Origin, Direction) with a triangle
        // (V0, V1, V2).  Return true if there is an intersection and also set *pDist
        // to the distance along the ray to the intersection.
        //
        // The algorithm is based on Moller, Tomas and Trumbore, "Fast, Minimum Storage
        // Ray-Triangle Intersection", Journal of Graphics Tools, vol. 2, no. 1,
        // pp 21-28, 1997.
        //-----------------------------------------------------------------------------
		inline bool XM_CALLCONV Intersects(
			const DirectX::FXMVECTOR& Origin,
			const DirectX::FXMVECTOR& Direction,
			const DirectX::FXMVECTOR& V0,
			const DirectX::GXMVECTOR& V1,
			const DirectX::HXMVECTOR& V2,
			float& Dist
		) noexcept
        {
            using namespace DirectX;

            assert(LisaApp::HelperMath::XMVector3IsUnit(Direction));

            XMVECTOR Zero = XMVectorZero();

            XMVECTOR e1 = XMVectorSubtract(V1, V0);
            XMVECTOR e2 = XMVectorSubtract(V2, V0);

            // p = Direction ^ e2;
            XMVECTOR p = XMVector3Cross(Direction, e2);

            // det = e1 * p;
            XMVECTOR det = XMVector3Dot(e1, p);

            XMVECTOR u, v, t;

            if (XMVector3GreaterOrEqual(det, g_RayEpsilon))
            {
                // Determinate is positive (front side of the triangle).
                XMVECTOR s = XMVectorSubtract(Origin, V0);

                // u = s * p;
                u = XMVector3Dot(s, p);

                XMVECTOR NoIntersection = XMVectorLess(u, Zero);

                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(u, det));

                // q = s ^ e1;
                XMVECTOR q = XMVector3Cross(s, e1);

                // v = Direction * q;
                v = XMVector3Dot(Direction, q);

                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(v, Zero));
                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(XMVectorAdd(u, v), det));

                // t = e2 * q;
                t = XMVector3Dot(e2, q);

                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(t, Zero));

                if (XMVector4EqualInt(NoIntersection, XMVectorTrueInt()))
                {
                    Dist = 0.f;
                    return false;
                }
            }
            else if (XMVector3LessOrEqual(det, DirectX::g_RayNegEpsilon))
            {
                // Determinate is negative (back side of the triangle).
                XMVECTOR s = XMVectorSubtract(Origin, V0);

                // u = s * p;
                u = XMVector3Dot(s, p);

                XMVECTOR NoIntersection = XMVectorGreater(u, Zero);
                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(u, det));

                // q = s ^ e1;
                XMVECTOR q = XMVector3Cross(s, e1);

                // v = Direction * q;
                v = XMVector3Dot(Direction, q);

                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(v, Zero));
                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorAdd(u, v), det));

                // t = e2 * q;
                t = XMVector3Dot(e2, q);

                NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(t, Zero));

                if (XMVector4EqualInt(NoIntersection, XMVectorTrueInt()))
                {
                    Dist = 0.f;
                    return false;
                }
            }
            else
            {
                // Parallel ray.
                Dist = 0.f;
                return false;
            }

            t = XMVectorDivide(t, det);

            // (u / det) and (v / dev) are the barycentric cooridinates of the intersection.

            // Store the x-component to *pDist
            DirectX::XMStoreFloat(&Dist, t);

            return true;
        }

	}
}

#endif // !HELPER_MATH_H
