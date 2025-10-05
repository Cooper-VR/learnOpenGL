#ifndef MATRIX_VECTOR_MATH
#define MATRIX_VECTOR_MATH

    //x,y,z,w are rows
    typedef struct {
        float m[16]; // column-major storage
    } Matrix4x4;

    typedef struct {
        float x, y, z, w;
    } Vector4;

    typedef struct {
        float x, y, z;
    } Vector3;
    
    void print_matrix(const float* matrix);
    void print_vector4(const Vector4* v);
    void Normalize_Vector4(Vector4* v);
    void Normalize_Vector3(Vector3* v);
    Vector3 Vector3_Subtract(Vector3 a, Vector3 b);
    float Vector4_Length(Vector4* v);
    float Vector3_Length(Vector3* v);
    Vector3 Vector3_Cross(Vector3 a, Vector3 b);
    Vector4 Vector4FromVector3(Vector3 v, float w);
    Matrix4x4 matrix4x4_identity();
    Matrix4x4 Perspective(float fovDeg, float aspect, float near, float far);
    Matrix4x4 MultiplyMatrix(const Matrix4x4 a, const Matrix4x4 b);
    Vector4 MultiplyMatrixVec4(Matrix4x4 mat, Vector4 v);
    Matrix4x4 TranslateMatrix(const Matrix4x4* mat, float x, float y, float z);
    Matrix4x4 ScaleMatrix(const Matrix4x4* mat, float x, float y, float z);
    Matrix4x4 RotateMatrix(const Matrix4x4* mat, float angleRad, float x, float y, float z);
    Matrix4x4 SetMatrixRow(const Matrix4x4* mat, int row, Vector4 vec);
    Matrix4x4 LookAt(Vector3 eye, Vector3 center, Vector3 up);
    float Vector3_Dot(Vector3 a, Vector3 b);
    Vector3 Vector3_Add(Vector3 a, Vector3 b);
    Vector3 Vector3_Scale(Vector3 a, float scale);
#endif