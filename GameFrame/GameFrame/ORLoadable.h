#pragma once
#include "ExtJson.h"
#include "Minimal.h"
#include <functional>
#include <imgui.h>

class LoadOrSkip {};

/*
！！如果Success==false那么后续的全部载入都会被打断！！
*/
class ORJsonLoader :public JsonObject
{
public:
    bool Success{ true };
    inline ORJsonLoader& ClearFlag() { Success = true; return *this; }
    template<typename T>
    inline ORJsonLoader& operator()(const std::string_view Item, T& Obj, const std::function<bool(T&)>& DefaultPolicy);
    template<typename T>
    inline ORJsonLoader& operator()(const std::string_view Item, T& Obj, const T& Default);
    template<typename T>
    inline ORJsonLoader& operator()(const std::string_view Item, T& Obj, LoadOrSkip);
    template<typename T>
    inline ORJsonLoader& operator()(const std::string_view Item, T& Obj);
    inline JsonObject PreCalcItem(const std::string_view Item);
    inline operator JsonObject()
    {
        return *(JsonObject*)this;
    }
    ORJsonLoader() = default;
    ORJsonLoader(JsonObject _) :JsonObject(_), Success(true) {}
};

template<typename T>
concept ClassLoadable = requires(ORJsonLoader & Obj, T & V)
{
    { V.Load(Obj) } -> std::same_as<void>;
};
template<typename T>
concept MapLoadable = !(std::same_as<T, int> || std::same_as<T, double> || std::same_as<T, bool> || std::same_as<T, std::string> || std::same_as <T, JsonObject>);
template<typename T>
concept VectorLoadable = !(std::same_as<T, int> || std::same_as<T, double> || std::same_as<T, uint8_t> || std::same_as<T, std::string> || std::same_as <T, JsonObject>);
//template<typename T>
//concept HasValueType = requires() { T::value_type; };
//template<typename T>
//concept HasMappedType = requires {T::mapped_type; };
template<typename T>
concept MapClassLoadable = MapLoadable<typename T::mapped_type> && std::same_as<T, std::unordered_map<std::string, typename T::mapped_type>>;
template<typename T>
concept VectorClassLoadable = VectorLoadable<typename T::value_type> && std::same_as<T, std::vector<typename T::value_type>>;
template<typename T>
concept SPtrClassLoadable = std::same_as<T, std::shared_ptr<typename T::element_type>>;
template<typename T>
concept UPtrClassLoadable = std::same_as<T, std::unique_ptr<typename T::element_type>>;
template<typename T>
concept NotClassLoadable = !(ClassLoadable<T> || MapClassLoadable<T> || VectorClassLoadable<T> || SPtrClassLoadable<T> || UPtrClassLoadable<T>);
/*
Load函数满足Obj.Available()==true保证
*/
template<ClassLoadable T>
void BasicTypeLoad(ORJsonLoader& Obj, T& Val)
{
    Val.Load(Obj);
};

template<NotClassLoadable T>
void BasicTypeLoad(ORJsonLoader& Obj, T& Val)
{
    static_assert(false, "BasicTypeLoad缺少载入的模板！");
};

template<>
void BasicTypeLoad<JsonObject>(ORJsonLoader& Obj, JsonObject& Val)
{
    Val = (JsonObject)Obj;
}

template<>
void BasicTypeLoad<ImVec2>(ORJsonLoader& Obj, ImVec2& Val)
{
    Val.x = Obj.GetFloat();
    Val.y = Obj.GetFloat();
}

template<SPtrClassLoadable T>
void BasicTypeLoad(ORJsonLoader& Obj, T& Val)
{
    if (!Val)Val.reset(new T::element_type);
    BasicTypeLoad(Obj, *Val);
}

