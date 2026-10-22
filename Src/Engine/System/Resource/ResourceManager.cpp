#include "../../../Pch.h"
#include "ResourceManager.h"
#include "../../../Application.h"

namespace 
{
    // ファイル名関連
    constexpr const wchar_t* RESOURCE_CSV_NAME = L"ResourceList.csv"; // リソースCSV名
    constexpr const wchar_t* RESOURCE_BIN_NAME = L"ResourceList.bin"; // リソースバイナリ名

    // CSVカラム番号関連
    constexpr size_t COLUMN_KEY = 0;        // キー名の列
    constexpr size_t COLUMN_TYPE = 1;       // リソース種別の列
    constexpr size_t COLUMN_PATH = 2;       // ファイルパスの列
    constexpr size_t COLUMN_DIV_X = 3;      // 横分割数の列
    constexpr size_t COLUMN_DIV_Y = 4;      // 縦分割数の列
    constexpr size_t COLUMN_SIZE_X = 5;     // 横幅の列
    constexpr size_t COLUMN_SIZE_Y = 6;     // 縦幅の列
    constexpr size_t MIN_COLUMN_COUNT = 3;  // 最小列数

    // バイナリ識別関連
    constexpr const char* BINARY_SIGNATURE = "RESB"; // バイナリ識別子
    constexpr size_t SIGNATURE_SIZE = 4;             // 識別子サイズ
}

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

void ResourceManager::Release(void)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    for (auto& entry : loadedResources_)
    {
        if (entry.resource != nullptr)
        {
            entry.resource->Release();
        }
    }

    loadedResources_.clear();
}

Resource ResourceManager::Load(const std::wstring& targetKey)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    Resource* loadedResource = ExecuteLoad(targetKey);

    if (loadedResource == nullptr)
    {
        return Resource();
    }

    return *loadedResource;
}

int ResourceManager::LoadModelDuplicate(const std::wstring& targetKey)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    Resource* loadedResource = ExecuteLoad(targetKey);

    if (loadedResource == nullptr || loadedResource->handleId_ == -1)
    {
        return -1;
    }

    int duplicatedId = MV1DuplicateModel(loadedResource->handleId_);

    loadedResource->duplicatedModelIdList_.push_back(duplicatedId);

    return duplicatedId;
}

int ResourceManager::GetHandleId(const std::wstring& targetKey)
{
    std::lock_guard<std::mutex> lockThread(resourceMutex_);

    Resource* loadedResource = ExecuteLoad(targetKey);

    if (loadedResource == nullptr)
    {
        return -1;
    }

    return loadedResource->GetHandleId();
}

bool ResourceManager::SaveBinary(const std::wstring& binaryFilePath) const
{
    std::ofstream file(binaryFilePath, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    ResourceBinaryHeader header{};
    header.recordCount = static_cast<uint32_t>(registeredResources_.size());
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));

    for (const auto& entry : registeredResources_)
    {
        const std::wstring& keyName = entry.key;
        const Resource& resourceData = entry.resource;

        // キー名の文字数と文字列データ
        uint32_t keyLength = static_cast<uint32_t>(keyName.size());
        file.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
        file.write(reinterpret_cast<const char*>(keyName.data()),
            sizeof(wchar_t) * keyLength);

        // リソース種別
        int32_t typeValue = static_cast<int32_t>(resourceData.resourceType_);
        file.write(reinterpret_cast<const char*>(&typeValue), sizeof(typeValue));

        // ファイルパスの文字数と文字列データ
        uint32_t pathLength = static_cast<uint32_t>(resourceData.filePath_.size());
        file.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
        file.write(reinterpret_cast<const char*>(resourceData.filePath_.data()),
            sizeof(wchar_t) * pathLength);

        // 分割画像用のパラメータ4つ
        file.write(reinterpret_cast<const char*>(&resourceData.divisionCountX_),
            sizeof(resourceData.divisionCountX_));
        file.write(reinterpret_cast<const char*>(&resourceData.divisionCountY_),
            sizeof(resourceData.divisionCountY_));
        file.write(reinterpret_cast<const char*>(&resourceData.imageSizeX_),
            sizeof(resourceData.imageSizeX_));
        file.write(reinterpret_cast<const char*>(&resourceData.imageSizeY_),
            sizeof(resourceData.imageSizeY_));
    }

    return true;
}

void ResourceManager::Initialize(void)
{
    InitTypeTable();

    std::wstring csvPath = Application::PATH_CSV + RESOURCE_CSV_NAME;
    std::wstring binaryPath = Application::PATH_BINARY + RESOURCE_BIN_NAME;

    LoadResourceList(csvPath, binaryPath);
}

