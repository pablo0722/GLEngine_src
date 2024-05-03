// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 3.0 Programming Guide.
//

#include <stdio.h>
#include <string.h>

#include <EGL/eglplatform.h>

#include "GLEngine/GLEngine.hpp"

static void keyFuncWrapper(void *ctx, unsigned char key, int x, int y) {
    if(ctx) {
        GLEngine *engine = (GLEngine *) ctx;

        engine->callKeyFunc(engine, key, x, y);
    }
}

bool GLEngine::create(const char *title, int posx, int posy, int width, int height, GLuint flags) {
    khr.CreateSurfaceAndBindContext(title, posx, posy, width, height, flags);

    gettimeofday(&t1 , &tz);

    return true;
}

void GLEngine::loop(void *ctx) {
    bool done = false;
    WindowEvent event;

    while(!done) {
        khr.getEvent(&event);
        while(WindowEvent::Type::NoEvent != event.type) {
            switch (event.type)
            {
                case WindowEvent::Type::DeleteEvent: {
                    done = true;
                } break;

                case WindowEvent::Type::KeyPressEvent: {
                    this->keyFunc(ctx, event.keyPressed, event.x, event.y);
                } break;
                
                default: {
                } break;
            }

            if(done) {
                break;
            }

            khr.getEvent(&event);
        }

        if(done) {
            break;
        }

        gettimeofday(&t2, &tz);
        deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
        t1 = t2;

        callUpdateFunc(this, deltatime);
        callDrawFunc(this);

        khr.swapBuffers();
    }

    callShutdownFunc(this);
}

//
/// \brief Register a draw callback function to be used to render each frame
/// \param context Application context
/// \param drawFunc Draw callback function that will be used to render the scene
//
void GLEngine::registerDrawFunc(void(*drawFunc)(void *ctx)) {
    this->drawFunc = drawFunc;
}

//
/// \brief Register a callback function to be called on shutdown
/// \param context Application context
/// \param shutdownFunc Shutdown callback function
//
void GLEngine::registerShutdownFunc(void(*shutdownFunc)(void *ctx)) {
    this->shutdownFunc = shutdownFunc;
}

//
/// \brief Register an update callback function to be used to update on each time step
/// \param context Application context
/// \param updateFunc Update callback function that will be used to render the scene
//
void GLEngine::registerUpdateFunc(void(*updateFunc)(void *ctx, float deltatime)) {
    this->updateFunc = updateFunc;
}

//
/// \brief Register a keyboard input processing callback function
/// \param context Application context
/// \param keyFunc Key callback function for application processing of keyboard input
//
void GLEngine::registerKeyFunc(void(*keyFunc)(void *ctx, unsigned char keyChar, int x, int y)) {
    this->keyFunc = keyFunc;
}

void GLEngine::callDrawFunc(void *ctx) {
    if(drawFunc) {
        drawFunc(ctx);
    }
}

void GLEngine::callShutdownFunc(void *ctx) {
    if(shutdownFunc) {
        shutdownFunc(ctx);
    }
}

void GLEngine::callUpdateFunc(void *ctx, float deltatime) {
    if(updateFunc) {
        updateFunc(ctx, deltatime);
    }
}

void GLEngine::callKeyFunc(void *ctx, unsigned char keyChar, int x, int y) {
    if(keyFunc) {
        keyFunc(ctx, keyChar, x, y);
    }
}