template<VectorClassLoadable T>
void BasicTypeLoad(ORJsonLoader& Obj, T& Val)
{
    std::vector<JsonObject> Arr = Obj.GetArrayObject();
    for (auto& p : Arr)
    {
        ORJsonLoader L(p);
        Val.emplace_back();
        BasicTypeLoad<typename T::value_type>(L, Val.back());
        Obj.Success == Obj.Success && L.Success;
    }
}
template<typename T>
using unordered_map_str = std::unordered_map<std::string, T>;
template<MapClassLoadable T>
void BasicTypeLoad(ORJsonLoader& Obj, T& Val)
{
    auto Map = Obj.GetMapObject();
    for (auto& p : Map)
    {
        ORJsonLoader L(p.second);
        BasicTypeLoad<typename T::mapped_type>(L, Val[p.first]);
        Obj.Success == Obj.Success && L.Success;
    }
}

#define BasicTypeLoad_Specialize(T, Fn) \
template<>\
void BasicTypeLoad< T >(ORJsonLoader& Obj, T & Val)\
{ Val = std::move(Obj. ## Fn ()); }
BasicTypeLoad_Specialize(int, GetInt)
BasicTypeLoad_Specialize(float, GetFloat)
BasicTypeLoad_Specialize(double, GetDouble)
BasicTypeLoad_Specialize(bool, GetBool)
BasicTypeLoad_Specialize(std::string, GetString)
BasicTypeLoad_Specialize(std::vector<int>, GetArrayInt)
BasicTypeLoad_Specialize(std::vector<double>, GetArrayDouble)
BasicTypeLoad_Specialize(std::vector<uint8_t>, GetArrayBool)
BasicTypeLoad_Specialize(std::vector<std::string>, GetArrayString)
BasicTypeLoad_Specialize(std::vector<JsonObject>, GetArrayObject)
BasicTypeLoad_Specialize(unordered_map_str<int>, GetMapInt)
BasicTypeLoad_Specialize(unordered_map_str<double>, GetMapDouble)
BasicTypeLoad_Specialize(unordered_map_str<bool>, GetMapBool)
BasicTypeLoad_Specialize(unordered_map_str<std::string>, GetMapString)
BasicTypeLoad_Specialize(unordered_map_str<JsonObject>, GetMapObject)

JsonObject ORJsonLoader::PreCalcItem(const std::string_view Item)
{
    if (!Success)return NullJsonObject;
    if (!Available())Success = false;
    else
    {
        auto j = GetObjectItem(Item);
        if (!j.Available())Success = false;
        else return j;
    }
    return NullJsonObject;
}
template<typename T>
inline ORJsonLoader& ORJsonLoader::operator()(const std::string_view Item, T& Obj, const std::function<bool(T&)>& DefaultPolicy)
{
    auto J = PreCalcItem(Item);
    if (J.Available())
    {
        ORJsonLoader L(J);
        BasicTypeLoad<T>(L, Obj);
        if (!L.Success)
            Success = Success && DefaultPolicy(Obj);
    }
    return *this;
}
template<typename T>
inline ORJsonLoader& ORJsonLoader::operator()(const std::string_view Item, T& Obj, const T& Default)
{
    auto J = PreCalcItem(Item);
    if (J.Available())
    {
        ORJsonLoader L(J);
        BasicTypeLoad<T>(L, Obj);
        if (!L.Success)
            Obj = Default;
    }
    return *this;
}
template<typename T>
inline ORJsonLoader& ORJsonLoader::operator()(const std::string_view Item, T& Obj, LoadOrSkip)
{
    auto J = PreCalcItem(Item);
    if (J.Available())
    {
        ORJsonLoader L(J);
        BasicTypeLoad<T>(L, Obj);
    }
    return *this;
}
template<typename T>
inline ORJsonLoader& ORJsonLoader::operator()(const std::string_view Item, T& Obj)
{
    auto J = PreCalcItem(Item);
    if (J.Available())
    {
        ORJsonLoader L(J);
        BasicTypeLoad<T>(L, Obj);
        Success = Success && L.Success;
    }
    return *this;
}


#define ORLoadable_DefineLoader void Load(ORJsonLoader& Obj)
#define ORLoadable_DefineLoaderOuter(Type) void Type ## ::Load(ORJsonLoader& Obj)