void ResourceManager::InitTypeTable(void)
{
    typeTable_.clear();
    typeTable_[L"IMAGE"] = Resource::RESOURCE_TYPE::IMAGE;
    typeTable_[L"IMAGES"] = Resource::RESOURCE_TYPE::IMAGES;
    typeTable_[L"MASK"] = Resource::RESOURCE_TYPE::MASK;
    typeTable_[L"MODEL"] = Resource::RESOURCE_TYPE::MODEL;
    typeTable_[L"ANIMATION"] = Resource::RESOURCE_TYPE::ANIMATION;
    typeTable_[L"EFFEKSEER"] = Resource::RESOURCE_TYPE::EFFEKSEER;
    typeTable_[L"SOUND"] = Resource::RESOURCE_TYPE::SOUND;
    typeTable_[L"VERTEX_SHADER"] = Resource::RESOURCE_TYPE::VERTEX_SHADER;
    typeTable_[L"PIXEL_SHADER"] = Resource::RESOURCE_TYPE::PIXEL_SHADER;
}

void ResourceManager::LoadResourceList(const std::wstring& csvFilePath,
    const std::wstring& binaryFilePath)
{
    registeredResources_.clear();

#ifdef _DEBUG
    // デバッグ時はCSVから読み込み、自動で最新バイナリを書き出す
    if (!LoadCsv(csvFilePath))
    {
        CreateDefaultCsv(csvFilePath);
        LoadCsv(csvFilePath);
    }
    SaveBinary(binaryFilePath);
#else
    // リリース時は高速なバイナリを優先し、無ければCSVから読み込む
    if (!LoadBinary(binaryFilePath))
    {
        if (LoadCsv(csvFilePath))
        {
            SaveBinary(binaryFilePath);
        }
    }
#endif
}

bool ResourceManager::LoadCsv(const std::wstring& csvFilePath)
{
    std::wifstream file(csvFilePath);

    if (!file.is_open())
    {
        return false;
    }

    std::wstring line;

    if (!std::getline(file, line))
    {
        return false;
    }

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (line.back() == L'\r')
        {
            line.pop_back();
        }

        std::wstringstream stream(line);
        std::wstring cell;
        std::vector<std::wstring> columns;

        while (std::getline(stream, cell, L','))
        {
            columns.push_back(cell);
        }

        if (columns.size() < MIN_COLUMN_COUNT)
        {
            continue;
        }

        const std::wstring& keyName = columns[COLUMN_KEY];
        const std::wstring& typeName = columns[COLUMN_TYPE];
        const std::wstring& relativePath = columns[COLUMN_PATH];

        auto typeIterator = typeTable_.find(typeName);
        if (keyName.empty() || typeIterator == typeTable_.end())
        {
            continue;
        }

        Resource::RESOURCE_TYPE resourceType = typeIterator->second;
        std::wstring fullPath = GetBaseFolderPath(resourceType) + relativePath;

        if (resourceType == Resource::RESOURCE_TYPE::IMAGES &&
            columns.size() > COLUMN_SIZE_Y)
        {
            int divisionX = std::stoi(columns[COLUMN_DIV_X]);
            int divisionY = std::stoi(columns[COLUMN_DIV_Y]);
            int sizeX = std::stoi(columns[COLUMN_SIZE_X]);
            int sizeY = std::stoi(columns[COLUMN_SIZE_Y]);

            Resource newResource(resourceType, fullPath,
                divisionX, divisionY, sizeX, sizeY);
            RegisterResource(keyName, newResource);
        }
        else
        {
            Resource newResource(resourceType, fullPath);
            RegisterResource(keyName, newResource);
        }
    }

    return !registeredResources_.empty();
}

