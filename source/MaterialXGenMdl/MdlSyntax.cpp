//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#include <MaterialXGenMdl/MdlSyntax.h>

#include <MaterialXGenShader/Library.h>
#include <MaterialXGenShader/TypeDesc.h>

#include <sstream>

namespace MaterialX
{

namespace
{

class MdlArrayTypeSyntax : public ScalarTypeSyntax
{
  public:
    MdlArrayTypeSyntax(const string& name) :
        ScalarTypeSyntax(name, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING)
    {}

    string getValue(const Value& value, bool uniform) const override
    {
        if (!isEmpty(value))
        {
            return "{" + value.getValueString() + "}";
        }
        // OSL disallows arrays without initialization when specified as input uniform
        else if (uniform)
        {
            throw ExceptionShaderGenError("Uniform array cannot initialize to a empty value.");
        }
        return EMPTY_STRING;
    }

    string getValue(const StringVec& values, bool /*uniform*/) const override
    {
        if (values.empty())
        {
            throw ExceptionShaderGenError("No values given to construct an array value");
        }

        string result = "{" + values[0];
        for (size_t i = 1; i<values.size(); ++i)
        {
            result += ", " + values[i];
        }
        result += "}";

        return result;
    }

  protected:
    virtual bool isEmpty(const Value& value) const = 0;
};

class MdlFloatArrayTypeSyntax : public MdlArrayTypeSyntax
{
  public:
    explicit MdlFloatArrayTypeSyntax(const string& name) :
        MdlArrayTypeSyntax(name)
    {}

  protected:
    bool isEmpty(const Value& value) const override
    {
        vector<float> valueArray = value.asA<vector<float>>();
        return valueArray.empty();
    }
};

class MdlIntegerArrayTypeSyntax : public MdlArrayTypeSyntax
{
  public:
    explicit MdlIntegerArrayTypeSyntax(const string& name) :
        MdlArrayTypeSyntax(name)
    {}

  protected:
    bool isEmpty(const Value& value) const override
    {
        vector<int> valueArray = value.asA<vector<int>>();
        return valueArray.empty();
    }
};

// In OSL vector2, vector4, color2 and color4 are custom struct types and require a different
// value syntax for uniforms. So override the aggregate type syntax to support this.
class MdlStructTypeSyntax : public AggregateTypeSyntax
{
  public:
    MdlStructTypeSyntax(const string& name, const string& defaultValue, const string& uniformDefaultValue,
                        const string& typeAlias = EMPTY_STRING, const string& typeDefinition = EMPTY_STRING,
                        const StringVec& members = EMPTY_MEMBERS) :
        AggregateTypeSyntax(name, defaultValue, uniformDefaultValue, typeAlias, typeDefinition, members)
    {}

    string getValue(const Value& value, bool uniform) const override
    {
        if (uniform)
        {
            return "{" + value.getValueString() + "}";
        }
        else
        {
            return getName() + "(" + value.getValueString() + ")";
        }
    }

    string getValue(const StringVec& values, bool uniform) const override
    {
        if (values.empty())
        {
            throw ExceptionShaderGenError("No values given to construct a value");
        }

        string result = uniform ? "{" : getName() + "(" + values[0];
        for (size_t i = 1; i<values.size(); ++i)
        {
            result += ", " + values[i];
        }
        result += uniform ? "}" : ")";

        return result;
    }
};

// For the color4 type we need even more specialization since it's a struct of a struct:
//
// struct color4 {
//    color rgb;
//    float a;
// }
//
class MdlColor4TypeSyntax : public MdlStructTypeSyntax
{
  public:
    MdlColor4TypeSyntax() :
        MdlStructTypeSyntax("color4", "color4(color(0.0), 0.0)", "{color(0.0), 0.0}", EMPTY_STRING, EMPTY_STRING, MdlSyntax::COLOR4_MEMBERS)
    {}

    string getValue(const Value& value, bool uniform) const override
    {
        StringStream ss;

        // Set float format and precision for the stream
        const Value::FloatFormat fmt = Value::getFloatFormat();
        ss.setf(std::ios_base::fmtflags(
            (fmt == Value::FloatFormatFixed ? std::ios_base::fixed :
            (fmt == Value::FloatFormatScientific ? std::ios_base::scientific : 0))),
            std::ios_base::floatfield);
        ss.precision(Value::getFloatPrecision());

        const Color4 c = value.asA<Color4>();

        if (uniform)
        {
            ss << "{color(" << c[0] << ", " << c[1] << ", " << c[2] << "), " << c[3] << "}";
        }
        else
        {
            ss << "color4(color(" << c[0] << ", " << c[1] << ", " << c[2] << "), " << c[3] << ")";
        }

        return ss.str();
    }

