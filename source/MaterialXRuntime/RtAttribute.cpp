//
// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#include <MaterialXRuntime/RtAttribute.h>

#include <MaterialXRuntime/Private/PvtAttribute.h>
#include <MaterialXRuntime/Private/PvtPrim.h>

namespace MaterialX
{

RT_DEFINE_RUNTIME_OBJECT(RtAttribute, RtObjType::ATTRIBUTE, "RtAttribute")

RtAttribute::RtAttribute(PvtDataHandle hnd) :
    RtObject(hnd)
{
}

const RtToken& RtAttribute::getType() const
{
    return hnd()->asA<PvtAttribute>()->getType();
}

const RtValue& RtAttribute::getValue() const
{
    return hnd()->asA<PvtAttribute>()->getValue();
}

RtValue& RtAttribute::getValue()
{
    return hnd()->asA<PvtAttribute>()->getValue();
}

void RtAttribute::setValue(const RtValue& v)
{
    return hnd()->asA<PvtAttribute>()->setValue(v);
}

string RtAttribute::getValueString() const
{
    return hnd()->asA<PvtAttribute>()->getValueString();
}

void RtAttribute::setValueString(const string& v)
{
    hnd()->asA<PvtAttribute>()->setValueString(v);
}

const RtToken& RtAttribute::getColorSpace() const
{
    return hnd()->asA<PvtAttribute>()->getColorSpace();
}

void RtAttribute::setColorSpace(const RtToken& colorspace)
{
    return hnd()->asA<PvtAttribute>()->setColorSpace(colorspace);
}

const RtToken& RtAttribute::getUnit() const
{
    return hnd()->asA<PvtAttribute>()->getUnit();
}

void RtAttribute::setUnit(const RtToken& unit)
{
    return hnd()->asA<PvtAttribute>()->setUnit(unit);
}


RT_DEFINE_RUNTIME_OBJECT(RtInput, RtObjType::INPUT, "RtInput")

RtInput::RtInput(PvtDataHandle hnd) :
    RtAttribute(hnd)
{
}

bool RtInput::isUniform() const
{
    return hnd()->asA<PvtInput>()->isUniform();
}

bool RtInput::isConnected() const
{
    return hnd()->asA<PvtInput>()->isConnected();
}

bool RtInput::isSocket() const
{
    return hnd()->asA<PvtInput>()->isSocket();
}

bool RtInput::isConnectable(const RtOutput& output) const
{
    return output.hnd()->asA<PvtOutput>()->isConnectable(hnd()->asA<PvtInput>());
}

void RtInput::connect(const RtOutput& output)
{
    output.hnd()->asA<PvtOutput>()->connect(hnd()->asA<PvtInput>());
}

void RtInput::disconnect(const RtOutput& output)
{
    output.hnd()->asA<PvtOutput>()->disconnect(hnd()->asA<PvtInput>());
}

void RtInput::clearConnection()
{
    return hnd()->asA<PvtInput>()->clearConnection();
}

RtOutput RtInput::getConnection() const
{
    PvtOutput* output = hnd()->asA<PvtInput>()->getConnection();
    return output ? RtOutput(output->hnd()) : RtOutput();
}


RT_DEFINE_RUNTIME_OBJECT(RtOutput, RtObjType::OUTPUT, "RtOutput")

RtOutput::RtOutput(PvtDataHandle hnd) :
    RtAttribute(hnd)
{
}

bool RtOutput::isConnected() const
{
    return hnd()->asA<PvtOutput>()->isConnected();
}

bool RtOutput::isSocket() const
{
    return hnd()->asA<PvtOutput>()->isSocket();
}

bool RtOutput::isConnectable(const RtInput& input) const
{
    return hnd()->asA<PvtOutput>()->isConnectable(input.hnd()->asA<PvtInput>());
}

void RtOutput::connect(const RtInput& input)
{
    return hnd()->asA<PvtOutput>()->connect(input.hnd()->asA<PvtInput>());
}

void RtOutput::disconnect(const RtInput& input)
{
    return hnd()->asA<PvtOutput>()->disconnect(input.hnd()->asA<PvtInput>());
}

void RtOutput::clearConnections()
{
    return hnd()->asA<PvtOutput>()->clearConnections();
}

RtConnectionIterator RtOutput::getConnections() const
{
    return hnd()->asA<PvtOutput>()->getConnections();
}

}
