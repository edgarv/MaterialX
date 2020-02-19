//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef GENMDL_H
#define GENMDL_H

#include <MaterialXTest/Catch/catch.hpp>

#include <MaterialXTest/GenShaderUtil.h>

namespace mx = MaterialX;

class MdlShaderGeneratorTester : public GenShaderUtil::ShaderGeneratorTester
{
  public:
    using ParentClass = GenShaderUtil::ShaderGeneratorTester;

    MdlShaderGeneratorTester(mx::ShaderGeneratorPtr shaderGenerator, const std::vector<mx::FilePath>& testRootPaths,
                             const mx::FilePath& libSearchPath, const mx::FileSearchPath& srcSearchPath,
                             const mx::FilePath& logFilePath, bool writeShadersToDisk) :
        GenShaderUtil::ShaderGeneratorTester(shaderGenerator, testRootPaths, libSearchPath, srcSearchPath, logFilePath, writeShadersToDisk)
    {}

    void setTestStages() override
    {
        _testStages.push_back(mx::Stage::PIXEL);
    }

    // Ignore trying to create shader code for lightshaders
    void addSkipNodeDefs() override
    {
        _skipNodeDefs.insert("ND_point_light");
        _skipNodeDefs.insert("ND_spot_light");
        _skipNodeDefs.insert("ND_directional_light");
        ParentClass::addSkipNodeDefs();
    }

    // Arnold specific files are ignored in vanilla osl target
    void addSkipLibraryFiles() override
    {
        _skipLibraryFiles.insert( "pbrlib_genosl_arnold_impl.mtlx" );
    }
    // Ignore light shaders in the document for MDL
    void findLights(mx::DocumentPtr /*doc*/, std::vector<mx::NodePtr>& lights) override
    {
        lights.clear();
    }

    // No direct lighting to register for MDL
    void registerLights(mx::DocumentPtr /*doc*/, const std::vector<mx::NodePtr>& /*lights*/, mx::GenContext& /*context*/) override
    {
    }

    // Compile MDL with mdlc if specified
    void compileSource(const std::vector<mx::FilePath>& sourceCodePaths) override;

  protected:
    void getImplementationWhiteList(mx::StringSet& whiteList) override
    {
        whiteList =
        {
            "ambientocclusion", "arrayappend", "backfacing", "screen", "curveadjust", "displacementshader",
            "volumeshader", "IM_constant_", "IM_dot_", "IM_geomattrvalue", "IM_angle"
        };
    }
};

#endif // GENOSL_H