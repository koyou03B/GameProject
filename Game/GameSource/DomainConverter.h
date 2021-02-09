#pragma once
#include <vector>
#include "TypeToRegister.h"

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>

class DomainConverter
{
public:
	DomainConverter() = default;
	~DomainConverter() = default;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		archive
		(
			m_primitiveTasks,
			m_compoundTasks,
			m_methods,
			m_preconditions
		);
	}

	inline void AddPrimitivetaskType(const PrimitiveTaskType& type)	 { m_primitiveTasks.push_back(type); }
	inline void AddCompoundTaskType(const CompoundTaskType& type)	 { m_compoundTasks.push_back(type); }
	inline void AddMethodType(const MethodType& type)				 { m_methods.push_back(type); }
	inline void AddPreconditionType(const PreconditionType& type)	 { m_preconditions.push_back(type); }

	inline std::vector<PrimitiveTaskType> GetPrimitiveTaskType()	{ return m_primitiveTasks; }
	inline std::vector<CompoundTaskType>  GetCompoundTaskType()		{ return m_compoundTasks; }
	inline std::vector<MethodType>	GetMethodType()					{ return m_methods; }
	inline std::vector<PreconditionType> GetPreconditionType()		{ return m_preconditions; }

private:
	std::vector<PrimitiveTaskType>	m_primitiveTasks;
	std::vector<CompoundTaskType>	m_compoundTasks;
	std::vector<MethodType>			m_methods;
	std::vector<PreconditionType>	m_preconditions;
};