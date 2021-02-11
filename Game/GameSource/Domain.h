#pragma once
#include <memory>
#include <map>
#include <vector>
#include <minmax.h>

#include "HTNAllHeader.h"
#include "TypeToRegister.h"
#include "DomainConverter.h"

template<class TWorldState, class TChara>
class Domain
{
public:
	Domain() = default;
	~Domain() = default;
	
	void CompleteCompoundTask(const CompoundTaskType& type);
	void AllSet(DomainConverter& converter);
	void Release();

	std::map < PrimitiveTaskType,
		std::shared_ptr<PrimitiveTask<TWorldState, TChara>>>& GetPrimitiveTasks() { return m_primitiveTasks; }
	std::map<CompoundTaskType,
		std::shared_ptr<CompoundTask<TWorldState, TChara>>>& GetCompoundTasks() { return m_compoundTasks; }
	std::map<MethodType,
		std::shared_ptr<Method<TWorldState, TChara>>>& GetMethods() { return m_methods; }
	std::map<PreconditionType,
		std::shared_ptr<PreconditionBase<TWorldState>>>& GetPreconditions() { return m_preconditions; }

	std::shared_ptr<PrimitiveTask<TWorldState, TChara>> GetPrimitiveTask(const PrimitiveTaskType& type);
	std::shared_ptr<CompoundTask<TWorldState, TChara>>	GetCompoundTask(const CompoundTaskType& type);
	std::shared_ptr<Method<TWorldState, TChara>>		GetMethod(const MethodType& type);
	std::shared_ptr<PreconditionBase<TWorldState>>		GetPrecondition(const PreconditionType& type);

