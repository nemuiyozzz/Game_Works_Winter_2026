#include "../../Pch.h"
#include "ResourceManager.h"
#include "../../Application.h"

// シングルトンのインスタンス初期化
ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new ResourceManager();
    }
}

ResourceManager& ResourceManager::GetInstance(void)
{
    return *instance_;
}

void ResourceManager::DestroyInstance(void)
{
    if (instance_)
    {
        instance_->Release();
        delete instance_;
    }
}

ResourceManager::ResourceManager(void)
{
}

void ResourceManager::Initialize(void)
{
    Resource sampleResource;

    // 単一画像のサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::IMAGE,
        Application::PATH_IMAGE + L"sample.png");
    registeredResourcesMap_.emplace(RESOURCE_ID::IMAGE_SAMPLE, sampleResource);

    // 分割画像のサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::IMAGES,
        Application::PATH_IMAGE + L"samples.png", 2, 2, 64, 64);
    registeredResourcesMap_.emplace(RESOURCE_ID::IMAGES_SAMPLE, sampleResource);

    // 3Dモデルのサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::MODEL,
        Application::PATH_MODEL + L"Player.mv1");
    registeredResourcesMap_.emplace(RESOURCE_ID::MODEL_SAMPLE, sampleResource);

    // アニメーションのサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::ANIMATION,
        Application::PATH_ANIM + L"sample_anim.mv1");
    registeredResourcesMap_.emplace(RESOURCE_ID::ANIMATION_SAMPLE, sampleResource);

    // エフェクトのサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::EFFEKSEER,
        Application::PATH_EFFECT + L"sample.efkefc");
    registeredResourcesMap_.emplace(RESOURCE_ID::EFFECT_SAMPLE, sampleResource);

    // サウンドのサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::SOUND,
        Application::PATH_SOUND + L"sample.mp3");
    registeredResourcesMap_.emplace(RESOURCE_ID::SOUND_SAMPLE, sampleResource);

    // 頂点シェーダのサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::VERTEX_SHADER,
        Application::PATH_SHADER + L"sampleVS.cso");
    registeredResourcesMap_.emplace(RESOURCE_ID::SHADER_VERTEX_SAMPLE, sampleResource);

    // ピクセルシェーダのサンプル
    sampleResource = Resource(Resource::RESOURCE_TYPE::PIXEL_SHADER,
        Application::PATH_SHADER + L"samplePS.cso");
    registeredResourcesMap_.emplace(RESOURCE_ID::SHADER_PIXEL_SAMPLE, sampleResource);
}

void ResourceManager::Release(void)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    for (auto& resourcePair : loadedResourcesMap_)
    {
        resourcePair.second->Release();
        delete resourcePair.second;
    }

    loadedResourcesMap_.clear();
}

Resource ResourceManager::Load(RESOURCE_ID targetId)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    Resource* loadedResource = ExecuteLoad(targetId);
    if (loadedResource == nullptr)
    {
        return Resource();
    }

    return *loadedResource;
}

int ResourceManager::LoadModelDuplicate(RESOURCE_ID targetId)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    Resource* loadedResource = ExecuteLoad(targetId);
    if (!loadedResource || loadedResource->handleId_ == -1)
    {
        return -1;
    }

    int duplicatedId = MV1DuplicateModel(loadedResource->handleId_);
    loadedResource->duplicatedModelIdList_.push_back(duplicatedId);

    return duplicatedId;
}

int ResourceManager::GetHandleId(RESOURCE_ID targetId)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);
    auto mapIterator = registeredResourcesMap_.find(targetId);

    if (mapIterator == registeredResourcesMap_.end())
    {
        return -1;
    }

    return mapIterator->second.GetHandleId();
}

Resource* ResourceManager::ExecuteLoad(RESOURCE_ID targetId)
{
    auto loadedIterator = loadedResourcesMap_.find(targetId);

    if (loadedIterator != loadedResourcesMap_.end())
    {
        return loadedIterator->second;
    }

    auto registeredIterator = registeredResourcesMap_.find(targetId);
    if (registeredIterator == registeredResourcesMap_.end())
    {
        return nullptr;
    }

    registeredIterator->second.Load();

    Resource* newResource = new Resource(registeredIterator->second);
    loadedResourcesMap_.emplace(targetId, newResource);

    return newResource;
}