#include "ActorComponent.h"
#include ".\LibrarySource\Camera.h"
#include ".\LibrarySource\Input.h"

bool Actor::Init()
{
	m_velocity = {0.0f,0.0f,0.0f};
	m_rotationalSpeed = 6.0f;
	return false;
}

void Actor::Update(float& elapsedTime)
{
#if 0
	if ((XINPUT.StickDeadzoneLX(10000) || XINPUT.StickDeadzoneLY(10000)))
	{
		FLOAT4X4 view = Source::CameraControlle::CameraManager().GetInstance()->GetView();
		view._14 = 0.0f;
		view._24 = 0.0f;
		view._34 = 0.0f;
		view._41 = 0.0f;
		view._42 = 0.0f;
		view._43 = 0.0f;
		view._44 = 1.0f;

		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view));
		VECTOR3F playerVec(-XINPUT.StickVectorL().x, 0.0f, -XINPUT.StickVectorL().y);
		DirectX::XMVECTOR moveVec = DirectX::XMLoadFloat3(&playerVec);
		moveVec = DirectX::XMVector4Transform(moveVec, viewMatrix);
		DirectX::XMStoreFloat3(&m_velocity, moveVec);

		auto& worldTransform = _entity->FindComponent<WorldTransform>();
		VECTOR3F translate = worldTransform.GetTranslate();
		VECTOR4F angle = worldTransform.GetAngle();

		VECTOR4F frontAngleVec(sinf(angle.y), 0.0f, cosf(angle.y),1.0f);
		DirectX::XMVECTOR front = DirectX::XMLoadFloat4(&frontAngleVec);
		DirectX::XMVECTOR cross = DirectX::XMVector3Cross(front, moveVec);

		VECTOR4F crossF;
		DirectX::XMStoreFloat4(&crossF, cross);

		float dot = frontAngleVec.x * m_velocity.x +
			frontAngleVec.y * m_velocity.y + frontAngleVec.z * m_velocity.z;

		float dangle = 1 - dot;

		if (dangle >= DirectX::XMConvertToRadians(4))
		{
			dangle = DirectX::XMConvertToRadians(4);
		}
		DirectX::XMVECTOR vRotationSpeed = {};

		if (1 - ::abs(dot) > DirectX::XMConvertToRadians(4))
		{
			if (crossF.y < 0.0f)
			{
				vRotationSpeed = DirectX::XMQuaternionRotationAxis(cross, dangle);

				//angle.y += dangle;
			}
			else
			{
				vRotationSpeed = DirectX::XMQuaternionRotationAxis(cross, -dangle);

				//angle.y -= dangle;

			}
		}
		else if (dot > 0.0f)
		{
			dangle = DirectX::XMConvertToRadians(4);
			if (crossF.y < 0.0f)
			{
				vRotationSpeed = DirectX::XMQuaternionRotationAxis(cross, dangle);

				//angle.y += dangle;
			}
			else
			{
				vRotationSpeed = DirectX::XMQuaternionRotationAxis(cross, -dangle);

				//angle.y -= dangle;

			}
		}

		DirectX::XMStoreFloat4(&angle, vRotationSpeed);

		m_velocity.x = sinf(angle.y) * 10.0f * elapsedTime;
		m_velocity.y = 0.0f;
		m_velocity.z = cosf(angle.y) * 10.0f * elapsedTime;

		translate.x += m_velocity.x;
		translate.y += m_velocity.y;
		translate.z += m_velocity.z;

		worldTransform.SetTranslate(translate);
		worldTransform.SetAngle(angle);

		worldTransform.WorldUpdate();
	}