    string getValue(const StringVec& values, bool uniform) const override
    {
        if (values.size() < 4)
        {
            throw ExceptionShaderGenError("Too few values given to construct a color4 value");
        }

        if (uniform)
        {
            return "{color(" + values[0] + ", " + values[1] + ", " + values[2] + "), " + values[3] + "}";
        }
        else
        {
            return "color4(color(" + values[0] + ", " + values[1] + ", " + values[2] + "), " + values[3] + ")";
        }
    }
};

class MdlMatrix3TypeSyntax : public AggregateTypeSyntax
{
  public:
      MdlMatrix3TypeSyntax(const string& name, const string& defaultValue, const string& uniformDefaultValue,
                         const string& typeAlias = EMPTY_STRING, const string& typeDefinition = EMPTY_STRING,
                         const StringVec& members = EMPTY_MEMBERS) :
        AggregateTypeSyntax(name, defaultValue, uniformDefaultValue, typeAlias, typeDefinition, members)
    {}

    string getValue(const Value& value, bool uniform) const
    {
        ScopedFloatFormatting fmt(Value::FloatFormatFixed, 3);
        StringVec values = splitString(value.getValueString(), ",");
        return getValue(values, uniform);
    }

    string getValue(const StringVec& values, bool /*uniform*/) const
    {
        if (values.empty())
        {
            throw ExceptionShaderGenError("No values given to construct a value");
        }

        // Write the value using a stream to maintain any float formatting set
        // using Value::setFloatFormat() and Value::setFloatPrecision()
        StringStream ss;
        ss << getName() << "(";
        for (size_t i = 0; i<values.size(); i++)
        {
            ss << values[i] << ", ";
            if ((i+1) % 3 == 0)
            {
                ss << "0.000" << ", ";
            }
        }
        static string ROW_4("0.000, 0.000, 0.000, 1.000");
        ss << ROW_4 << ")";

        return ss.str();
    }
};

} // anonymous namespace

const StringVec MdlSyntax::VECTOR_MEMBERS  = { "[0]", "[1]", "[2]" };
const StringVec MdlSyntax::VECTOR2_MEMBERS = { ".x", ".y" };
const StringVec MdlSyntax::VECTOR4_MEMBERS = { ".x", ".y", ".z", ".w" };
const StringVec MdlSyntax::COLOR2_MEMBERS  = { ".r", ".a" };
const StringVec MdlSyntax::COLOR4_MEMBERS  = { ".rgb[0]", ".rgb[1]", ".rgb[2]", ".a" };

//
// MdlSyntax methods
//

MdlSyntax::MdlSyntax()
{
    // Add in all reserved words and keywords in MDL
    registerReservedWords(
    {
        // Reserved words
        "annotation", "bool", "bool2", "bool3", "bool4", "break", "bsdf", "bsdf_measurement", "case", "cast", "color", "const",
        "continue", "default", "do", "double", "double2", "double2x2", "double2x3", "double3", "double3x2", "double3x3", "double3x4",
        "double4", "double4x3", "double4x4", "double4x2", "double2x4", "edf", "else", "enum", "export", "false", "float", "float2",
        "float2x2", "float2x3", "float3", "float3x2", "float3x3", "float3x4", "float4", "float4x3", "float4x4", "float4x2", "float2x4",
        "for", "hair_bsdf", "if", "import", "in", "int", "int2", "int3", "int4", "intensity_mode", "intensity_power", "intensity_radiant_exitance",
        "let", "light_profile", "material", "material_emission", "material_geometry", "material_surface", "material_volume", "mdl", "module",
        "package", "return", "string", "struct", "switch", "texture_2d", "texture_3d", "texture_cube", "texture_ptex", "true", "typedef", "uniform",
        "using", "varying", "vdf", "while", 
        // Reserved for future use
        "auto", "catch", "char", "class", "const_cast", "delete", "dynamic_cast", "explicit", "extern", "external", "foreach", "friend", "goto",
        "graph", "half", "half2", "half2x2", "half2x3", "half3", "half3x2", "half3x3", "half3x4", "half4", "half4x3", "half4x4", "half4x2", "half2x4",
        "inline", "inout", "lambda", "long", "mutable", "namespace", "native", "new", "operator", "out", "phenomenon", "private", "protected", "public",
        "reinterpret_cast", "sampler", "shader", "short", "signed", "sizeof", "static", "static_cast", "technique", "template", "this", "throw", "try",
        "typeid", "typename", "union", "unsigned", "virtual", "void", "volatile", "wchar_t"
    });

    //
    // Register type syntax handlers for each data type.
    //

    registerTypeSyntax
    (
        Type::FLOAT,
        std::make_shared<ScalarTypeSyntax>(
            "float",
            "0.0",
            "0.0")
    );

    registerTypeSyntax
    (
        Type::FLOATARRAY,
        std::make_shared<MdlFloatArrayTypeSyntax>(
            "float")
    );

    registerTypeSyntax
    (
        Type::INTEGER,
        std::make_shared<ScalarTypeSyntax>(
            "int",
            "0",
            "0")
    );

    registerTypeSyntax
    (
        Type::INTEGERARRAY,
        std::make_shared<MdlIntegerArrayTypeSyntax>(
            "int")
    );

    registerTypeSyntax
    (
        Type::BOOLEAN,
        std::make_shared<ScalarTypeSyntax>(
            "int",
            "0",
            "0",
            EMPTY_STRING,
            "#define true 1\n#define false 0")
    );

    registerTypeSyntax
    (
        Type::COLOR2,
        std::make_shared<MdlStructTypeSyntax>(
            "color2",
            "color2(0.0, 0.0)",
            "{0.0, 0.0}",
            EMPTY_STRING,
            EMPTY_STRING,
            COLOR2_MEMBERS)
    );

    registerTypeSyntax
    (
        // Note: the color type in OSL is a built in type and
        // should not use the custom MdlStructTypeSyntax.
        Type::COLOR3,
        std::make_shared<AggregateTypeSyntax>(
            "color",
            "color(0.0)",
            "color(0.0)",
            EMPTY_STRING,
            EMPTY_STRING,
            VECTOR_MEMBERS)
    );

    registerTypeSyntax
    (
        Type::COLOR4,
        std::make_shared<MdlColor4TypeSyntax>()
    );

    registerTypeSyntax
    (
        Type::VECTOR2,
        std::make_shared<MdlStructTypeSyntax>(
            "vector2",
            "vector2(0.0, 0.0)",
            "{0.0, 0.0}",
            EMPTY_STRING,
            EMPTY_STRING,
            VECTOR2_MEMBERS)
    );

    registerTypeSyntax
    (
        // Note: the vector type in OSL is a built in type and
        // should not use the custom MdlStructTypeSyntax.
        Type::VECTOR3,
        std::make_shared<AggregateTypeSyntax>(
            "vector",
            "vector(0.0)",
            "vector(0.0)",
            EMPTY_STRING,
            EMPTY_STRING,
            VECTOR_MEMBERS)
    );

    registerTypeSyntax
    (
        Type::VECTOR4,
        std::make_shared<MdlStructTypeSyntax>(
            "vector4",
            "vector4(0.0, 0.0, 0.0, 0.0)",
            "{0.0, 0.0, 0.0, 0.0}",
            EMPTY_STRING,
            EMPTY_STRING,
            VECTOR4_MEMBERS)
    );

    registerTypeSyntax
    (
        Type::MATRIX33,
        std::make_shared<MdlMatrix3TypeSyntax>(
            "matrix",
            "matrix(1.0)",
            "matrix(1.0)")
    );

    registerTypeSyntax
    (
        Type::MATRIX44,
        std::make_shared<AggregateTypeSyntax>(
            "matrix",
            "matrix(1.0)",
            "matrix(1.0)")
    );

    registerTypeSyntax
    (
        Type::STRING,
        std::make_shared<StringTypeSyntax>(
            "string",
            "\"\"",
            "\"\"")
    );

    registerTypeSyntax
    (
        Type::FILENAME,
        std::make_shared<StringTypeSyntax>(
            "string",
            "\"\"",
            "\"\"")
    );

    registerTypeSyntax
    (
        Type::BSDF,
        std::make_shared<ScalarTypeSyntax>(
            "BSDF",
            "null_closure",
            "0",
            "closure color")
    );

    registerTypeSyntax
    (
        Type::EDF,
        std::make_shared<ScalarTypeSyntax>(
            "EDF",
            "null_closure",
            "0",
            "closure color")
    );

    registerTypeSyntax
    (
        Type::VDF,
        std::make_shared<ScalarTypeSyntax>(
            "VDF",
            "null_closure",
            "0",
            "closure color")
    );

    registerTypeSyntax
    (
        Type::SURFACESHADER,
        std::make_shared<ScalarTypeSyntax>(
            "surfaceshader",
            "null_closure",
            "0",
            "closure color")
    );

    registerTypeSyntax
    (
        Type::VOLUMESHADER,
        std::make_shared<ScalarTypeSyntax>(
            "volumeshader",
            "null_closure",
            "0",
            "closure color")
    );

    registerTypeSyntax
    (
        Type::DISPLACEMENTSHADER,
        std::make_shared<MdlStructTypeSyntax>(
            "displacementshader",
            "{vector(0.0), 0.0}",
            "{vector(0.0), 0.0}",
            EMPTY_STRING,
            "struct displacementshader { vector offset; float scale; };")
    );

    registerTypeSyntax
    (
        Type::LIGHTSHADER,
        std::make_shared<ScalarTypeSyntax>(
            "lightshader",
            "null_closure",
            "0",
            "closure color")
    );
}

} // namespace MaterialX