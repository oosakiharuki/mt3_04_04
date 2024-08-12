#include <Novice.h>
#include "MyMath.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include<cmath>
#include<assert.h>

#include<imgui.h>

const char kWindowTitle[] = "LE2C_07_オオサキ_ハルキ_タイトル";

MyMath* myMath = new MyMath();

Vector3 operator+(const Vector3& v1, const Vector3& v2) { return myMath->Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return myMath->Subtract(v1, v2); }
Vector3 operator*(const Vector3& v1, const Vector3& v2) { return myMath->MultiplyVV(v1, v2); }

Vector3 operator*(float f, const Vector3& v) { return myMath->MultiplyFV(f, v); }
Vector3 operator*(const Vector3& v,float f) { return f * v; }

Vector3 operator+=(Vector3 v1,const Vector3& v2) { return myMath->EndLessAdd(v1,v2); }


Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHandleWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHandleWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHandleWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x,0.0f,-kGridHandleWidth };
		Vector3 end{ x,0.0f,kGridHandleWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);
	}
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHandleWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ -kGridHandleWidth,0.0f,z };
		Vector3 end{ kGridHandleWidth,0.0f,z };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);

	}
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

struct Ball {
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	uint32_t color;
};

struct Plane {
	Vector3 normal;
	float distance;
};

Vector3 MultiplyPlane(float m1, Vector3 m2) {
	Vector3  result{};
	result.x = m1 * m2.x;
	result.y = m1 * m2.y;
	result.z = m1 * m2.z;

	return result;
}

Vector3 Normalize(const Vector3& v) {
	Vector3 result;
	result.x = v.x / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	result.y = v.y / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	result.z = v.z / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	Vector3 center = MultiplyPlane(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = MultiplyPlane(2.0f, perpendiculars[index]);
		Vector3 point = myMath->Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, WHITE);
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[3].x, (int)points[3].y, WHITE);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[2].x, (int)points[2].y, WHITE);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[3].x, (int)points[3].y, WHITE);
}

void DrawSphere(const Ball& ball, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 30;
	const float kLonEvery = float(M_PI) / 8.0f;
	const float kLatEvery = float(M_PI) / 8.0f;

	float pi = float(M_PI);

	Vector3 pointAB[kSubdivision] = {};
	Vector3 pointAC[kSubdivision] = {};

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//緯度 シ－タ

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {

			float lon = lonIndex * kLonEvery;//経度　ファイ

			Vector3 a{ ball.position.x + ball.radius * std::cos(lat) * std::cos(lon), ball.position.y + ball.radius * std::sin(lat),ball.position.z + ball.radius * std::cos(lat) * std::sin(lon) };
			Vector3 b{ ball.position.x + ball.radius * std::cos(lat + lat) * std::cos(lon), ball.position.y + ball.radius * std::sin(lat + lat),ball.position.z + ball.radius * std::cos(lat + lat) * std::sin(lon) };
			Vector3 c{ ball.position.x + ball.radius * std::cos(lat) * std::cos(lon + lon), ball.position.y + ball.radius * std::sin(lat), ball.position.z + ball.radius * std::cos(lat) * std::sin(lon + lon) };


			Vector3 aScreen = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 bScreen = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 cScreen = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);


			if (pointAB[latIndex].x != 0 && pointAB[lonIndex].x != 0) {
				Novice::DrawLine((int)aScreen.x, (int)aScreen.y, (int)pointAB[latIndex].x, (int)pointAB[latIndex].y, color);
				Novice::DrawLine((int)aScreen.x, (int)aScreen.y, (int)pointAC[lonIndex].x, (int)pointAC[lonIndex].y, color);
			}

			pointAB[latIndex] = aScreen;
			pointAC[lonIndex] = aScreen;
		}
	}
}

bool IsCollision(const Sphere& s1, const Plane& p2) {
	float distanceX = (p2.normal.x + s1.center.x);
	float distanceY = (p2.normal.y + s1.center.y);
	float distanceZ = (p2.normal.z + s1.center.z);

	float d = p2.distance * p2.normal.x + p2.distance * p2.normal.y + p2.distance * p2.normal.z;

	if (distanceX + distanceY + distanceZ <= d) {
		return true;
	}
	return false;
}

Vector3 Reflect(const Vector3& input,const Vector3& normal){
	Vector3 result{};

	result.x = input.x - 2 * (input.x * normal.x) * normal.x;
	result.y = input.y - 2 * (input.y * normal.y) * normal.y; 
	result.z = input.z - 2 * (input.z * normal.z) * normal.z;

	return result;
}
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	
	float t = ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z)) / ((v2.x * v2.x) + (v2.y * v2.y) + (v2.z * v2.z));

	Vector3 result{};
	result.x = t * v2.x;
	result.y = t * v2.y;
	result.z = t * v2.z;
	return result;
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Plane plane;
	plane.normal = Normalize({-0.2f,0.9f,-0.3f});
	plane.distance = 0.0f;

	Ball ball{};
	ball.position = { 0.8f,1.2f,0.3f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = WHITE;

	float deltaTime = 1.0f / 60.0f;

	Vector3 cameraPosition = { 0.0f ,0.0f,-20.0f };
	Vector3 cameraTranslate = { 0.0f,-1.0f,-6.49f };
	Vector3 cameraRotate = { -0.16f,0.0f,0.0f };


	Vector3 e = { 0.0f,0.0f,0.0f };



	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 worldMatrix = myMath->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 cameraMatrix = myMath->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = myMath->Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
		Matrix4x4 WorldViewProjectionMatrix = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = myMath->MakeViewportMatrix(0, 0, float(1280.0f), float(720.0f), 0.0f, 1.0f);

		if (keys[DIK_SPACE]) {
			ball.acceleration = { 0.0f,-9.8f,0.0f };
		}
		Transform(Transform(ball.velocity,WorldViewProjectionMatrix), viewportMatrix);
		Transform(Transform(ball.acceleration, WorldViewProjectionMatrix), viewportMatrix);


		//ball.velocity += ball.acceleration * deltaTime;
		//ball.position += ball.velocity * deltaTime;

		ball.velocity.x += ball.acceleration.x * deltaTime;
		ball.position.x += ball.velocity.x * deltaTime;
		ball.velocity.y += ball.acceleration.y * deltaTime;
		ball.position.y += ball.velocity.y * deltaTime;
		ball.velocity.z += ball.acceleration.z * deltaTime;
		ball.position.z += ball.velocity.z * deltaTime;

		if (IsCollision(Sphere{ ball.position,ball.radius }, plane)) {
			Vector3 reflected = Reflect(ball.velocity,plane.normal);
			Vector3 projectToNormal = Project(reflected, plane.normal);
			Vector3 movingDirection = reflected - projectToNormal;
			ball.velocity = projectToNormal * e + movingDirection;

			ball.color = RED;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(WorldViewProjectionMatrix, viewportMatrix);
		DrawPlane(plane, WorldViewProjectionMatrix, viewportMatrix);
		DrawSphere(ball, WorldViewProjectionMatrix, viewportMatrix,ball.color);
		

		ImGui::Begin("window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::End();

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