#else
	//if ((XINPUT.StickDeadzoneLX(10000) || XINPUT.StickDeadzoneLY(10000)))
	//{
	//	FLOAT4X4 view = Source::CameraControlle::CameraManager().GetInstance()->GetView();
	//	view._14 = 0.0f;
	//	view._24 = 0.0f;
	//	view._34 = 0.0f;
	//	view._41 = 0.0f;
	//	view._42 = 0.0f;
	//	view._43 = 0.0f;
	//	view._44 = 1.0f;
	//	
	//	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view));
	//	
	//	VECTOR3F stickVec(-XINPUT.StickVectorL().x, 0.0f, -XINPUT.StickVectorL().y);
	//	DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

	//	vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
	//	DirectX::XMStoreFloat3(&m_velocity, vStickVex);

	//	auto& worldTransform = _entity->FindComponent<WorldTransform>();
	//	VECTOR3F translate = worldTransform.GetTranslate();

	//	VECTOR4F angle = worldTransform.GetAngle();
	//    VECTOR3F frontVec(sinf(angle.y), 0.0f, cosf(angle.y));

	//	FLOAT4X4 world = worldTransform.GetWorld();
	//	//VECTOR3F frontVec(world._31, world._32, world._33);

	//	DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&frontVec);
	//	DirectX::XMVECTOR vCross = DirectX::XMVector3Cross(vFront, vStickVex);

	//	DirectX::XMMATRIX mQuaternion = DirectX::XMMatrixRotationQuaternion(vCross);
	//	DirectX::XMVECTOR vUp = mQuaternion.r[1];

	//	VECTOR4F crossF;
	//	DirectX::XMStoreFloat4(&crossF, vCross);
	//	float dot = frontVec.x * m_velocity.x +
	//		frontVec.y * m_velocity.y + frontVec.z * m_velocity.z;

	//	float dangle = 1 - dot;

	//	if (dangle >= DirectX::XMConvertToRadians(3))
	//	{
	//		dangle = DirectX::XMConvertToRadians(3);
	//	}
	//	DirectX::XMVECTOR vRotationSpeed = {};

	//	if (1 - ::abs(dot) > DirectX::XMConvertToRadians(3))
	//	{
	//		if (crossF.y < 0.0f)
	//		{
	//			//vRotationSpeed = DirectX::XMQuaternionRotationAxis(vUp, dangle);

	//			angle.y += dangle;
	//		}
	//		else
	//		{
	//			//vRotationSpeed = DirectX::XMQuaternionRotationAxis(vUp, -dangle);

	//			angle.y -= dangle;

	//		}
	//	}
	//	else if (dot > 0.0f)
	//	{
	//		dangle = DirectX::XMConvertToRadians(3);
	//		if (crossF.y < 0.0f)
	//		{
	//			//vRotationSpeed = DirectX::XMQuaternionRotationAxis(vUp, dangle);

	//			angle.y += dangle;
	//		}
	//		else
	//		{
	//			//vRotationSpeed = DirectX::XMQuaternionRotationAxis(vUp, -dangle);

	//			angle.y -= dangle;

	//		}
	//	}

	////	DirectX::XMStoreFloat4(&angle, vRotationSpeed);
	//	worldTransform.SetAngle(angle);

	//	m_velocity.x = world._31 * 10.0f * elapsedTime;
	//	m_velocity.y = 0.0f;
	//	m_velocity.z = world._33 *10.0f * elapsedTime;
	//	translate += m_velocity;

	//	worldTransform.SetTranslate(translate);

	//	worldTransform.WorldUpdate();
	//}

#endif
	//else
	//{

	//}
	//auto& worldTransform = _entity->FindComponent<WorldTransform>();
	//
	//VECTOR4F r = worldTransform.GetAngle();
	//DirectX::XMVECTOR angle = DirectX::XMVectorSet(r.x, r.y, r.z, r.w);
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(angle);

	//DirectX::XMVECTOR forward = R.r[2];
	//DirectX::XMVECTOR up = R.r[1];
	//DirectX::XMVECTOR right = R.r[0];

	//DirectX::XMVECTOR rota = DirectX::XMQuaternionRotationAxis(up, m_rotationalSpeed);

	//DirectX::XMStoreFloat4(&r, rota);
	//worldTransform.SetAngle(r);
	//worldTransform.WorldUpdate();
}
