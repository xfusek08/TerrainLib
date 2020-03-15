#pragma once

#include <memory>

#include <map>

#include <GeoPlanetLib/geoplanetlib_export.h>

#include <GeoPlanetLib/AbstactFactory.h>
#include <GeoPlanetLib/Surface.h>

namespace gp
{
    enum class ModifierVariableType
    {
        Bool, Integer, Float
    };

    typedef union {
        bool boolval;
        int intVal;
        float floatVal;
    } ModifierVariableData;

    struct ModifierVariable
    {
        ModifierVariableType type;
        std::string description;
        ModifierVariableData value;
    };

    class GEOPLANETLIB_EXPORT SurfaceModifier : public AbstractFactoryBase<SurfaceModifier>
    {
    public:
        // properties
        std::map<std::string, ModifierVariable> variables;

        //methods
        SurfaceModifier();

        virtual bool apply(std::shared_ptr<Surface> surface) = 0;

        inline int getIntegerVariable(std::string ident) const { return getVariable(ident).value.intVal; }
        inline bool getBoolVariable(std::string ident)   const { return getVariable(ident).value.boolval; }
        inline float getFloatVariable(std::string ident) const { return getVariable(ident).value.floatVal; }
        ModifierVariable getVariable(std::string ident)  const;

        void addIntegerVariable(std::string ident, std::string description, int value = 0);
        void addBoolVariable(std::string ident, std::string description, bool value = false);
        void addFloatVariable(std::string ident, std::string description, float value = 0.0);
        void addVariable(std::string ident, ModifierVariable variable);

    protected:
        // methods
        virtual void initVariables() {};
    };
}
