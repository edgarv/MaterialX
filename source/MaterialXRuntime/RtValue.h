//
// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_RTVALUE_H
#define MATERIALX_RTVALUE_H

/// @file
/// TODO: Docs

#include <MaterialXRuntime/Library.h>
#include <MaterialXRuntime/RtToken.h>

#include <MaterialXCore/Types.h>

#ifndef _WIN32
// Disable warnings for breaking strict aliasing.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

namespace MaterialX
{

/// @class RtValue
/// Generic value class for storing values of all the data types
/// supported by the API. Values that fit into 16 bytes of data
/// are stored directly. Values larger than 16 bytes are stored
/// as pointers and lifetime managed by the prim they belong to.
class RtValue
{
public:
    /// Default constructor
    RtValue() : _data{0,0} {}

    /// Explicit value constructor
    explicit RtValue(bool v) { asBool() = v; }
    explicit RtValue(int v) { asInt() = v; }
    explicit RtValue(float v) { asFloat() = v; }
    explicit RtValue(const Color2& v) { asColor2() = v; }
    explicit RtValue(const Color3& v) { asColor3() = v; }
    explicit RtValue(const Color4& v) { asColor4() = v; }
    explicit RtValue(const Vector2& v) { asVector2() = v; }
    explicit RtValue(const Vector3& v) { asVector3() = v; }
    explicit RtValue(const Vector4& v) { asVector4() = v; }
    explicit RtValue(const RtToken& v) { asToken() = v; }
    explicit RtValue(void* v) { asPtr() = v; }

    /// Explicit value constructor for large values.
    /// Allocated data is managed by the given prim.
    explicit RtValue(const Matrix33& v, RtObject& prim);
    explicit RtValue(const Matrix44& v, RtObject& prim);
    explicit RtValue(const string& v, RtObject& prim);

    /// Return bool value.
    const bool& asBool() const
    {
        return *reinterpret_cast<const bool*>(&_data);
    }
    /// Return reference to bool value.
    bool& asBool()
    {
        return *reinterpret_cast<bool*>(&_data);
    }

    /// Return int value.
    int asInt() const
    {
        return *reinterpret_cast<const int*>(&_data);
    }
    /// Return reference to int value.
    int& asInt()
    {
        return *reinterpret_cast<int*>(&_data);
    }

    /// Return float value.
    float asFloat() const
    {
        return *reinterpret_cast<const float*>(&_data);
    }
    /// Return reference to float value.
    float& asFloat()
    {
        return *reinterpret_cast<float*>(&_data);
    }

    /// Return Color2 value.
    const Color2& asColor2() const
    {
        return *reinterpret_cast<const Color2*>(&_data);
    }
    /// Return reference to Color2 value.
    Color2& asColor2()
    {
        return *reinterpret_cast<Color2*>(&_data);
    }

    /// Return Color3 value.
    const Color3& asColor3() const
    {
        return *reinterpret_cast<const Color3*>(&_data);
    }
    /// Return reference to Color3 value.
    Color3& asColor3()
    {
        return *reinterpret_cast<Color3*>(&_data);
    }

    /// Return Color4 value.
    const Color4& asColor4() const
    {
        return *reinterpret_cast<const Color4*>(&_data);
    }
    /// Return reference to Color4 value.
    Color4& asColor4()
    {
        return *reinterpret_cast<Color4*>(&_data);
    }

    /// Return Vector2 value.
    const Vector2& asVector2() const
    {
        return *reinterpret_cast<const Vector2*>(&_data);
    }
    /// Return reference to Vector2 value.
    Vector2& asVector2()
    {
        return *reinterpret_cast<Vector2*>(&_data);
    }

    /// Return Vector3 value.
    const Vector3& asVector3() const
    {
        return *reinterpret_cast<const Vector3*>(&_data);
    }
    /// Return reference to Vector3 value.
    Vector3& asVector3()
    {
        return *reinterpret_cast<Vector3*>(&_data);
    }

