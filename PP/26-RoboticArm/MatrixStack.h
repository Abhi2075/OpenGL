#pragma once

#include "vmath.h"

using namespace vmath;

mat4 modelViewMatrix;
mat4 modelViewProjectionMatrix;
mat4 translationMatrix;
mat4 scaleMatrix;
mat4 rotationMatrix;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

/*--------------------------------- STACK [START] ---------------------------------*/

#define NUMBER_OF_MAX_STACK_ELEMENTS 32

int top = -1;
mat4 matrixArray[NUMBER_OF_MAX_STACK_ELEMENTS];
mat4 originalModelViewMatrixArray[NUMBER_OF_MAX_STACK_ELEMENTS];
bool isMatrixMode = false;

//TODO: Check for stack overflow condition based on NUMBER_OF_MAX_STACK_ELEMENTS.
inline void pushMatrix(void)
{
    top++;
    originalModelViewMatrixArray[top] = top == 0 ? modelViewMatrix : originalModelViewMatrixArray[top - 1];

    matrixArray[top] = top == 0 ? modelViewMatrix : matrixArray[top - 1];

    isMatrixMode = true;
}

inline void popMatrix(void)
{
    modelViewMatrix = originalModelViewMatrixArray[top];
    matrixArray[top] = mat4::identity();
    originalModelViewMatrixArray[top] = mat4::identity();
    top--;

    if (top == -1)
    {
        isMatrixMode = false;
    }
}

/*--------------------------------- STACK [END] ---------------------------------*/

inline void setTranslation(void)
{
    if (isMatrixMode)
    {
        matrixArray[top] = matrixArray[top] * translationMatrix;
        modelViewProjectionMatrix = perspectiveProjectionMatrix * matrixArray[top];
    }
    else
    {
        modelViewMatrix = modelViewMatrix * translationMatrix;
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    }

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
}

inline void setScale(void)
{
    if (isMatrixMode)
    {
        matrixArray[top] = matrixArray[top] * scaleMatrix;
        modelViewProjectionMatrix = perspectiveProjectionMatrix * matrixArray[top];
    }
    else
    {
        modelViewMatrix = modelViewMatrix * scaleMatrix;
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    }

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
}

inline void setRotation(void)
{
    if (isMatrixMode)
    {
        matrixArray[top] = matrixArray[top] * rotationMatrix;
        modelViewProjectionMatrix = perspectiveProjectionMatrix * matrixArray[top];
    }
    else
    {
        modelViewMatrix = modelViewMatrix * rotationMatrix;
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    }

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
}

inline void loadIdentity(void)
{
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();
    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();

    /*------------------ Reset Matrix Stack [START] ------------------*/
    for (int i = 0; i < NUMBER_OF_MAX_STACK_ELEMENTS; i++)
    {
        matrixArray[i] = mat4::identity();
        originalModelViewMatrixArray[i] = mat4::identity();
    }

    top = -1;
    /*------------------ Reset Matrix Stack [END] ------------------*/

    setTranslation();
    setRotation();
}
