/*
Copyright libCellML Contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "libcellml/component.h"
#include "libcellml/componententity.h"
#include "libcellml/entity.h"
#include "libcellml/model.h"

namespace libcellml {

/**
 * @brief The Entity::EntityImpl struct.
 *
 * The private implementation for the Entity class.
 */
struct Entity::EntityImpl
{
    ModelPtr mParentModel; /**< Pointer to parent model. */
    ComponentPtr mParentComponent; /**< Pointer to component model. */
    std::string mId; /**< String document identifier for this entity. */
};

Entity::Entity()
    : mPimpl(new EntityImpl())
{
    mPimpl->mParentModel = nullptr;
    mPimpl->mParentComponent = nullptr;
}

Entity::~Entity()
{
    delete mPimpl;
}

Entity::Entity(const Entity &rhs)
    : mPimpl(new EntityImpl())
{
    mPimpl->mParentComponent = rhs.mPimpl->mParentComponent;
    mPimpl->mParentModel = rhs.mPimpl->mParentModel;
    mPimpl->mId = rhs.mPimpl->mId;
}

Entity::Entity(Entity &&rhs) noexcept
    : mPimpl(rhs.mPimpl)
{
    rhs.mPimpl = nullptr;
}

Entity &Entity::operator=(Entity rhs)
{
    rhs.swap(*this);
    return *this;
}

void Entity::swap(Entity &rhs)
{
    std::swap(mPimpl, rhs.mPimpl);
}

void Entity::setId(const std::string &id)
{
    mPimpl->mId = id;
}

std::string Entity::id() const
{
    return mPimpl->mId;
}

ModelPtr Entity::parentModel() const
{
    return mPimpl->mParentModel;
}

ComponentPtr Entity::parentComponent() const
{
    return mPimpl->mParentComponent;
}

void Entity::setParent(const ModelPtr &parent)
{
    mPimpl->mParentModel = parent;
}

void Entity::setParent(const ComponentPtr &parent)
{
    mPimpl->mParentComponent = parent;
}

void Entity::clearParent()
{
    mPimpl->mParentComponent = nullptr;
    mPimpl->mParentModel = nullptr;
}

bool Entity::hasParent(const ComponentPtr &component) const
{
    if (mPimpl->mParentComponent == component) {
        return true;
    }

    if (mPimpl->mParentComponent) {
        return mPimpl->mParentComponent->hasParent(component);
    }

    return false;
}

} // namespace libcellml