    /// Return Vector4 value.
    const Vector4& asVector4() const
    {
        return *reinterpret_cast<const Vector4*>(&_data);
    }
    /// Return reference to Vector4 value.
    Vector4& asVector4()
    {
        return *reinterpret_cast<Vector4*>(&_data);
    }

    /// Return token value.
    const RtToken& asToken() const
    {
        return *reinterpret_cast<const RtToken*>(&_data);
    }
    /// Return reference to token value.
    RtToken& asToken()
    {
        return *reinterpret_cast<RtToken*>(&_data);
    }

    /// Return const pointer.
    void* const& asPtr() const
    {
        return *reinterpret_cast<void* const*>(&_data);
    }
    /// Return reference to pointer.
    void*& asPtr()
    {
        return *reinterpret_cast<void**>(&_data);
    }

    /// Return Matrix33 value.
    const Matrix33& asMatrix33() const
    {
        return **reinterpret_cast<const Matrix33* const*>(&_data);
    }
    /// Return reference to Matrix33 value.
    Matrix33& asMatrix33()
    {
        return **reinterpret_cast<Matrix33**>(&_data);
    }

    /// Return Matrix44 value.
    const Matrix44& asMatrix44() const
    {
        return **reinterpret_cast<Matrix44* const*>(&_data);
    }
    /// Return reference to Matrix44 value.
    Matrix44& asMatrix44()
    {
        return **reinterpret_cast<Matrix44**>(&_data);
    }

    /// Return string value.
    const string& asString() const
    {
        return **reinterpret_cast<string* const*>(&_data);
    }
    /// Return reference to string value.
    string& asString()
    {
        return **reinterpret_cast<string**>(&_data);
    }

    /// Clear the value with zeores.
    void clear()
    {
        _data[0] = _data[1] = 0;
    }

    /// Equality operator.
    /// NOTE: This operator will only compare the raw data stored.
    /// For a semantic compare which considers the actual type being stored and
    /// handles comparison for large types stored as pointers you should instead
    /// use the method RtValue::compare().
    bool operator==(const RtValue& other) const
    {
        return _data[0] == other._data[0] && _data[1] == other._data[1];
    }

    /// Create a new value of given type.
    /// If the type is a large value the given object will take
    /// ownership of allocated data.
    static RtValue createNew(const RtToken& type, RtObject owner);

    /// Copy a value from one instance to another.
    /// Both RtValue instances must be initialized for the given type.
    static void copy(const RtToken& type, const RtValue& src, RtValue& dest);

    /// Test if two values are equal.
    /// Both RtValue instances must be initialized for the given type.
    static bool compare(const RtToken& type, const RtValue& a, const RtValue& b);

    /// Convert an RtValue of given type into a string representation.
    static void toString(const RtToken& type, const RtValue& src, string& dest);

    /// Convert a value from a string representation into an RtValue of the given type.
    /// Destination RtValue must been initialized for the given type.
    static void fromString(const RtToken& type, const string& src, RtValue& dest);

private:
    // 16 bytes of data storage to hold the main data types,
    // up to four component vector/color. Larger data types
    // needs to be allocated through the RtValueStore class.
    // Storage is aligned to 64-bit to hold pointers for
    // heap allocated data types as well as other pointers.
    uint64_t _data[2];
};

class RtTypedValue
{
public:
    RtTypedValue() :
        _type(EMPTY_TOKEN)
    {}

    RtTypedValue(const RtToken& t, const RtValue& v) :
        _type(t),
        _value(v)
    {}

    const RtToken& getType() const
    {
        return _type;
    }

    const RtValue& getValue() const
    {
        return _value;
    }

    RtValue& getValue()
    {
        return _value;
    }

    /// Return a string representation for the value of this attribute.
    string getValueString() const
    {
        string dest;
        RtValue::toString(_type, _value, dest);
        return dest;
    }

    /// Set attribute value from a string representation.
    void setValueString(const string& v)
    {
        RtValue::fromString(_type, v, _value);
    }

private:
    RtToken _type;
    RtValue _value;
};

}

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif

#endif
