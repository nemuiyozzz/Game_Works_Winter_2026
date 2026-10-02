#include "EcsRegistry.h"

EcsRegistry::EcsRegistry(void)
	: nextEntityId_(0)
{
}

Entity EcsRegistry::CreateEntity(void)
{
	// 再利用可能なIDがあればそれを使う
	if (!freeEntities_.empty())
	{
		Entity reusedEntity = freeEntities_.front();
		freeEntities_.pop();
		return reusedEntity;
	}

	// なければ新しいIDを発行する
	Entity newEntity = nextEntityId_;
	nextEntityId_++;
	return newEntity;
}

void EcsRegistry::DestroyEntity(Entity entity)
{
	// すべてのコンポーネントプールからこのエンティティのデータを削除する
	for (auto& pair : componentPools_)
	{
		std::shared_ptr<IComponentPool> pool = pair.second;
		pool->RemoveEntity(entity);
	}

	// IDを再利用キューに戻す
	freeEntities_.push(entity);
}
