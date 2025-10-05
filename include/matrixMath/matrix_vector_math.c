#include "matrix_vector_math.h"
#include <math.h>
#include <stdio.h>

void print_matrix(const float* matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", matrix[i * 4 + j]);
        }
        printf("\n");
    }
}

void print_vector4(const Vector4* v) {
    printf("Vector4: (%f, %f, %f, %f)\n", v->x, v->y, v->z, v->w);
}

float Vector4_Length(Vector4* v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}

float Vector3_Length(Vector3* v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

void Normalize_Vector4(Vector4* v) {
    float length = Vector4_Length(v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
        v->z /= length;
        v->w /= length;
    }
}

float Vector3_Dot(Vector3 a, Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

void Normalize_Vector3(Vector3* v) {
    float len = Vector3_Length(v);
    if (len > 1e-8f) {
        v->x /= len;
        v->y /= len;
        v->z /= len;
    } else {
        // avoid NaNs for zero-length vectors
        v->x = v->y = v->z = 0.0f;
    }
}

Vector3 Vector3_Add(Vector3 a, Vector3 b) {
    return (Vector3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3 Vector3_Subtract(Vector3 a, Vector3 b) {
    return (Vector3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3 Vector3_Scale(Vector3 a, float scale) {
    return (Vector3){ a.x * scale, a.y * scale, a.z * scale };
}

Vector3 Vector3_Cross(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

Vector4 Vector4FromVector3(Vector3 v, float w) {
    Vector4 result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    result.w = w;
    return result;
}

Matrix4x4 matrix4x4_identity() {
    Matrix4x4 mat = {0};
    mat.m[0] = 1.0f;
    mat.m[5] = 1.0f;
    mat.m[10] = 1.0f;
    mat.m[15] = 1.0f;
    return mat;
}

Matrix4x4 Perspective(float fovDeg, float aspect, float near, float far) {
    float fovRad = fovDeg * (M_PI / 180.0f); // convert degrees to radians
    float f = 1.0f / tanf(fovRad / 2.0f);

    Matrix4x4 mat = {0};

    mat.m[0]  = f / aspect;  // column 0, row 0
    mat.m[5]  = f;           // column 1, row 1
    mat.m[10] = (far + near) / (near - far); // column 2, row 2
    mat.m[11] = -1.0f;       // column 3, row 2
    mat.m[14] = (2.0f * far * near) / (near - far); // column 2, row 3

    // All other entries are already 0
    return mat;
}

Matrix4x4 MultiplyMatrix(const Matrix4x4 a, const Matrix4x4 b) {
    Matrix4x4 result;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result.m[col*4 + row] =
                a.m[0*4 + row] * b.m[col*4 + 0] +
                a.m[1*4 + row] * b.m[col*4 + 1] +
                a.m[2*4 + row] * b.m[col*4 + 2] +
                a.m[3*4 + row] * b.m[col*4 + 3];
        }
    }
    return result;
}

Vector4 MultiplyMatrixVec4(Matrix4x4 mat, Vector4 v) {
    Vector4 result;
    result.x = mat.m[0]*v.x + mat.m[4]*v.y + mat.m[8]*v.z + mat.m[12]*v.w;
    result.y = mat.m[1]*v.x + mat.m[5]*v.y + mat.m[9]*v.z + mat.m[13]*v.w;
    result.z = mat.m[2]*v.x + mat.m[6]*v.y + mat.m[10]*v.z + mat.m[14]*v.w;
    result.w = mat.m[3]*v.x + mat.m[7]*v.y + mat.m[11]*v.z + mat.m[15]*v.w;
    return result;
}

Matrix4x4 TranslateMatrix(const Matrix4x4* mat, float x, float y, float z) {
    Matrix4x4 t = matrix4x4_identity();
    t.m[12] = x;
    t.m[13] = y;
    t.m[14] = z;
    return MultiplyMatrix(*mat, t); // mat * t
}

Matrix4x4 ScaleMatrix(const Matrix4x4* mat, float sx, float sy, float sz) {
    Matrix4x4 s = matrix4x4_identity();
    s.m[0] = sx;
    s.m[5] = sy;
    s.m[10] = sz;
    return MultiplyMatrix(*mat, s);
}

Matrix4x4 RotateMatrix(const Matrix4x4* mat, float angleRad, float x, float y, float z) {
    // normalize axis
    float len = Vector3_Length(&(Vector3){x, y, z});
    if (len > 0.0f) { x/=len; y/=len; z/=len; }

    float c = cosf(angleRad);
    float s = sinf(angleRad);
    float one_c = 1.0f - c;

    Matrix4x4 r = {0};

    r.m[0]  = x*x*one_c + c;
    r.m[1]  = y*x*one_c + z*s;
    r.m[2]  = z*x*one_c - y*s;
    r.m[3]  = 0.0f;

    r.m[4]  = x*y*one_c - z*s;
    r.m[5]  = y*y*one_c + c;
    r.m[6]  = z*y*one_c + x*s;
    r.m[7]  = 0.0f;

    r.m[8]  = x*z*one_c + y*s;
    r.m[9]  = y*z*one_c - x*s;
    r.m[10] = z*z*one_c + c;
    r.m[11] = 0.0f;

    r.m[12] = 0.0f;
    r.m[13] = 0.0f;
    r.m[14] = 0.0f;
    r.m[15] = 1.0f;

    return MultiplyMatrix(*mat, r); // mat * r
}

Matrix4x4 SetMatrixRow(const Matrix4x4* mat, int row, Vector4 vec) {
    if (row < 0 || row > 3) {
        // Invalid row index, return original matrix
        return *mat;
    }
    Matrix4x4 result = *mat;
    result.m[row + 0] = vec.x; // column 0
    result.m[row + 4] = vec.y; // column 1
    result.m[row + 8] = vec.z; // column 2
    result.m[row + 12] = vec.w; // column 3
    return result;
}

Matrix4x4 LookAt(Vector3 eye, Vector3 center, Vector3 up) {
    // 1. forward (f) = normalize(center - eye)
    Vector3 f = Vector3_Subtract(center, eye);
    Normalize_Vector3(&f);

    // 2. right (s) = normalize(cross(f, up))
    Vector3 s = Vector3_Cross(f, up);
    Normalize_Vector3(&s);

    // 3. true up (u) = cross(s, f)
    Vector3 u = Vector3_Cross(s, f);

    Matrix4x4 result = {0};

    // column-major layout (like glm::lookAt)
    result.m[0] = s.x;
    result.m[1] = u.x;
    result.m[2] = -f.x;
    result.m[3] = 0.0f;

    result.m[4] = s.y;
    result.m[5] = u.y;
    result.m[6] = -f.y;
    result.m[7] = 0.0f;

    result.m[8]  = s.z;
    result.m[9]  = u.z;
    result.m[10] = -f.z;
    result.m[11] = 0.0f;

    result.m[12] = -Vector3_Dot(s, eye);
    result.m[13] = -Vector3_Dot(u, eye);
    result.m[14] =  Vector3_Dot(f, eye);
    result.m[15] = 1.0f;

    return result;
}