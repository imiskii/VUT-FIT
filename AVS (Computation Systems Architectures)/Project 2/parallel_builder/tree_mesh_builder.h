/**
 * @file    tree_mesh_builder.h
 *
 * @author  Michal Ľaš <xlasmi00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    DATE
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include "base_mesh_builder.h"

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned int decomposeOctree(const unsigned gridSize, const Vec3_t<float> &pos, const ParametricScalarField &field);
    unsigned marchCubes(const ParametricScalarField &field);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }


    const unsigned int GRID_SIZE_CUTOFF = 2;
    const unsigned int TREE_CHILDS = 8;
    std::vector<Triangle_t> mTriangles; ///< Temporary array of triangles
};

#endif // TREE_MESH_BUILDER_H
