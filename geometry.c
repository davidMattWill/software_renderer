#include "geometry.h"



//WONT COMPILE WITHOUT USING C99!!!!
void createCube(mesh *mesh){
    mesh->numTris = 12;
    mesh->tris = (triangle*)malloc(sizeof(triangle) * mesh->numTris);

    //face1
    mesh->tris[0] = (triangle){(vertex3d){0,0,0, (u32)0xb3bb51}, (vertex3d){0,1,0,(u32)0x1acff6}, (vertex3d){1,1,0,(u32)0x35ded4}};
    mesh->tris[1] = (triangle){(vertex3d){0,0,0,(u32)0x1acff6}, (vertex3d){1,1,0,(u32)0xe2d135}, (vertex3d){1,0,0,(u32)0xb441d0}};
    //face2
    mesh->tris[2] = (triangle){(vertex3d){1,0,0, (u32)0xb441d0}, (vertex3d){1,1,0,(u32)0xdc6c3c}, (vertex3d){1,1,1,(u32)0xdc6c3c}};
    mesh->tris[3] = (triangle){(vertex3d){1,0,0,(u32)0x1cf932}, (vertex3d){1,1,1,(u32)0x1acff6}, (vertex3d){1,0,1,(u32)0x871cf3}};
    //face3
    mesh->tris[4] = (triangle){(vertex3d){1,0,1, (u32)0x1acff6}, (vertex3d){1,1,1, (u32)0xb441d0}, (vertex3d){0,1,1, (u32)0x35ded4}};
    mesh->tris[5] = (triangle){(vertex3d){1,0,1, (u32)0xdc6c3c}, (vertex3d){0,1,1, (u32)0xe2d135}, (vertex3d){0,0,1, (u32)0x35ded4}};
    //face4
    mesh->tris[6] = (triangle){(vertex3d){0,0,1, (u32)0xdc6c3c}, (vertex3d){0,1,1, (u32)0xe2d135}, (vertex3d){0,1,0, (u32)0x1cf932}};
    mesh->tris[7] = (triangle){(vertex3d){0,0,1, (u32)0x35ded4}, (vertex3d){0,1,0, (u32)0x35ded4}, (vertex3d){0,0,0, (u32)0x35ded4}};
    //face5
    mesh->tris[8] = (triangle){(vertex3d){0,1,0, (u32)0x24ea7f}, (vertex3d){0,1,1, (u32)0x35ded4}, (vertex3d){1,1,1, (u32)0xb441d0}};
    mesh->tris[9] = (triangle){(vertex3d){0,1,0, (u32)0xb3bb51}, (vertex3d){1,1,1, (u32)0}, (vertex3d){1,1,0, (u32)0}};
    //face6
    mesh->tris[10] = (triangle){(vertex3d){1,0,1,(u32)0xe2d135}, (vertex3d){0,0,1, (u32)0x1acff6}, (vertex3d){0,0,0, (u32)0xe2d135}};
    mesh->tris[11] = (triangle){(vertex3d){1,0,1,(u32)0x1acff6}, (vertex3d){0,0,0,(u32)0xb3bb51}, (vertex3d){1,0,0,(u32)0}};

    //REORDER!!
   
}

float **createProjMat(float fov, float zNear, float zFar, float aspectRatio){
    int rows = 4; int cols = 4;
    float **mat = (float**)malloc(rows*sizeof(float*));
    for(int i = 0; i < rows; i++){
        mat[i] = (float *)malloc(cols * sizeof(float));
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            mat[i][j] = 0;
        }
    }

    
    
    float scale = 1 / tanf(fov/2); 
    mat[0][0] = aspectRatio *scale;
    mat[1][1] = scale;
    mat[2][2] = zFar / (zFar - zNear);
    mat[3][2] = (-zFar * zNear) / (zFar - zNear);
    mat[2][3] = 1.0f;
    mat[3][3] = 0.0f;


    return mat;
}

