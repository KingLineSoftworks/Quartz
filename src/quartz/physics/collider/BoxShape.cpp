#include "math/transform/Vec3.hpp"

#include "quartz/physics/collider/BoxShape.hpp"

quartz::physics::BoxShape::BoxShape(
    reactphysics3d::BoxShape* p_boxShape
) :
    mp_colliderShape(p_boxShape)
{}

quartz::physics::BoxShape::BoxShape(
    quartz::physics::BoxShape&& other
) :
    mp_colliderShape(std::move(other.mp_colliderShape))
{}

quartz::physics::BoxShape&
quartz::physics::BoxShape::operator=(
    quartz::physics::BoxShape&& other
) {
    if (this == &other) {
        return *this;
    }

    mp_colliderShape = std::move(other.mp_colliderShape);

    return *this;
}

math::Vec3
quartz::physics::BoxShape::getHalfExtents_m() const {
    return mp_colliderShape->getHalfExtents();
}

std::array<math::Vec3, 8>
quartz::physics::BoxShape::getLocalVertexPositions() const {
    return {
        mp_colliderShape->getVertexPosition(0), // -x -y  z
        mp_colliderShape->getVertexPosition(1), //  x -y  z
        mp_colliderShape->getVertexPosition(2), //  x  y  z
        mp_colliderShape->getVertexPosition(3), // -x  y  z
        mp_colliderShape->getVertexPosition(4), // -x -y -z
        mp_colliderShape->getVertexPosition(5), //  x -y -z
        mp_colliderShape->getVertexPosition(6), //  x  y -z
        mp_colliderShape->getVertexPosition(7), // -x  y -z
    };
}

void
quartz::physics::BoxShape::setHalfExtents_m(
    const math::Vec3& halfExtents_m
) {
    mp_colliderShape->setHalfExtents(halfExtents_m);
}