bool ResourceManager::LoadBinary(const std::wstring& binaryFilePath)
{
    std::ifstream file(binaryFilePath, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    ResourceBinaryHeader header{};
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (std::memcmp(header.signature, BINARY_SIGNATURE, SIGNATURE_SIZE) != 0)
    {
        return false;
    }

    registeredResources_.reserve(header.recordCount);

    for (uint32_t index = 0; index < header.recordCount; ++index)
    {
        // キー名の読み込み
        uint32_t keyLength = 0;
        file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
        std::wstring keyName(keyLength, L'\0');
        file.read(reinterpret_cast<char*>(keyName.data()),
            sizeof(wchar_t) * keyLength);

        // リソース種別の読み込み
        int32_t typeValue = 0;
        file.read(reinterpret_cast<char*>(&typeValue), sizeof(typeValue));
        Resource::RESOURCE_TYPE resourceType =
            static_cast<Resource::RESOURCE_TYPE>(typeValue);

        // ファイルパスの読み込み
        uint32_t pathLength = 0;
        file.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
        std::wstring fullPath(pathLength, L'\0');
        file.read(reinterpret_cast<char*>(fullPath.data()),
            sizeof(wchar_t) * pathLength);

        // 分割パラメータの読み込み
        int divisionX = -1;
        int divisionY = -1;
        int sizeX = -1;
        int sizeY = -1;
        file.read(reinterpret_cast<char*>(&divisionX), sizeof(divisionX));
        file.read(reinterpret_cast<char*>(&divisionY), sizeof(divisionY));
        file.read(reinterpret_cast<char*>(&sizeX), sizeof(sizeX));
        file.read(reinterpret_cast<char*>(&sizeY), sizeof(sizeY));

        if (resourceType == Resource::RESOURCE_TYPE::IMAGES)
        {
            Resource newResource(resourceType, fullPath,
                divisionX, divisionY, sizeX, sizeY);
            RegisterResource(keyName, newResource);
        }
        else
        {
            Resource newResource(resourceType, fullPath);
            RegisterResource(keyName, newResource);
        }
    }

    return !registeredResources_.empty();
}

void ResourceManager::CreateDefaultCsv(const std::wstring& csvFilePath) const
{
    std::wofstream file(csvFilePath);
    if (!file.is_open())
    {
        return;
    }

    file << L"Key,Type,FilePath,DivX,DivY,SizeX,SizeY\n";
    file << L"IMAGE_SAMPLE,IMAGE,sample.png,0,0,0,0\n";
    file << L"IMAGES_SAMPLE,IMAGES,samples.png,2,2,64,64\n";
    file << L"MODEL_SAMPLE,MODEL,Player.mv1,0,0,0,0\n";
    file << L"ANIMATION_SAMPLE,ANIMATION,sample_anim.mv1,0,0,0,0\n";
    file << L"EFFECT_SAMPLE,EFFEKSEER,sample.efkefc,0,0,0,0\n";
    file << L"SOUND_SAMPLE,SOUND,sample.mp3,0,0,0,0\n";
    file << L"SHADER_VERTEX_SAMPLE,VERTEX_SHADER,sampleVS.cso,0,0,0,0\n";
    file << L"SHADER_PIXEL_SAMPLE,PIXEL_SHADER,samplePS.cso,0,0,0,0\n";
}

std::wstring ResourceManager::GetBaseFolderPath(Resource::RESOURCE_TYPE targetType) const
{
    switch (targetType)
    {
    case Resource::RESOURCE_TYPE::IMAGE:
    case Resource::RESOURCE_TYPE::IMAGES:
    case Resource::RESOURCE_TYPE::MASK:
        return Application::PATH_IMAGE;

    case Resource::RESOURCE_TYPE::MODEL:
        return Application::PATH_MODEL;

    case Resource::RESOURCE_TYPE::ANIMATION:
        return Application::PATH_ANIM;

    case Resource::RESOURCE_TYPE::EFFEKSEER:
        return Application::PATH_EFFECT;

    case Resource::RESOURCE_TYPE::SOUND:
        return Application::PATH_SOUND;

    case Resource::RESOURCE_TYPE::VERTEX_SHADER:
    case Resource::RESOURCE_TYPE::PIXEL_SHADER:
        return Application::PATH_SHADER;

    default:
        return Application::PATH_DATA;
    }
}

void ResourceManager::RegisterResource(const std::wstring& keyName, const Resource& newResource)
{
    for (auto& entry : registeredResources_)
    {
        if (entry.key == keyName)
        {
            entry.resource = newResource;
            return;
        }
    }

    RegisteredResourceEntry newEntry{};
    newEntry.key = keyName;
    newEntry.resource = newResource;
    registeredResources_.push_back(newEntry);
}

Resource* ResourceManager::FindRegisteredResource(const std::wstring& targetKey)
{
    for (auto& entry : registeredResources_)
    {
        if (entry.key == targetKey)
        {
            return &entry.resource;
        }
    }
    return nullptr;
}

Resource* ResourceManager::FindLoadedResource(const std::wstring& targetKey)
{
    for(auto& entry : loadedResources_)
    {
        if (entry.key == targetKey)
        {
            return entry.resource.get();
        }
    }
    return nullptr;
}

Resource* ResourceManager::ExecuteLoad(const std::wstring& targetKey)
{
    Resource* loadedResource = FindLoadedResource(targetKey);

    if (loadedResource != nullptr)
    {
        return loadedResource;
    }

    Resource* registeredResource = FindRegisteredResource(targetKey);

    if (registeredResource == nullptr)
    {
        return nullptr;
    }

    registeredResource->Load();

    LoadedResourceEntry newLoadedEntry{};

    newLoadedEntry.key = targetKey;
    newLoadedEntry.resource = std::make_unique<Resource>(*registeredResource);

    Resource* resultPointer = newLoadedEntry.resource.get();

    loadedResources_.push_back(std::move(newLoadedEntry));

    return resultPointer;
}