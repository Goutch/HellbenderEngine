//

// Created by username on 3/7/2025.
//

#include "Node.h"
#include "core/scene/Scene.h"

namespace HBE {
    Node::Node() {
    }

    Node::Node(Entity entity) {
        this->entity = entity;
        this->scene = entity.getScene();
    }

    //Node::Node(Node &&other) noexcept {
    //    parent = other.parent;
    //    children = std::move(other.children);
    //}

    uint32_t Node::getDepth() const {
        return depth;
    }

    uint32_t Node::getLocalIndex() const {
        return local_index;
    }

    uint32_t Node::getGlobalIndex() const {
        return global_index;
    }

    bool Node::hasParent() const {
        return parent != NULL_ENTITY_HANDLE;
    }

    void Node::setParent(Entity parent_entity) {
        HB_ASSERT(scene!=nullptr, "Scene must not be a nullptr to set the entity Node component parent");
        scene->setParent(entity, parent_entity);
    }


    void Node::setParent(entity_handle parent_entity_handle) {
        HB_ASSERT(scene!=nullptr, "Scene must not be a nullptr to set the entity Node component parent");
        scene->setParent(entity.getHandle(), parent_entity_handle);
    }

    entity_handle Node::getParentHandle() {
        return parent;
    }

    Entity Node::getParent() {
        return Entity(parent, scene);
    }

    bool Node::isActive() const {
        return is_active;
    }

    bool Node::isActiveInHierarchy() {
        if (!isActive()) return false;
        if (active_in_hierarchy_dirty) {
            if (entity.getScene()->valid(parent)) {
                Node *parent_node = entity.getScene()->get<Node>(this->parent);
                if (parent_node == nullptr) {
                    is_active_in_hierarchy = true;
                }

                is_active_in_hierarchy = parent_node->isActiveInHierarchy();
            } else {
                is_active_in_hierarchy = true;
            }
            active_in_hierarchy_dirty = false;
        }
        return is_active_in_hierarchy;
    }
}
