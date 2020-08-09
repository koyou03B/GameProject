#include "ActorModelComponent.h"
#include "WorldTransformComponent.h"
#include "AnimationBlendComponent.h"
#include ".\LibrarySource\Framework.h"
bool ActorModel::Init()
{
	m_animation.AnimationReset();
	return false;
}

void ActorModel::Update(float& elapsedTime)
{
	if (m_actModel)
		m_animation.AnimationUpdate(*m_actModel.get(), elapsedTime);
}


void ActorModel::Render()
{
	if (m_actModel)
	{
		FLOAT4X4 world = _entity->FindComponent<WorldTransform>().GetWorld();
		VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };

		auto& transform = _entity->FindComponent<AnimationBlend>().m_blendLocals;
		auto& transformToParent = _entity->FindComponent<AnimationBlend>().m_blendActor;
		m_actModel->Render(Framework::GetContext(), world, color,transform,transformToParent);

//	    m_actModel->Render(Framework::GetContext(), world, color,m_animation.GetAnimationClip(), m_animation.GetAnimationFrame());
	}
}

void Animation::AnimationUpdate(Source::SkinnedMesh::SkinnedMesh& model, float& elapsedTime)
{
	if (animationClip != lastAnimationClip)
	{
		animationFrame = 0;
		animationTick = 0.0f;
	}

	animationFrame = static_cast<size_t>(animationTick * model._resource->_animationTakes.at(animationClip).samplingRate);
	size_t lastOfFrame = model._resource->_animationTakes.at(animationClip).numberOfKeyframes - 1;

	if (animationFrame > lastOfFrame)
	{
		animationFrame = 0;
		animationTick = 0.0f;
	}

	lastAnimationClip = animationClip;
	animationTick += 1.0f * animeSpeed * elapsedTime;

}

void Animation::AnimationReset()
{
	animationFrame = 0;
	animationTick = 0.0f;
	animeSpeed = 1.0f;
	animationClip = 0;
	lastAnimationClip = 0;
}
