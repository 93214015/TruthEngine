#include "pch.h"
#include "LightPoint.h"

namespace TruthEngine
{
    LightPoint::LightPoint(uint32_t _ID, const char* _Name, const float3& _Strength, const float _LightSize, const float3& _Position, const bool _CastShadow, const float _AttenuationConstant, const float _AttenuationLinear, const float _AttenuationQuadrant )
        : ILight(_ID, _Name, TE_LIGHT_TYPE::Point)
        , m_Data(_Strength, _LightSize, _Position, _CastShadow, _AttenuationConstant, _AttenuationLinear, _AttenuationQuadrant)
    {
    }
    void LightPoint::SetStrength(const float3& _Strength) noexcept
    {
        m_Data.Strength = _Strength;

        InvokeEventUpdateLight();
    }
    void LightPoint::SetCastShadow(const bool _castshadow) noexcept
    {
        m_Data.CastShadow = static_cast<uint32_t>(_castshadow);

        InvokeEventUpdateLight();
    }
    void LightPoint::SetPosition(const float3& _Position) noexcept
    {
        m_Data.Position = _Position;

        InvokeEventUpdateLight();
    }
    void LightPoint::SetPosition(const float x, const float y, const float z) noexcept
    {
        m_Data.Position = float3(x, y, z);

        InvokeEventUpdateLight();
    }
    void LightPoint::SetLightSize(float _LightSize) noexcept
    {
        m_Data.LightSize = _LightSize;

        InvokeEventUpdateLight();
    }
    void LightPoint::SetAttenuationConstant(float _AttenuationConstant) noexcept
    {
        m_Data.AttenuationConstant = _AttenuationConstant;

        InvokeEventUpdateLight();
    }
    void LightPoint::SetAttenuationLinear(float _AttenuationLinear) noexcept
    {
        m_Data.AttenuationLinear = _AttenuationLinear;

        InvokeEventUpdateLight();
    }
    void LightPoint::SetAttenuationQuadrant(float _AttenuationQuadrant) noexcept
    {
        m_Data.AttenuationQuadrant = _AttenuationQuadrant;

        InvokeEventUpdateLight();
    }
    const float3& LightPoint::GetPosition() const noexcept
    {
        return m_Data.Position;
    }
    const float3& LightPoint::GetStrength() const noexcept
    {
        return m_Data.Strength;
    }
    bool LightPoint::GetCastShadow() const noexcept
    {
        return static_cast<bool>(m_Data.CastShadow);
    }
    float LightPoint::GetAttuantionConstant() const noexcept
    {
        return m_Data.AttenuationConstant;
    }
    float LightPoint::GetAttenuationLinear() const noexcept
    {
        return m_Data.AttenuationLinear;
    }
    float LightPoint::GetAttenuationQuadrant() const noexcept
    {
        return m_Data.AttenuationQuadrant;
    }
    const PointLightData& LightPoint::GetLightData() const noexcept
    {
        return m_Data;
    }
}