#include <MaterialXShaderGen/SgNode.h>
#include <MaterialXShaderGen/ShaderGenerator.h>
#include <MaterialXShaderGen/ShaderGenRegistry.h>
#include <MaterialXShaderGen/NodeImplementation.h>
#include <MaterialXShaderGen/Util.h>

#include <MaterialXCore/Document.h>
#include <MaterialXCore/Value.h>

#include <MaterialXFormat/File.h>

#include <sstream>

namespace MaterialX
{

namespace
{
    void getSourceCode(const Implementation& impl, string& source, bool& inlined)
    {
        const string& file = impl.getAttribute("file");
        if (file.empty())
        {
            throw ExceptionShaderGenError("No source file specified for implementation '" + impl.getName() + "'");
        }

        source = "";
        inlined = (getFileExtension(file) == "inline");

        // Find the function name to use
        string function = impl.getAttribute("function");
        if (function.empty())
        {
            // No function given so use node def name
            function = impl.getNodeDef();
        }

        if (!readFile(ShaderGenRegistry::findSourceCode(file), source))
        {
            throw ExceptionShaderGenError("Can't find source file '" + file + "' used by implementation '" + impl.getName() + "'");
        }

        if (inlined)
        {
            source.erase(std::remove(source.begin(), source.end(), '\n'), source.end());
        }
    }
}


void SgNode::ScopeInfo::adjustAtConditionalInput(const NodePtr& condNode, int branch, const uint32_t fullMask)
{
    if (type == ScopeInfo::Type::GLOBAL || (type == ScopeInfo::Type::SINGLE && conditionBitmask == fullConditionMask))
    {
        type = ScopeInfo::Type::SINGLE;
        conditionalNode = condNode;
        conditionBitmask = 1 << branch;
        fullConditionMask = fullMask;
    }
    else if (type == ScopeInfo::Type::SINGLE)
    {
        type = ScopeInfo::Type::MULTIPLE;
        conditionalNode = nullptr;
    }
}

void SgNode::ScopeInfo::merge(const ScopeInfo &fromScope)
{
    if (type == ScopeInfo::Type::UNKNOWN || fromScope.type == ScopeInfo::Type::GLOBAL)
    {
        *this = fromScope;
    }
    else if (type == ScopeInfo::Type::GLOBAL)
    {

    }
    else if (type == ScopeInfo::Type::SINGLE && fromScope.type == ScopeInfo::Type::SINGLE && conditionalNode == fromScope.conditionalNode)
    {
        conditionBitmask |= fromScope.conditionBitmask;

        // This node is needed for all branches so it is no longer conditional
        if (conditionBitmask == fullConditionMask)
        {
            type = ScopeInfo::Type::GLOBAL;
            conditionalNode = nullptr;
        }
    }
    else
    {
        // NOTE: Right now multiple scopes is not really used, it works exactly as GLOBAL_SCOPE
        type = ScopeInfo::Type::MULTIPLE;
        conditionalNode = nullptr;
    }
}


SgNode::SgNode(NodePtr node, const string& language, const string& target)
    : _node(node)
    , _nodeDef(nullptr)
    , _customImpl(nullptr)
{
    if (!_node)
    {
        return;
    }

    _nodeDef = node->getReferencedNodeDef();
    if (!_nodeDef)
    {
        throw ExceptionShaderGenError("Could not find a nodedef for node '" + node->getName() + "'");
    }

    // Check if this node has a custom node implementation registered
    _customImpl = ShaderGenRegistry::findNodeImplementation(_nodeDef->getNode(), language, target);

    if (!_customImpl)
    {
        ImplementationPtr impl = getSourceCodeImplementation(*_nodeDef, language, target);
        if (!impl)
        {
            throw ExceptionShaderGenError("Could not find an implementation for node '" + _nodeDef->getNode() + "' matching language '" + language + "' and target '" + target + "'");
        }

        getSourceCode(*impl, _functionSource, _inlined);
        _functionName = impl->getFunction();
    }
}

const ValueElement& SgNode::getPort(const string& name) const
{
    ValueElementPtr port = _node->getChildOfType<ValueElement>(name);
    if (!port)
    {
        port = _nodeDef->getChildOfType<ValueElement>(name);
        if (!port)
        {
            throw ExceptionShaderGenError("Node '" + _node->getName() + "' has no input port named '" + name + "'");
        }
    }
    return *port;
}

ImplementationPtr SgNode::getSourceCodeImplementation(const NodeDef& nodeDef, const string& language, const string& target)
{
    vector<ElementPtr> elements = nodeDef.getDocument()->getMatchingImplementations(nodeDef.getName());
    for (ElementPtr element : elements)
    {
        if (!element->isA<Implementation>())
        {
            // Ignore node graph implementations
            continue;
        }

        ImplementationPtr impl = element->asA<Implementation>();

        const string& matchingTarget = impl->getTarget();

        if (impl->getLanguage() == language && (matchingTarget.empty() || matchingTarget == target))
        {
            return impl;
        }
    }

    return nullptr;
}

} // namespace MaterialX
