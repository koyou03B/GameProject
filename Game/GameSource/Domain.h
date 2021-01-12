#pragma once
#include "CompoundTask.h"
#include "Method.h"
#include "PreCondition.h"
#include "Effect.h"

enum class DomainContents
{
	PrimitiveTask,
	CompoundTask,
	Method,
	Precondition,
	Effect
};

template<class WorldState>
class Domain
{
public:
	Domain() = default;
	~Domain() = default;

	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/Domain/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/Domain/")+ name + ".bin").c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/Domain/")+ name + ".bin").c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	inline void ToResize(const int size, DomainContents value)
	{
		switch (value)
		{
		case DomainContents::PrimitiveTask:
			m_primitiveTask.resize(size);
			break;
		case DomainContents::CompoundTask:
			m_compoundTask.resize(size);
			break;
		case DomainContents::Method:
			m_method.resize(size);
			break;
		case DomainContents::Precondition:
			m_precondition.resize(size);
			break;
		case DomainContents::Effect:
			m_effect.resize(size);
			break;
		}
	}

	inline int CommunicateNumber(DomainContents value)
	{
		size_t count = 0;
		switch (value)
		{
		case DomainContents::PrimitiveTask:
			count = m_primitiveTask.size();
			break;
		case DomainContents::CompoundTask:
			count = m_compoundTask.size();
			break;
		case DomainContents::Method:
			count = m_method.size();
			break;
		case DomainContents::Precondition:
			count = m_precondition.size();
			break;
		case DomainContents::Effect:
			count = m_effect.size();
			break;
		default:
			break;
		}

		return static_cast<int>(count);
	}

	inline std::vector<std::shared_ptr<PrimitiveTask<WorldState>>>& GetPrimitiveTasks()
	{
		return m_primitiveTask;
	}

	inline std::vector<std::shared_ptr<CompoundTask<WorldState>>>& GetCompoundTasks()
	{
		return m_compoundTask;
	}

	inline std::vector<std::shared_ptr<Method<WorldState>>>& GetMethod() { return m_method; }
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (0 <= version)
		{
			archive
			(
				m_primitiveTask,
				m_compoundTask,
				m_method,
				m_precondition,
				m_effect
			);
		}
		else
		{
			archive
			(
				m_primitiveTask,
				m_compoundTask,
				m_method,
				m_precondition,
				m_effect
			);
		}
	}

private:
	std::vector<std::shared_ptr<PrimitiveTask<WorldState>>> m_primitiveTask;
	std::vector<std::shared_ptr<CompoundTask<WorldState>>> m_compoundTask;
	std::vector<std::shared_ptr<Method<WorldState>>> m_method;
	std::vector<std::shared_ptr<PreCondition<WorldState>>> m_precondition;
	std::vector<std::shared_ptr<Effect<WorldState>>> m_effect;
};

CEREAL_CLASS_VERSION(Domain<ArcherWorldState>, 0);
CEREAL_CLASS_VERSION(PrimitiveTask<ArcherWorldState>, 0);
CEREAL_CLASS_VERSION(CompoundTask<ArcherWorldState>, 0);
CEREAL_CLASS_VERSION(Method<ArcherWorldState>, 0);
CEREAL_CLASS_VERSION(PreCondition<ArcherWorldState>, 0);
CEREAL_CLASS_VERSION(Effect<ArcherWorldState>, 0);