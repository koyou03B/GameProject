#include "ArchersGamemaker.h"
#include "ArcherCharacter.h"
#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(ArchersGameMaker, 1);

void ArchersGameMaker::Init()
{
	m_domain.AllSet(m_domainConverter);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::FindAttackPoint)->SetOperator(&Archer::FindAttackPoint);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::Move)->SetOperator(&Archer::MoveRun);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::SetArrow)->SetOperator(&Archer::SetArrow);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::ShootArrow)->SetOperator(&Archer::Shoot);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::Avoiding)->SetOperator(&Archer::Avoid);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::FindDirectionAvoid)->SetOperator(&Archer::FindDirectionToAvoid);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::Recover)->SetOperator(&Archer::Heal);

}

void ArchersGameMaker::Release()
{
	m_domain.Release();
}

void ArchersGameMaker::SetRootTask(const CompoundTaskType& type)
{
	auto task = m_domain.GetCompoundTask(type);
	m_planRunner.SetRootTask(task);
}

std::vector<TaskBase<ArcherWorldState, Archer>*> ArchersGameMaker::SearchBehaviorTask(ArcherWorldState& worldState)
{
	return m_planRunner.UpdatePlan(worldState);
}

void ArchersGameMaker::ImGui(Archer* archer)
{
#ifdef _DEBUG
	ImGui::Begin("HTN");
	if (ImGui::CollapsingHeader("HTN"))
	{
		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("PrimitiveTask"))
		{
			static int primitiveTask = 0;
			ImGui::RadioButton("FindAttackPoint", &primitiveTask, 0); ImGui::SameLine();
			ImGui::RadioButton("Move", &primitiveTask, 1);
			ImGui::RadioButton("SetArrow", &primitiveTask, 2); ImGui::SameLine();
			ImGui::RadioButton("ShootArrow", &primitiveTask, 3);
			ImGui::RadioButton("Avoiding", &primitiveTask, 4); ImGui::SameLine();
			ImGui::RadioButton("FindDirectionAvoid", &primitiveTask, 5);
			ImGui::RadioButton("Heal", &primitiveTask, 6);//ImGui::SameLine();
			PrimitiveTaskType type = static_cast<PrimitiveTaskType>(primitiveTask);

			int count = static_cast<int>(m_domain.GetPrimitiveTasks().size());
			ImGui::InputInt("PrimitiveTasks Size", &count);

			if (ImGui::Button("SetPrimitiveTask"))
			{
				m_domain.SetPrimitiveTask(type);
				m_domainConverter.AddPrimitivetaskType(type);
			}

			if (ImGui::Button("Set Operator"))
			{
				switch (type)
				{
				case PrimitiveTaskType::FindAttackPoint:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::FindAttackPoint);
					break;
				case PrimitiveTaskType::Move:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::MoveRun);
					break;
				case PrimitiveTaskType::SetArrow:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::SetArrow);
					break;
				case PrimitiveTaskType::ShootArrow:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::Shoot);
					break;
				case PrimitiveTaskType::Avoiding:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::Avoid);
					break;
				case PrimitiveTaskType::FindDirectionAvoid:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::FindDirectionToAvoid);
					break;
				case PrimitiveTaskType::Recover:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::Heal);
					break;
				}
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("CompoundTask"))
		{
			static int compoundTask = 0;
			ImGui::RadioButton("Attack", &compoundTask, 0); ImGui::SameLine();
			ImGui::RadioButton("PrepareAttack", &compoundTask, 1);
			ImGui::RadioButton("Avoid", &compoundTask, 2); ImGui::SameLine();
			ImGui::RadioButton("Recover", &compoundTask, 3);
			CompoundTaskType type = static_cast<CompoundTaskType>(compoundTask);

			int count = static_cast<int>(m_domain.GetCompoundTasks().size());
			ImGui::InputInt("CompoundTasks Size", &count);

			if (ImGui::Button("SetCompoundTask"))
			{
				m_domain.SetCompoundTask(type);
				m_domainConverter.AddCompoundTaskType(type);
			}
			ImGui::SameLine();

			if (ImGui::Button("Complete Task"))
				m_domain.CompleteCompoundTask(type);

			if (ImGui::Button("Select RootTask"))
			{
				auto task = m_domain.GetCompoundTask(type);
				m_planRunner.SetRootTask(task);
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("Method"))
		{
			static int method = 0;
			ImGui::RadioButton("AtkMethod", &method, 0); ImGui::SameLine();
			ImGui::RadioButton("FindAPMethod", &method, 1);
			ImGui::RadioButton("PrepareAtkMethod", &method, 2); ImGui::SameLine();
			ImGui::RadioButton("AvoidMethod", &method, 3);
			ImGui::RadioButton("FindDAMethod", &method, 4); ImGui::SameLine();
			ImGui::RadioButton("HealMethod", &method, 5);

			int count = static_cast<int>(m_domain.GetMethods().size());
			ImGui::InputInt("Methods Size", &count);

			if (ImGui::Button("SetMethod"))
			{
				MethodType type = static_cast<MethodType>(method);
				m_domain.SetMethod(type);
				m_domainConverter.AddMethodType(type);
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("Precondition"))
		{
			static int precondition = 0;
			ImGui::RadioButton("AtkPrecondition", &precondition, 0); ImGui::SameLine();
			ImGui::RadioButton("PrepareAtkPrecondition", &precondition, 1);
			ImGui::RadioButton("TruePrecondition", &precondition, 2); ImGui::SameLine();
			ImGui::RadioButton("AvoidPrecondition", &precondition, 3);
			ImGui::RadioButton("FindDAPrecondition", &precondition, 4); ImGui::SameLine();
			ImGui::RadioButton("HealPrecondition", &precondition, 5);

			int count = static_cast<int>(m_domain.GetPreconditions().size());
			ImGui::InputInt("Preconditions Size", &count);

			if (ImGui::Button("SetPrecondition"))
			{
				PreconditionType type = static_cast<PreconditionType>(precondition);
				m_domain.SetPrecondition(type);
				m_domainConverter.AddPreconditionType(type);
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("Planner"))
		{
			static bool isExecute = false;
			auto& planList = archer->GetPlanList();
			if (ImGui::Button("Planning"))
				planList = m_planRunner.UpdatePlan((archer->GetWorldState()));
			if (ImGui::Button("Execute"))
				isExecute = !isExecute;
			if (isExecute)
			{
				size_t count = planList.size();
				static size_t currentTask = 0;
				ImGui::Text("CurrentTask %d", static_cast<int>(currentTask));
				for (currentTask; currentTask < count;)
				{
					if (planList.at(currentTask)->ExecuteOperator(archer))
						++currentTask;
					else
						break;

				}
				if (currentTask == count)
				{
					isExecute = false;
					currentTask = 0;
				}
			}

			ImGui::TreePop();
		}

	}
	ImGui::End();
#endif
}
