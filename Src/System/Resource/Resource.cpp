#include "../../Pch.h"
#include "Resource.h"

Resource::Resource(void)
{
    resourceType_ = RESOURCE_TYPE::NONE;
    filePath_ = L"";
    divisionCountX_ = -1;
    divisionCountY_ = -1;
    imageSizeX_ = -1;
    imageSizeY_ = -1;
    handleId_ = -1;
    handleIdArray_ = nullptr;
}

Resource::Resource(RESOURCE_TYPE targetType, const std::wstring& filePath)
{
    resourceType_ = targetType;
    filePath_ = filePath;
    divisionCountX_ = -1;
    divisionCountY_ = -1;
    imageSizeX_ = -1;
    imageSizeY_ = -1;
    handleId_ = -1;
    handleIdArray_ = nullptr;
}

Resource::Resource(RESOURCE_TYPE targetType, const std::wstring& filePath,
    int divisionX, int divisionY, int sizeX, int sizeY)
{
    resourceType_ = targetType;
    filePath_ = filePath;
    divisionCountX_ = divisionX;
    divisionCountY_ = divisionY;
    imageSizeX_ = sizeX;
    imageSizeY_ = sizeY;
    handleId_ = -1;
    handleIdArray_ = nullptr;
}

Resource::~Resource(void)
{
}

void Resource::Load(void)
{
    int previousAsyncFlag = 0;

    switch (resourceType_)
    {
    case RESOURCE_TYPE::IMAGE:
    {
        handleId_ = LoadGraph(filePath_.c_str());
        break;
    }
    case RESOURCE_TYPE::IMAGES:
    {
        int totalCount = divisionCountX_ * divisionCountY_;
        handleIdArray_ = new int[totalCount];

        handleId_ = LoadDivGraph(filePath_.c_str(), totalCount,
            divisionCountX_, divisionCountY_,
            imageSizeX_, imageSizeY_, &handleIdArray_[0]);
        break;
    }
    case RESOURCE_TYPE::MASK:
    {
        handleId_ = LoadGraph(filePath_.c_str());
        break;
    }
    case RESOURCE_TYPE::MODEL:
    case RESOURCE_TYPE::ANIMATION:
    {
        handleId_ = MV1LoadModel(filePath_.c_str());
        break;
    }
    case RESOURCE_TYPE::EFFEKSEER:
    {
        previousAsyncFlag = GetUseASyncLoadFlag();
        SetUseASyncLoadFlag(FALSE);

        handleId_ = LoadEffekseerEffect(filePath_.c_str());

        SetUseASyncLoadFlag(previousAsyncFlag);
        break;
    }
    case RESOURCE_TYPE::SOUND:
    {
        handleId_ = LoadSoundMem(filePath_.c_str());
        break;
    }
    case RESOURCE_TYPE::VERTEX_SHADER:
    {
        handleId_ = LoadVertexShader(filePath_.c_str());
        break;
    }
    case RESOURCE_TYPE::PIXEL_SHADER:
    {
        handleId_ = LoadPixelShader(filePath_.c_str());
        break;
    }
    default:
    {
        break;
    }
    }
}

void Resource::Release(void)
{
    switch (resourceType_)
    {
    case RESOURCE_TYPE::IMAGE:
    case RESOURCE_TYPE::MASK:
    {
        DeleteGraph(handleId_);
        break;
    }
    case RESOURCE_TYPE::IMAGES:
    {
        int totalCount = divisionCountX_ * divisionCountY_;
        if (handleIdArray_ != nullptr)
        {
            for (int index = 0; index < totalCount; index++)
            {
                DeleteGraph(handleIdArray_[index]);
            }
            delete[] handleIdArray_;
            handleIdArray_ = nullptr;
        }
        break;
    }
    case RESOURCE_TYPE::MODEL:
    case RESOURCE_TYPE::ANIMATION:
    {
        MV1DeleteModel(handleId_);

        for (auto duplicatedId : duplicatedModelIdList_)
        {
            MV1DeleteModel(duplicatedId);
        }
        duplicatedModelIdList_.clear();
        break;
    }
    case RESOURCE_TYPE::EFFEKSEER:
    {
        DeleteEffekseerEffect(handleId_);
        break;
    }
    case RESOURCE_TYPE::SOUND:
    {
        DeleteSoundMem(handleId_);
        break;
    }
    case RESOURCE_TYPE::VERTEX_SHADER:
    case RESOURCE_TYPE::PIXEL_SHADER:
    {
        DeleteShader(handleId_);
        break;
    }
    default:
    {
        break;
    }
    }
}

void Resource::CopyHandle(int* outputImages)
{
    if (handleIdArray_ == nullptr)
    {
        return;
    }

    int totalCount = divisionCountX_ * divisionCountY_;
    for (int index = 0; index < totalCount; index++)
    {
        outputImages[index] = handleIdArray_[index];
    }
}

int Resource::GetHandleId(void) const
{
    return handleId_;
}
