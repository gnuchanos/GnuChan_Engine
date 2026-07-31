#include "context_gl_windows.h"

#include "core/error_macros.h"
#include "core/print_string.h"

#include "glad/glad.h"
#include <stdio.h>

#ifndef WGL_ARB_create_context
#define WGL_ARB_create_context 1
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define ERROR_INVALID_VERSION_ARB 0x2095
#define ERROR_INVALID_PROFILE_ARB 0x2096
#endif

typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

static void load_wgl_extensions() {
    // Create a dummy window/context to get wgl extensions
    HWND dummy = CreateWindowA("STATIC", "dummy", WS_OVERLAPPED, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
    HDC dummy_dc = GetDC(dummy);
    
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    
    int pf = ChoosePixelFormat(dummy_dc, &pfd);
    SetPixelFormat(dummy_dc, pf, &pfd);
    HGLRC dummy_rc = wglCreateContext(dummy_dc);
    wglMakeCurrent(dummy_dc, dummy_rc);
    
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(dummy_rc);
    ReleaseDC(dummy, dummy_dc);
    DestroyWindow(dummy);
}

ContextGL_Windows::ContextGL_Windows(HWND hWnd, bool p_gles3_context) {
    this->hWnd = hWnd;
    this->hDC = NULL;
    this->hRC = NULL;
    this->hRC_offscreen = NULL;
    this->gles3_context = false; // Always desktop GL, never ES
    this->opengl_43_core = true;
}

ContextGL_Windows::~ContextGL_Windows() {
    if (hRC) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC);
    }
    if (hRC_offscreen) {
        wglDeleteContext(hRC_offscreen);
    }
    if (hDC) {
        ReleaseDC(hWnd, hDC);
    }
}

Error ContextGL_Windows::initialize() {
    hDC = GetDC(hWnd);
    if (!hDC) {
        return ERR_CANT_OPEN;
    }

    // Set up pixel format
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hDC, &pfd);
    if (!pf) {
        return ERR_CANT_OPEN;
    }
    SetPixelFormat(hDC, pf, &pfd);

    // Load WGL extensions (needs a temporary context)
    load_wgl_extensions();

    if (wglCreateContextAttribsARB) {
        // Create OpenGL 4.3 core profile context
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
    }

    if (!hRC) {
        // Fallback: create legacy context
        hRC = wglCreateContext(hDC);
        if (!hRC) {
            return ERR_CANT_OPEN;
        }
    }

    wglMakeCurrent(hDC, hRC);

    // Initialize glad OpenGL loader
    if (!gladLoadGL()) {
        print_line("ERROR: gladLoadGL failed");
        return ERR_CANT_OPEN;
    }
    
    // Print GL version to verify
    const unsigned char *ver = glGetString(GL_VERSION);
    if (ver) {
        print_line(String("OpenGL ") + (const char *)ver + " initialized");
    }

    return OK;
}

void ContextGL_Windows::make_current() {
    wglMakeCurrent(hDC, hRC);
}

void ContextGL_Windows::release_current() {
    wglMakeCurrent(NULL, NULL);
}

void ContextGL_Windows::make_current_offscreen() {
    if (hRC_offscreen) {
        wglMakeCurrent(hDC, hRC_offscreen);
    }
}

void ContextGL_Windows::release_current_offscreen() {
    wglMakeCurrent(NULL, NULL);
}

void ContextGL_Windows::set_use_vsync(bool p_use) {
    typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(p_use ? 1 : 0);
    }
}

void ContextGL_Windows::set_offscreen_gl_current(bool p_offscreen) {
    if (p_offscreen) {
        make_current_offscreen();
    } else {
        make_current();
    }
}

void ContextGL_Windows::set_offscreen_gl_rc(HGLRC rc) {
    hRC_offscreen = rc;
}
