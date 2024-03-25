#include "sdk.h"

Vector3 SDK::GetBoneWithRotation(uintptr_t mesh, int id)
{
	int IsCached = read<int>(mesh + 0x5F8);
	auto BoneTransform = read<FTransform>(read<uintptr_t>(mesh + 0x10 * IsCached + 0x5b0) + 0x60 * id);
	
	FTransform ComponentToWorld = read<FTransform>(mesh + 0x1c0);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(BoneTransform.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}
struct CamewaDescwipsion
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
	char Useless[0x18];
};
Camera SDK::GetViewAngles() {
	CamewaDescwipsion camera;
	auto locationPointer = read<uintptr_t>(LocalPtrs::Gworld + 0x110);
	auto rotationPointer = read<uintptr_t>(LocalPtrs::Gworld + 0x120);

	struct Rotation
	{
		double a; //0x0000
		char pad_0008[24]; //0x0008
		double b; //0x0020
		char pad_0028[424]; //0x0028
		double c; //0x01D0
	};
	Rotation rotation;

	rotation = read<Rotation>(rotationPointer);

	camera.Location = read<Vector3>(locationPointer);
	camera.Rotation.x = asin(rotation.c) * (180.0 / M_PI);
	camera.Rotation.y = ((atan2(rotation.a * -1, rotation.b) * (180.0 / M_PI)) * -1) * -1;
	camera.FieldOfView = read<float>(LocalPtrs::PlayerController + 0x394) * 90.f;

	return { camera.Location, camera.Rotation, camera.FieldOfView };
}

Vector2 SDK::ProjectWorldToScreen(Vector3 WorldLocation)
{

	vCamera = SDK::GetViewAngles(); //get ur players newest view angles
	if (Debug::PrintLocations) {
		Util::Print3D("Location: ", vCamera.Location);
		Util::Print3D("Rotation: ", vCamera.Rotation);
		Util::Print2D("Fov: ", Vector2(vCamera.FieldOfView, 69));
	}

	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector2((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z);
}
