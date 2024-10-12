#pragma once
#include "ExtJson.h"
#include "ORFile.h"
#include "Minimal.h"
#include <functional>
#include <imgui.h>
#include "ORException.h"
#include "ORTimer.h"

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

/*
ParseFromFile会抛异常，接着点！
*/
class ORJsonSource:public JsonFile
{
public:
    inline void ParseFromFile(ORStaticStraw& Src);
    inline void ParseFromFile(const std::string_view Name);
    inline ORJsonLoader GetLoader() const;
    template<typename T>
    inline bool LoadObject(T& Obj);
    template<typename T>
    inline bool LoadObject(const std::string_view Name, T& Obj);
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
inline void BasicTypeLoad<JsonObject>(ORJsonLoader& Obj, JsonObject& Val)
{
    Val = (JsonObject)Obj;
}

template<>
inline void BasicTypeLoad<ETimer::dura_t>(ORJsonLoader& Obj, ETimer::dura_t& Val)
{
    auto O = Obj.GetObjectItem("second");
    if (O.Available())
    {
        Val = std::chrono::seconds((int64_t)O.GetInt());
        return;
    }
    O = Obj.GetObjectItem("milli");
    if (O.Available())
    {
        Val = std::chrono::milliseconds((int64_t)O.GetInt());
        return;
    }
    O = Obj.GetObjectItem("micro");
    if (O.Available())
    {
        Val = std::chrono::microseconds((int64_t)O.GetInt());
        return;
    }
    O = Obj.GetObjectItem("nano");
    if (O.Available())
    {
        Val = std::chrono::nanoseconds((int64_t)O.GetInt());
        return;
    }
    Obj.Success = false;
}

template<>
inline void BasicTypeLoad<ImVec2>(ORJsonLoader& Obj, ImVec2& Val)
{
    Val.x = Obj.GetArrayItem(0).GetFloat();
    Val.y = Obj.GetArrayItem(1).GetFloat();
}

template<>
inline void BasicTypeLoad<ImColor>(ORJsonLoader& Obj, ImColor& Val)
{
    if (Obj.IsTypeNull())
    {
        Val = ImColor(0, 0, 0, 0);
        return;
    }
    auto O = Obj.GetObjectItem("RGB");
    if (O.Available())
    {
        auto V = O.GetArrayInt();
        if (V.size() < 3)Obj.Success = false;
        else Val = ImColor(V[0], V[1], V[2]);
        return;
    }
    O = Obj.GetObjectItem("RGBA");
    if (O.Available())
    {
        auto V = O.GetArrayInt();
        if (V.size() < 4)Obj.Success = false;
        else Val = ImColor(V[0], V[1], V[2], V[3]);
        return;
    }
    O = Obj.GetObjectItem("KA");
    if (O.Available())
    {
        auto V = O.GetArrayInt();
        if (V.size() < 2)Obj.Success = false;
        else Val = ImColor(V[0], V[0], V[0], V[1]);
        return;
    }
    O = Obj.GetObjectItem("HSV");
    if (O.Available())
    {
        auto V = O.GetArrayInt();
        if (V.size() < 3)Obj.Success = false;
        else Val.SetHSV(V[0] / 360.0F, V[1] / 100.0F, V[2] / 100.0F);
        return;
    }
    O = Obj.GetObjectItem("HSVA");
    if (O.Available())
    {
        auto V = O.GetArrayInt();
        if (V.size() < 4)Obj.Success = false;
        else Val.SetHSV(V[0] / 360.0F, V[1] / 100.0F, V[2] / 100.0F, V[3] / 255.0F);
        return;
    }
    Obj.Success = false;
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
inline void BasicTypeLoad< T >(ORJsonLoader& Obj, T & Val)\
{ Val = std::move(Obj. ## Fn ()); }
#define BasicTypeLoad_PlainSpecialize(T, Fn) \
template<>\
inline void BasicTypeLoad< T >(ORJsonLoader& Obj, T & Val)\
{ Val = ( T ) Obj. ## Fn (); }
BasicTypeLoad_PlainSpecialize(int, GetInt)
BasicTypeLoad_PlainSpecialize(unsigned, GetInt)
BasicTypeLoad_PlainSpecialize(short, GetInt)
BasicTypeLoad_PlainSpecialize(unsigned short, GetInt)
BasicTypeLoad_PlainSpecialize(float, GetFloat)
BasicTypeLoad_PlainSpecialize(double, GetDouble)
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

inline JsonObject ORJsonLoader::PreCalcItem(const std::string_view Item)
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
    else Success = DefaultPolicy(Obj);
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
    else
    {
        Obj = Default;
        Success = true;
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

template<typename T>
inline bool ORJsonSource::LoadObject(T& Obj)
{
    ORJsonLoader Loader = GetLoader();
    BasicTypeLoad(Loader, Obj);
    return Loader.Success;
}
template<typename T>
inline bool ORJsonSource::LoadObject(const std::string_view Item, T& Obj)
{
    ORJsonLoader Loader = GetLoader();
    ORJsonLoader L = Loader.GetObjectItem(Item);
    if (!L.Available())return false;
    BasicTypeLoad(L, Obj);
    return Loader.Success;
}

inline void ORJsonSource::ParseFromFile(ORStaticStraw& Src)
{
    auto Str = Src.ReadWholeAsString(0);
    if (Str.empty()) throw (ORException(u8"ORJsonSource::ParseFromFile未获取到字符串！"));
    auto Err = ParseTmpChecked(std::move(Str), u8"<- 错误位置 ->");
    if (!Err.empty()) throw (ORException(std::move(Err)));
}
inline void ORJsonSource::ParseFromFile(const std::string_view Name)
{
    auto st = ORExtFileStraw(Name);
    ParseFromFile(st);
}
inline ORJsonLoader ORJsonSource::GetLoader() const
{
    return ORJsonLoader(GetObj());
}

#define ORLoadable_DefineLoader void Load(ORJsonLoader& Obj)
#define ORLoadable_DefineLoaderOuter(Type) void Type ## ::Load(ORJsonLoader& Obj)

