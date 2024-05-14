#include <stdlib.h>
#include <stdio.h>
#include "math.h"

typedef struct vec3d{
    float x,y,z;
}vec3d;

typedef struct triangle{
    vec3d vertices[3];
}triangle;

typedef struct mesh{
    int numTris;
    triangle *tris;
}mesh;

void createCube(mesh *mesh);
void createMesh();


float** createProjMat(float fov, float fNear, float fFar, float aspectRatio);
void multProjMat(float **mat, triangle t, triangle *tProj, int width, int height);
void rotateTriangleX(triangle t, triangle *tRot, float theta);
void rotateTriangleY(triangle t, triangle *tRot, float theta);
void rotateTriangleZ(triangle t, triangle *tRot, float theta);

void calculateNormal(triangle t, vec3d *normal);
