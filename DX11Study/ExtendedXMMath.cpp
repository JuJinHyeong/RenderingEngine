#include "ExtendedXMMath.h"
#include <assimp/scene.h>

DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& matrix) {
    DirectX::XMFLOAT3 euler = { 0.0f, 0.0f, 0.0f };

    euler.x = asinf(-matrix._32);
    if (cosf(euler.x) > 0.0001) {
        euler.y = atan2f(matrix._31, matrix._33);
        euler.z = atan2f(matrix._12, matrix._22);
    }
    else {
        euler.y = 0.0f;
        euler.z = atan2f(-matrix._21, matrix._11);
    }
    return euler;
}

DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix) {
    return { matrix._41, matrix._42, matrix._43 };
}

DirectX::XMMATRIX ScaleTranslation(DirectX::XMMATRIX matrix, float scale) {
    matrix.r[3].m128_f32[0] *= scale;
    matrix.r[3].m128_f32[1] *= scale;
    matrix.r[3].m128_f32[2] *= scale;
    return matrix;
}

DirectX::XMFLOAT4X4 ConvertToDirectXMatrix(const aiMatrix4x4& aiMat) {
    DirectX::XMFLOAT4X4 dxMat = DirectX::XMFLOAT4X4();

    dxMat._11 = aiMat.a1; dxMat._12 = aiMat.b1; dxMat._13 = aiMat.c1; dxMat._14 = aiMat.d1;
    dxMat._21 = aiMat.a2; dxMat._22 = aiMat.b2; dxMat._23 = aiMat.c2; dxMat._24 = aiMat.d2;
    dxMat._31 = aiMat.a3; dxMat._32 = aiMat.b3; dxMat._33 = aiMat.c3; dxMat._34 = aiMat.d3;
    dxMat._41 = aiMat.a4; dxMat._42 = aiMat.b4; dxMat._43 = aiMat.c4; dxMat._44 = aiMat.d4;

    return dxMat;
}