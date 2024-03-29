#include "MiniginPCH.h"
#include "GridComponent.h"

#include <map>
#include "Renderer.h"
#include "GameObject.h"
#include "BodyComponent.h"

namespace dae
{
	GridComponent::GridComponent(int sectionSize, const glm::vec2 &origin, int width, int height, bool snapToGrid)
		: m_SectionSize(sectionSize)
		, m_Origin(origin)
		, m_Width(width)
		, m_Height(height)
		, m_NrOfRows(int(width / sectionSize))
		, m_NrOfColums(int(height / sectionSize))
		, m_SnapToGrid(snapToGrid)
	{
		m_GridPointsVec.reserve(m_NrOfColums * m_NrOfRows);

		glm::vec2 gridPoint{};

		for (int i = 0; i < m_NrOfColums; ++i)
		{
			gridPoint.y = i * m_SectionSize + m_Origin.y;

			for (int j = 0; j < m_NrOfRows; ++j)
			{
				gridPoint.x = j * m_SectionSize + m_Origin.x;
				m_GridPointsVec.push_back(gridPoint);
			}
		}
	}

	GridComponent::~GridComponent()
	{
	}

	void GridComponent::Initialize(const SceneContext &)
	{

	}

	void GridComponent::LateUpdate(const SceneContext &)
	{
		if (m_SnapToGrid)
		{
			auto pos = m_pParent->GetPosition();
			auto closestPoint = GetClosestGridPoint(pos);

			if (closestPoint != glm::vec2() && glm::distance(closestPoint, pos) < .3f)
			{
				m_pParent->SetPosition(closestPoint);
				auto body = m_pParent->GetComponentNoError<BodyComponent>();

				if (body != nullptr)
				{
					body->SetLinearVelocity(0.f, 0.f);
				}
			}
		}
	}

	// https://stackoverflow.com/questions/9034388/c-how-to-make-find-if-not-only-return-the-first-object-for-which-applying-pred
	std::vector<glm::vec2> GridComponent::GetClosestGridPoints(const glm::vec2 &position) const
	{
		std::vector<glm::vec2> matchesVec;

		std::copy_if(m_GridPointsVec.begin(), m_GridPointsVec.end(), std::back_inserter(matchesVec),
			[position, this](const glm::vec2 &gridPoint)
		{
			return glm::abs(gridPoint.x - position.x) < m_SectionSize + 1
				&& glm::abs(gridPoint.y - position.y) < m_SectionSize + 1;
		});

		return matchesVec;
	}

	glm::vec2 GridComponent::GetClosestGridPoint(const glm::vec2 &position) const
	{
		auto points = GetClosestGridPoints(position);

		std::map<float, glm::vec2> map;

		for (const auto& point : points)
		{
			auto dist = glm::distance(point, position);
			if(dist < 0)
				LogFormatC(dae::LogLevel::Debug, "Distance: %f", dist);
			if(dist != 0.f)
				map.insert({ dist,point });
		}

		if (map.empty())
		{
			LogWarningC("Too far away from grid.");
			return glm::vec2();
		}

		return (*map.begin()).second;
	}

	glm::vec2 GridComponent::GetNextGridPoint(const glm::vec2 &position, const glm::vec2 &direction) const
	{
		auto closestPoint = GetClosestGridPoint(position + direction);
		auto projectedPoint = closestPoint + direction * float(m_SectionSize);
		return  IsValidGridPoint(projectedPoint) ? projectedPoint : closestPoint;
	}

	bool GridComponent::IsValidGridPoint(const glm::vec2 &point) const
	{
		return std::find(m_GridPointsVec.begin(), m_GridPointsVec.end(), point) != m_GridPointsVec.end();
	}

	bool GridComponent::IsOnGrid(const glm::vec2 &point) const
	{
		auto points = GetClosestGridPoints(point);
		bool horizontal{ false }, vertical{ false };

		for (const auto& gPoint : points)
		{
			if (point.x == gPoint.x)
			{
				horizontal = true;
			}
			if(point.y == gPoint.y)
			{
				vertical = true;
			}
		}

		return horizontal && vertical;
	}

	std::vector<glm::vec2> GridComponent::GetGrid() const
	{
		return m_GridPointsVec;
	}

	glm::vec2 GridComponent::SnapToGrid(const glm::vec2 &pos) const
	{
		auto point = GetClosestGridPoint(pos);

		auto xDiff = glm::abs(pos.x - point.x);
		auto yDiff = glm::abs(pos.y - point.y);

		if (xDiff < yDiff)
			return glm::vec2(point.x, pos.y);
		else
			return glm::vec2(pos.x, point.y);
	}
}