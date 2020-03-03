//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_MDLSYNTAX_H
#define MATERIALX_MDLSYNTAX_H

/// @file
/// OSL syntax class

#include <MaterialXGenShader/Syntax.h>

namespace MaterialX
{

class MdlSyntax;

/// Shared pointer to a MdlSyntax
using MdlSyntaxPtr = shared_ptr<MdlSyntax>;

/// @class MdlSyntax
/// Syntax class for MDL (Material Definition Language)
class MdlSyntax : public Syntax
{
public:
    MdlSyntax();

    static SyntaxPtr create() { return std::make_shared<MdlSyntax>(); }

    const string& getConstantQualifier() const override { return CONST_QUALIFIER; };
    const string& getUniformQualifier() const override { return UNIFORM_QUALIFIER; };

    string getSwizzledVariable(const string& srcName, const TypeDesc* srcType, const string& channels, const TypeDesc* dstType) const override;

    /// Override to return array type suffix.
    string getArrayTypeSuffix(const TypeDesc* type, const Value& value) const override;

    /// Override to indicate array variables have no array suffix.
    string getArrayVariableSuffix(const TypeDesc*, const Value&) const override { return EMPTY_STRING; };

    static const string CONST_QUALIFIER;
    static const string UNIFORM_QUALIFIER;
    static const StringVec VECTOR2_MEMBERS;
    static const StringVec VECTOR3_MEMBERS;
    static const StringVec VECTOR4_MEMBERS;
    static const StringVec COLOR2_MEMBERS;
    static const StringVec ADDRESSMODE_MEMBERS;
    static const StringVec COORDINATESPACE_MEMBERS;
    static const StringVec FILTERLOOKUPMODE_MEMBERS;
    static const StringVec FILTERTYPE_MEMBERS;
    static const StringVec DISTRIBUTIONTYPE_MEMBERS;

    /// Get an type description for an enumeration based on member value
    const TypeDesc* getEnumeratedType(const string& value) const;

    /// Given an input specification attempt to remap this to an enumeration which is accepted by
    /// the shader generator. The enumeration may be converted to a different type than the input.
    bool remapEnumeration(const string& value, const TypeDesc* type, const string& enumNames, std::pair<const TypeDesc*, ValuePtr>& result) const override;
};

namespace Type
{
    extern const TypeDesc* MDL_ADDRESSMODE;
    extern const TypeDesc* MDL_COORDINATESPACE;
    extern const TypeDesc* MDL_FILTERLOOKUPMODE;
    extern const TypeDesc* MDL_FILTERTYPE;
    extern const TypeDesc* MDL_DISTRIBUTIONTYPE;
}

} // namespace MaterialX

#endif