void multProjMat(float **mat,  triangle t, triangle *tProj, int width, int height){

    
    float x,y,z;
    for(int i = 0; i < 3; i++){
        x = t.vertices[i].x;
        y = t.vertices[i].y;
        z = t.vertices[i].z;

        tProj->vertices[i].x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + mat[3][0];
        tProj->vertices[i].y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + mat[3][1];
        tProj->vertices[i].z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + mat[3][2];
        float w = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + mat[3][3];

        if(w != 0){
            tProj->vertices[i].x /= w;
            tProj->vertices[i].y /= w;
            tProj->vertices[i].z /= w;
        }

    //rescale for screen coordinates
        tProj->vertices[i].x += 1;
        tProj->vertices[i].y += 1;

        tProj->vertices[i].x *= 0.5 * (float)width;
        tProj->vertices[i].y *= 0.5 * (float)height; 
        tProj->vertices[i].vertex_color = t.vertices[i].vertex_color;
    }

}


void rotateTriangleX(triangle t, triangle *tRot, float theta){
    float mat[4][4] = {0};
    mat[0][0] = 1;
    mat[1][1] = cosf(theta *0.5);
    mat[1][2] = sinf(theta * 0.5);
    mat[2][1] = -sinf(theta *0.5);
    mat[2][2] = cosf(theta*0.5);
    mat[3][3] = 1.0;
    
    float x,y,z;
    for(int i = 0; i < 3; i++){
        x = t.vertices[i].x;
        y = t.vertices[i].y;
        z = t.vertices[i].z;

        tRot->vertices[i].x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + mat[3][0];
        tRot->vertices[i].y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + mat[3][1];
        tRot->vertices[i].z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + mat[3][2];
        
        float w = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + mat[3][3];

        
            tRot->vertices[i].x /= w;
            tRot->vertices[i].y /= w;
        tRot->vertices[i].z /= w;
        tRot->vertices[i].vertex_color = t.vertices[i].vertex_color;
    }
  
}



void rotateTriangleZ(triangle t, triangle *tRot, float theta){
    float mat[4][4] = {0};
    mat[0][0] = cosf(theta);
    mat[0][1] = sinf(theta);
    mat[1][0] = -sinf(theta);
    mat[1][1] = cosf(theta);
    mat[2][2] = 1.0;
    mat[3][3] = 1.0;
    
    float x,y,z;
    for(int i = 0; i < 3; i++){
        x = t.vertices[i].x;
        y = t.vertices[i].y;
        z = t.vertices[i].z;

        tRot->vertices[i].x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + mat[3][0];
        tRot->vertices[i].y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + mat[3][1];
        tRot->vertices[i].z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + mat[3][2];
        
        float w = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + mat[3][3];

        if(w != 0){
            tRot->vertices[i].x /= w;
            tRot->vertices[i].y /= w;
            tRot->vertices[i].z /= w;
        }
        tRot->vertices[i].vertex_color = t.vertices[i].vertex_color;

    }
  
}


void calculateNormal(triangle t, vertex3d *normal){
    //ORDER IS IMPORTANT,  WONT WORK IF VERTICES ARE NOT CLOCKWISE
    
    vertex3d A, B;
    //vertex 1 - vertex 0
    A.x = t.vertices[1].x - t.vertices[0].x;
    A.y = t.vertices[1].y - t.vertices[0].y;
    A.z = t.vertices[1].z - t.vertices[0].z;

    B.x = t.vertices[2].x - t.vertices[0].x;
    B.y = t.vertices[2].y - t.vertices[0].y;
    B.z = t.vertices[2].z - t.vertices[0].z;

    normal->x = A.y * B.z - A.z * B.y;
    normal->y = A.z * B.x - A.x * B.z;
    normal->z = A.x * B.y - A.y * B.x;

}

vec3d crossProduct(triangle t){
    vertex3d v1,v2,v3;
    v1 = t.vertices[0]; v2 = t.vertices[1]; v3 = t.vertices[2];
    
    vec3d cross;
    cross.x = (v1.y * v2.z) - (v1.z * v2.y);
    cross.y = (v1.x * v2.z) - (v1.z - v2.x);
    cross.z = (v1.x * v2.y) - (v1.y * v1.x);
    return cross;
}






