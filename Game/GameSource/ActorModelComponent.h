#pragma once
#include <memory>
#include ".\LibrarySource\EntityComponentSystem.h"
#include ".\LibrarySource\SkinnedMesh.h"

struct Animation
{
private:
	size_t animationFrame;
	float  animationTick;
	float  animeSpeed;
	int    animationClip;
	int    lastAnimationClip;

public:
	void AnimationUpdate(Source::SkinnedMesh::SkinnedMesh& model,float& elapsedTime);
	void AnimationReset();

	inline const int GetAnimationClip() { return animationClip; }
	inline const size_t GetAnimationFrame() { return animationFrame; }
	inline void SetAnimationClip(int animationClip) { this->animationClip = animationClip; }
};

class ActorModel : public Component
{
private:
	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> m_actModel;
	Animation m_animation;	

public:
	ActorModel() = default;
	~ActorModel() {};

	bool Init() override;
	void Update(float& elapsedTime) override;
	void Render() override;

	inline Animation& GetAnimation() { return m_animation; }
	inline std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& GetActorModel() { return m_actModel; }
	inline void SetActModel(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> actModel)
	{
		m_actModel = actModel; 
	}

};