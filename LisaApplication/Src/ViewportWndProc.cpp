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

#include "Viewport.h"
#include "Globals.h"
#include "DefaultElementsData.h"

extern struct LisaApp::Default::SphereData gSphereData;
extern struct LisaApp::Default::GeoSphereData gGeoSphereData;
extern struct LisaApp::Default::CubeData gCubeData;
extern struct LisaApp::Default::CylinderData gCylinderData;
extern struct LisaApp::Default::ConeData gConeData;
extern struct LisaApp::Default::TorusData gTorusData;
extern struct LisaApp::Default::PlaneData gPlaneData;

// The main window message handler.
LRESULT CALLBACK Viewport::ViewportWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps{};

    using namespace LisaApp::Global;

    static UINT sComponents{ selection::mesh };

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        Viewport* pWindow = (Viewport*)pcs->lpCreateParams;
        // The "SetWindowLongPtrW" function allows you to create several windows
        // using one "WindowProc" function for all.
        //
        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

        DX::ThrowIfFailed(RoInitialize(RO_INIT_MULTITHREADED));

        pWindow->InitializationResources(hwnd, 800, 600);

        // Save the working HWND.
        pWindow->SetHwnd(hwnd);

        result = 1;
    }
    else
    {
        Viewport* pWindow = reinterpret_cast<Viewport*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));

        bool wasHandled = false;

        if (pWindow)
        {
            switch (message)
            {
            case WM_KEYDOWN:
            {
                pWindow->OnKeyDown(hwnd, wParam);
            }
            return 0;
            break;
            case WM_ACTIVATE:
                // WM_ACTIVATE is sent when the window is activated or deactivated.  
                // We pause the game when the window is deactivated and unpause it 
                // when it becomes active.  
                if (LOWORD(wParam) == WA_INACTIVE)
                {
                    pWindow->mAppPaused = true;
                    pWindow->mTimer.Stop();
                }
                else
                {
                    pWindow->mAppPaused = false;
                    pWindow->mTimer.Start();
                }
                return 0;
                break;
            case WM_GETMINMAXINFO:
            {
                HelperWTools tools;

                tools.MinMaxWindow(lParam, pWindow->GetRight(), pWindow->GetBottom());
            }
            return 0;
            break;
            case WM_LBUTTONDOWN:
            {
                if (GetFocus() != hwnd)
                    SetFocus(hwnd);
                
                if (HIBYTE(GetKeyState(VK_MENU)) & 0x80)
                {
                    // The following conditions only work while the Alt key is pressed.
                    if ((DWORD)wParam & MK_LBUTTON)
                    {
                        // Capture the initial coordinates for camera rotation.
                        pWindow->m_camera.OnLButtonDown(lParam);
                    }
                }
                else if (HIBYTE(GetKeyState(VK_SHIFT)) & 0x80)
                {
                    // Selecting objects.
                    pWindow->OnMouseDown(lParam, pWindow->m_width, pWindow->m_height, true);
                }
                else
                {
                    // Selecting objects.
                    pWindow->OnMouseDown(lParam, pWindow->m_width, pWindow->m_height);
                }

                InvalidateRect(hwnd, NULL, false);
            }
            return 0;
            break;
            case WM_LBUTTONUP:
            {
                pWindow->OnMouseUp();

                InvalidateRect(hwnd, NULL, false);
            }
            return 0;
            break;
            case WM_RBUTTONDOWN:
            {
                if (HIBYTE(GetKeyState(VK_MENU)) & 0x80)
                {
                    // The following conditions only work while the Alt key is pressed.
                    if ((DWORD)wParam & MK_RBUTTON)
                    {
                        // Capture initial coordinates for camera zoom in/out.
                        pWindow->m_camera.OnRButtonDown(lParam);
                    }
                }

                InvalidateRect(hwnd, NULL, false);
            }
            return 0;
            break;
            case WM_MBUTTONDOWN:
            {
                if (HIBYTE(GetKeyState(VK_MENU)) & 0x80)
                {
                    // The following conditions only work while the Alt key is pressed.
                    if ((DWORD)wParam & MK_MBUTTON)
                    {
                        // Capture the initial coordinates for camera movement.
                        pWindow->m_camera.OnMButtonDown(lParam);
                    }
                }

                InvalidateRect(hwnd, NULL, false);
            }
            return 0;
            break;
            case WM_MBUTTONUP:
                return 0;
                break;
            case WM_MOUSEMOVE:
            {
                if (HIBYTE(GetKeyState(VK_MENU)) & 0x80)
                {
                    // The following conditions only work while the Alt key is pressed.
                    if ((DWORD)wParam & MK_LBUTTON)
                    {
                        // Rotate the camera.
                        pWindow->m_camera.OnLButtonMove(lParam);
                        //pWindow->m_camera.UpdateViewMatrix();

                        pWindow->m_pivot->SetPivotDirty(true);
                        pWindow->m_pivot->HidingPivotComponents(pWindow->m_camera.GetLook());
                        //pWindow->m_pivot->CircleFacingTheCamera(pWindow->m_camera.GetCameraRotateX(), pWindow->m_camera.GetCameraRotateY());
                    }
                    else if ((DWORD)wParam & MK_MBUTTON)
                    {
                        // Move the camera.
                        pWindow->m_camera.OnMButtonMove(lParam);
                        //pWindow->m_camera.UpdateViewMatrix();

                        pWindow->m_pivot->SetPivotDirty(true);
                        pWindow->m_pivot->HidingPivotComponents(pWindow->m_camera.GetLook());
                        //pWindow->m_pivot->CircleFacingTheCamera(pWindow->m_camera.GetCameraRotateX(), pWindow->m_camera.GetCameraRotateY());
                    }
                    else if ((DWORD)wParam & MK_RBUTTON)
                    {
                        // Zoom the camera out/in.
                        pWindow->m_camera.OnRButtonMove(lParam);
                        //pWindow->m_camera.UpdateViewMatrix();

                        pWindow->m_pivot->SetPivotDirty(true);
                        pWindow->m_pivot->HidingPivotComponents(pWindow->m_camera.GetLook());
                        //pWindow->m_pivot->CircleFacingTheCamera(pWindow->m_camera.GetCameraRotateX(), pWindow->m_camera.GetCameraRotateY());
                    }
                }
                else
                {
                    SetCursor(LoadCursorW(0, IDC_ARROW));

                    pWindow->m_camera.UpdateViewMatrix();//???
                    pWindow->OnMouseMove(wParam, lParam, pWindow->m_width, pWindow->m_height);
                }

                InvalidateRect(hwnd, NULL, false);
            }
            return 0;
            break;
            case WM_MOUSELEAVE:
            {
                //pWindow->OnMouseUp();

                //InvalidateRect(hwnd, NULL, false);
            }
            return 0;
            break;
            case WM_COMMAND:
            {
                UINT wmId = LOWORD(wParam);

                switch (wmId)
                {
                case UI::GETTING_DATA_DURING_INITIALIZATION:
                {
                    // Get the dimensions of the parent window at the time of creation.
                    RECT rect;
                    GetClientRect(GetAncestor(hwnd, GA_PARENT), &rect);

                    pWindow->m_parentWindowWidth = rect.right;
                    pWindow->m_parentWindowHeight = rect.bottom;
                }
                break;
                case UI::RESIZING_CHILD_WINDOW_FROM_SIMPLE:
                {
                    INT l{ pWindow->GetLeft() }, t{ pWindow->GetTop() }, r{ pWindow->GetRight() }, b{ pWindow->GetBottom() };

                    pWindow->RestorePosition(
                        hwnd,
                        pWindow->m_parentWindowWidth,
                        pWindow->m_parentWindowHeight,
                        pWindow->GetTransform(),
                        pWindow->GetNonClientAreaSize(),
                        pWindow->GetLeft(),
                        pWindow->GetTop(),
                        pWindow->GetRight(),
                        pWindow->GetBottom(),
                        l, t, r, b
                    );

                    SetWindowPos(hwnd, 0, l, t, r, b, SWP_NOZORDER | SWP_NOACTIVATE);
                }
                break;
                case UI::RESIZING_CHILD_WINDOW_FROM_INBUILT:
                {
                    // Get the parent window handle once
                    auto getParent = [](HWND hwnd) -> std::expected<HWND, DWORD>
                        {
                            HWND parent = GetAncestor(hwnd, GA_PARENT);
                            if (!parent)
                                return std::unexpected(GetLastError());
                            return parent;
                        };

                    const auto getRect = [](HWND hwnd, bool rectType) -> std::expected<RECT, DWORD>
                        {
                            RECT rc{};
                            const auto success = rectType ? GetWindowRect(hwnd, &rc) : GetClientRect(hwnd, &rc);
                            return success ? std::expected<RECT, DWORD>(rc) : std::unexpected(GetLastError());
                        };

                    const auto& hParent = getParent(hwnd);

                    if (!hParent.has_value())
                        throw std::runtime_error("Failed to get parent HWND for separators");

                    const auto& parentRect = getRect(*hParent, false);

                    if (!parentRect)
                        throw std::runtime_error("Failed to get parent rects for separators");


                    INT l{ pWindow->GetLeft() }, t{ pWindow->GetTop() }, r{ pWindow->GetRight() }, b{ pWindow->GetBottom() };

                    if (pWindow->GetTransform() & UI::ui_transform::stretching_x)
                        r = parentRect->right - (pWindow->m_parentWindowWidth - pWindow->GetRight());

                    if (pWindow->GetTransform() & UI::ui_transform::stretching_y)
                        b = parentRect->bottom - (pWindow->m_parentWindowHeight - pWindow->GetBottom());

                    if (pWindow->GetTransform() & UI::ui_transform::restore_lx)
                        l += (parentRect->right - pWindow->m_parentWindowWidth);

                    if (pWindow->GetTransform() & UI::ui_transform::restore_ty)
                        t += (parentRect->bottom - pWindow->m_parentWindowHeight);

                    // Check for missing flag to avoid conflicts.
                    if (pWindow->GetTransform() != UI::ui_transform::none)
                    {
                        if (!SetWindowPos(hwnd, nullptr, l, t, r, b, SWP_NOZORDER | SWP_NOACTIVATE))
                        {
                            throw std::system_error(
                                std::error_code(GetLastError(), std::system_category()),
                                "Window position update failed"
                            );
                        }
                    }
                }
                break;
                case UI::CREATE_PRIMITIVES:
                {
                    using namespace LisaApp::Default;
                    
                    LisaApp::PrimitivesData pd;
                    
                    if ((UINT(lParam)) == LisaApp::Global::SPHERE)
                    {
                        pd = { 
                            .Radius = gSphereData.Radius,
                            .SubdivisionsHeight = gSphereData.SubdivisionsHeight,
                            .SubdivisionsAxis = gSphereData.SubdivisionsAxis
                        };
                    }
                    else if ((UINT(lParam)) == LisaApp::Global::GEO_SPHERE)
                    {
                        pd = {
                            .Radius = gGeoSphereData.Radius,
                            .Subdivisions = gGeoSphereData.Subdivisions,
                            .RhCoords = gGeoSphereData.RhCoords
                        };
                    }
                    else if ((UINT(lParam)) == LisaApp::Global::CUBE)
                    {
                        pd = {
                            .Width = gCubeData.Width,
                            .Height = gCubeData.Height,
                            .Depth = gCubeData.Depth,
                            .SubdivisionsWidth = gCubeData.SubdivisionsWidth,
                            .SubdivisionsHeight = gCubeData.SubdivisionsHeight,
                            .SubdivisionsDepth = gCubeData.SubdivisionsDepth
                        };
                    }
                    else if ((UINT(lParam)) == LisaApp::Global::CYLINDER)
                    {
                        pd = {
                            .Radius = gCylinderData.Radius,
                            .Height = gCylinderData.Height,
                            .SubdivisionsHeight = gCylinderData.SubdivisionsHeight,
                            .SubdivisionsAxis = gCylinderData.SubdivisionsAxis,
                            .SubdivisionsCaps = gCylinderData.SubdivisionsCaps
                        };
                    }
                    else if ((UINT(lParam)) == LisaApp::Global::CONE)
                    {
                        pd = {
                            .Radius = gConeData.Radius,
                            .Height = gConeData.Height,
                            .SubdivisionsHeight = gConeData.SubdivisionsHeight,
                            .SubdivisionsAxis = gConeData.SubdivisionsAxis,
                            .SubdivisionsCaps = gConeData.SubdivisionsCaps
                        };
                    }
                    else if ((UINT(lParam)) == LisaApp::Global::TORUS)
                    {
                        pd = {
                            .Radius = gTorusData.Radius,
                            .SubdivisionsHeight = gTorusData.SubdivisionsHeight,
                            .SubdivisionsAxis = gTorusData.SubdivisionsAxis,
                            .SectionRadius = gTorusData.SectionRadius,
                            .RhCoords = gTorusData.RhCoords
                        };
                    }
                    else if ((UINT(lParam)) == LisaApp::Global::PLANE)
                    {
                        pd = {
                            .Width = gPlaneData.Width,
                            .Depth = gPlaneData.Depth,
                            .SubdivisionsWidth = gPlaneData.SubdivisionsWidth,
                            .SubdivisionsDepth = gPlaneData.SubdivisionsDepth
                        };
                    }

                    pWindow->CreateSceneObjects(pd, (UINT(lParam)));


                    InvalidateRect(hwnd, NULL, false);

                    SetFocus(hwnd);
                }
                break;
                default:
                    return DefWindowProc(hwnd, message, wParam, lParam);
                }
            }
            break;
            case WM_SIZE:
            {
                // Save the new client area dimensions.
                pWindow->m_width = GET_X_LPARAM(lParam);
                pWindow->m_height = GET_Y_LPARAM(lParam);

                if (pWindow->m_deviceResources->GetD3DDevice())
                {
                    if (wParam == SIZE_MINIMIZED)
                    {
                        pWindow->mAppPaused = true;
                        pWindow->mMinimized = true;
                        pWindow->mMaximized = false;
                    }
                    else if (wParam == SIZE_MAXIMIZED)
                    {
                        pWindow->mAppPaused = false;
                        pWindow->mMinimized = false;
                        pWindow->mMaximized = true;
                        pWindow->OnWindowSizeChanged(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    }
                    else if (wParam == SIZE_RESTORED)
                    {
                        // Restoring from minimized state?
                        if (pWindow->mMinimized)
                        {
                            pWindow->mAppPaused = false;
                            pWindow->mMinimized = false;
                            pWindow->OnWindowSizeChanged(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                        }

                        // Restoring from maximized state?
                        else if (pWindow->mMaximized)
                        {
                            pWindow->mAppPaused = false;
                            pWindow->mMaximized = false;
                            pWindow->OnWindowSizeChanged(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                        }
                        else if (pWindow->mResizing)
                        {
                            // If user is dragging the resize bars, we do not resize 
                            // the buffers here because as the user continuously 
                            // drags the resize bars, a stream of WM_SIZE messages are
                            // sent to the window, and it would be pointless (and slow)
                            // to resize for each WM_SIZE message received from dragging
                            // the resize bars.  So instead, we reset after the user is 
                            // done resizing the window and releases the resize bars, which 
                            // sends a WM_EXITSIZEMOVE message.
                        }
                        else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                        {
                            pWindow->OnWindowSizeChanged(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                        }
                    }
                } 
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_ENTERSIZEMOVE:
            {
                // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
                pWindow->mAppPaused = true;
                pWindow->mResizing = true;
                pWindow->mTimer.Stop();
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_EXITSIZEMOVE:
            {
                // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
                // Here we reset everything based on the new window dimensions.
                pWindow->mAppPaused = false;
                pWindow->mResizing = false;
                pWindow->mTimer.Start();
                pWindow->OnWindowSizeChanged(pWindow->m_width, pWindow->m_height);
            }
            result = 0;
            wasHandled = true;
            break;
            //case WM_MENUCHAR:
            //{
            //    // The WM_MENUCHAR message is sent when a menu is active and the user presses 
            //    // a key that does not correspond to any mnemonic or accelerator key. 
            //    
            //    // Don't beep when we alt-enter.
            //    return MAKELRESULT(0, MNC_CLOSE);
            //}
            //result = 0;
            //wasHandled = true;
            //break;
            case WM_DISPLAYCHANGE:
            case WM_PAINT:
            {               
                BeginPaint(hwnd, &ps);
                pWindow->OnWindowSizeChanged(pWindow->m_width, pWindow->m_height);
                pWindow->Update();
                pWindow->Draw();
                EndPaint(hwnd, &ps);
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}
