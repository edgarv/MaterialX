//
// TM & (c) 2020 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_BLURNODEMDL_H
#define MATERIALX_BLURNODEMDL_H

#include <MaterialXGenShader/Nodes/BlurNode.h>

namespace MaterialX
{

/// Blur node MDL implementation
class BlurNodeMdl : public BlurNode
{
  public:
    static ShaderNodeImplPtr create();

    /// Constructor
    BlurNodeMdl();

  protected:

    /// Output sample array
    void outputSampleArray(const ShaderGenerator& shadergen, ShaderStage& stage,
                           const string& inputTypeString,
                           const string& sampleName, const StringVec& sampleStrings) const override;
};

} // namespace MaterialX

#endif