	void SetPrimitiveTask(const PrimitiveTaskType type);
	void SetCompoundTask(const CompoundTaskType type);
	void SetMethod(const MethodType type);
	void SetPrecondition(const PreconditionType type);

private:
	void CompleteAttackTask();
	void CompletePrepareAttackTask();
	void CompleteAvoidTask();
private:
	std::map < PrimitiveTaskType,
		std::shared_ptr<PrimitiveTask<TWorldState, TChara>>> m_primitiveTasks;
	std::map<CompoundTaskType,
		std::shared_ptr<CompoundTask<TWorldState, TChara>>> m_compoundTasks;
	std::map<MethodType,
		std::shared_ptr<Method<TWorldState, TChara>>> m_methods;
	std::map<PreconditionType,
		std::shared_ptr<PreconditionBase<TWorldState>>> m_preconditions;
};

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::CompleteCompoundTask(const CompoundTaskType& type)
{
	switch (type)
	{
	case CompoundTaskType::Attack:
		CompleteAttackTask();
		break;
	case CompoundTaskType::PrepareAttack:
		CompletePrepareAttackTask();
		break;
	case CompoundTaskType::Avoid:
		CompleteAvoidTask();
	}
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::AllSet(DomainConverter& converter)
{
	auto primitiveTaskType = converter.GetPrimitiveTaskType();
	auto compoundTaskType = converter.GetCompoundTaskType();
	auto methodType = converter.GetMethodType();
	auto preconditionType = converter.GetPreconditionType();

	int ptCount = static_cast<int>(primitiveTaskType.size());
	int ctCount = static_cast<int>(compoundTaskType.size());
	int meCount = static_cast<int>(methodType.size());
	int prCount = static_cast<int>(preconditionType.size());
	int count = std::max({ ptCount, ctCount, meCount, prCount });

	for (int i = 0; i < count; ++i)
	{
		if (ptCount > i)
			SetPrimitiveTask(primitiveTaskType.at(i));
		if (ctCount > i)
			SetCompoundTask(compoundTaskType.at(i));
		if (meCount > i)
			SetMethod(methodType.at(i));
		if (prCount > i)
			SetPrecondition(preconditionType.at(i));
	}

	CompleteCompoundTask(CompoundTaskType::Attack);
	CompleteCompoundTask(CompoundTaskType::PrepareAttack);
	CompleteCompoundTask(CompoundTaskType::Avoid);
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::Release()
{
	int ptCount = static_cast<int>(PrimitiveTaskType::PTaskEnd);
	int ctCount = static_cast<int>(CompoundTaskType::CTaskEnd);
	int meCount = static_cast<int>(MethodType::MethodEnd);
	int prCount = static_cast<int>(PreconditionType::PreconditionEnd);
	int count = std::max({ ptCount, ctCount, meCount, prCount });

	for (int i = 0; i < count; ++i)
	{
		if (ptCount > i)
			m_primitiveTasks.find(static_cast<PrimitiveTaskType>(i))->second.reset();
		if (prCount > i)
			m_preconditions.find(static_cast<PreconditionType>(i))->second.reset();
		if (meCount > i)
			m_methods.find(static_cast<MethodType>(i))->second.reset();
		if (ctCount > i)
			m_compoundTasks.find(static_cast<CompoundTaskType>(i))->second.reset();


	}
	m_primitiveTasks.clear();
	m_compoundTasks.clear();
	m_methods.clear();
	m_preconditions.clear();
}

template<class TWorldState, class TChara>
inline std::shared_ptr<PrimitiveTask<TWorldState, TChara>> Domain<TWorldState, TChara>::GetPrimitiveTask(const PrimitiveTaskType& type)
{
	auto it = m_primitiveTasks.find(type);
	if (it != m_primitiveTasks.end())
		return it->second;

	return nullptr;
}

template<class TWorldState, class TChara>
inline std::shared_ptr<CompoundTask<TWorldState, TChara>> Domain<TWorldState, TChara>::GetCompoundTask(const CompoundTaskType& type)
{
	auto it =  m_compoundTasks.find(type);
	if (it != m_compoundTasks.end())
		return it->second;

	return nullptr;
}

template<class TWorldState, class TChara>
inline std::shared_ptr<Method<TWorldState, TChara>> Domain<TWorldState, TChara>::GetMethod(const MethodType& type)
{
	auto it = m_methods.find(type);
	if (it != m_methods.end())
		return it->second;

	return nullptr;
}

template<class TWorldState, class TChara>
inline std::shared_ptr<PreconditionBase<TWorldState>> Domain<TWorldState, TChara>::GetPrecondition(const PreconditionType& type)
{
	auto it = m_preconditions.find(type);
	if (it != m_preconditions.end())
		return it->second;

	return nullptr;
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::SetPrimitiveTask(const PrimitiveTaskType type)
{
	std::shared_ptr<PrimitiveTask<TWorldState, TChara>> task;
	switch (type)
	{
	case PrimitiveTaskType::FindAttackPoint:
		task = std::make_shared<FindAPTask<TWorldState, TChara>>();
		break;
	case PrimitiveTaskType::Move:
		task = std::make_shared<MoveTask<TWorldState, TChara>>();
		break;
	case PrimitiveTaskType::SetArrow:
		task = std::make_shared<SetArrowTask<TWorldState, TChara>>();
		break;
	case PrimitiveTaskType::ShootArrow:
		task = std::make_shared<ShootArrowTask<TWorldState, TChara>>();
		break;
	case PrimitiveTaskType::Avoiding:
		task = std::make_shared<AvoidingTask<TWorldState, TChara>>();
		break;
	case PrimitiveTaskType::FindDirectionAvoid:
		task = std::make_shared<FindDATask<TWorldState, TChara>>();
		break;
	}
	m_primitiveTasks.insert(std::make_pair(type, task));
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::SetCompoundTask(const CompoundTaskType type)
{
	std::shared_ptr<CompoundTask<TWorldState, TChara>> task;
	switch (type)
	{
	case CompoundTaskType::Attack:
		task = std::make_shared<AttackTask<TWorldState, TChara>>();
		break;
	case CompoundTaskType::PrepareAttack:
		task = std::make_shared<PrepareAttackTask<TWorldState, TChara>>();
		break;
	case CompoundTaskType::Avoid:
		task = std::make_shared<AvoidTask<TWorldState, TChara>>();
		break;
	}
	m_compoundTasks.insert(std::make_pair(type, task));
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::SetMethod(const MethodType type)
{
	std::shared_ptr<Method<TWorldState, TChara>> method;
	switch (type)
	{
	case MethodType::AtkMethod:
		method = std::make_shared<AttackMethod<TWorldState, TChara>>();
		break;
	case MethodType::FindAPMethod:
		method = std::make_shared<FindAttackPointMethod<TWorldState, TChara>>();
		break;
	case MethodType::PrepareAtkMethod:
		method = std::make_shared<PrepareAttackMethod<TWorldState, TChara>>();
		break;
	case MethodType::AvoidMethod:
		method = std::make_shared<AvoidMethod<TWorldState, TChara>>();
		break;
	case MethodType::FindDAMethod:
		method = std::make_shared<FindDAMethod<TWorldState, TChara>>();
		break;
	}
	m_methods.insert(std::make_pair(type, method));

}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::SetPrecondition(const PreconditionType type)
{
	std::shared_ptr<PreconditionBase<TWorldState>> precondition;
	switch (type)
	{
	case PreconditionType::AtkPrecondition:
		precondition = std::make_shared<AttackPrecondition<TWorldState>>();
		break;
	case PreconditionType::PrepareAtkPrecondition:
		precondition = std::make_shared<PrepareAttackPrecondition<TWorldState>>();
		break;
	case PreconditionType::TruePrecondition:
		precondition = std::make_shared<PreconditionBase<TWorldState>>();
		break;
	case PreconditionType::AvoidPrecondition:
		precondition = std::make_shared<AvoidPrecondition<TWorldState>>();
		break;
	case PreconditionType::FindDAPrecondition:
		precondition = std::make_shared<FindDAPrecondition<TWorldState>>();
		break;
	}
	m_preconditions.insert(std::make_pair(type, precondition));
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::CompleteAttackTask()
{
	auto atkTask = GetCompoundTask(CompoundTaskType::Attack);
	
	auto setArrowTask = GetPrimitiveTask(PrimitiveTaskType::SetArrow);
	auto shootArrowTask = GetPrimitiveTask(PrimitiveTaskType::ShootArrow);
	auto atkPrecondition = GetPrecondition(PreconditionType::AtkPrecondition);
	auto atkMethod = GetMethod(MethodType::AtkMethod);

	atkMethod->AddSubTask(GetPrimitiveTask(PrimitiveTaskType::SetArrow));
	atkMethod->AddSubTask(shootArrowTask);
	atkMethod->AddPrecondition(atkPrecondition);
	atkTask->AddMethod(atkMethod);

	auto prepareAttackTask = GetCompoundTask(CompoundTaskType::PrepareAttack);
	auto prepareAtkMethod = GetMethod(MethodType::PrepareAtkMethod);
	auto prepareAtkPrecondition = GetPrecondition(PreconditionType::PrepareAtkPrecondition);

	prepareAtkMethod->AddSubTask(prepareAttackTask);
	prepareAtkMethod->AddSubTask(atkTask);
	prepareAtkMethod->AddPrecondition(prepareAtkPrecondition);
	atkTask->AddMethod(prepareAtkMethod);
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::CompletePrepareAttackTask()
{
	auto prepareAtkTask = GetCompoundTask(CompoundTaskType::PrepareAttack);

	auto findAttackPointTask = GetPrimitiveTask(PrimitiveTaskType::FindAttackPoint);
	auto moveTask = GetPrimitiveTask(PrimitiveTaskType::Move);
	auto truePrecondition = GetPrecondition(PreconditionType::TruePrecondition);
	auto findAPMethod = GetMethod(MethodType::FindAPMethod);

	findAPMethod->AddSubTask(findAttackPointTask);
	findAPMethod->AddSubTask(moveTask);
	findAPMethod->AddPrecondition(truePrecondition);
	
	prepareAtkTask->AddMethod(findAPMethod);
}

template<class TWorldState, class TChara>
inline void Domain<TWorldState, TChara>::CompleteAvoidTask()
{
	auto avoidTask = GetCompoundTask(CompoundTaskType::Avoid);
	
	auto avoidingTask = GetPrimitiveTask(PrimitiveTaskType::Avoiding);
	auto avoidPrecondition = GetPrecondition(PreconditionType::AvoidPrecondition);
	auto avoidMethod = GetMethod(MethodType::AvoidMethod);
	avoidMethod->AddSubTask(avoidingTask);
	avoidMethod->AddPrecondition(avoidPrecondition);

	auto findDirectionAvoidTask = GetPrimitiveTask(PrimitiveTaskType::FindDirectionAvoid);
	auto findDAPrecondition = GetPrecondition(PreconditionType::FindDAPrecondition);
	auto findDAMethod = GetMethod(MethodType::FindDAMethod);
	findDAMethod->AddSubTask(findDirectionAvoidTask);
	findDAMethod->AddSubTask(avoidTask);
	findDAMethod->AddPrecondition(findDAPrecondition);

	avoidTask->AddMethod(avoidMethod);
	avoidTask->AddMethod(findDAMethod);

}

