/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  Michal Ľaš <xlasmi00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    DATE
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

unsigned int TreeMeshBuilder::decomposeOctree(const unsigned gridSize, const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    static const float sphere_radius_exp = sqrtf(3.0f) / 2.0f;
    
    /* 1. Kontrola či sa podstrom prechádza hľadaným povrchom */
    const unsigned newGridSize = gridSize / 2;
    const Vec3_t<float> midPoint = {
        (pos.x + newGridSize) * mGridResolution,
        (pos.y + newGridSize) * mGridResolution,
        (pos.z + newGridSize) * mGridResolution 
    };
    float fieldPoint = evaluateFieldAt(midPoint, field);
    float fieldCondition = mIsoLevel + sphere_radius_exp * (gridSize * mGridResolution);

    if (fieldPoint <= fieldCondition){
        /* 2.1. Ak je dosiahnuté maximálne zanorenie - podmenka konca rekurzie */
        if (gridSize <= GRID_SIZE_CUTOFF){
            unsigned cubeTriangles = 0;
            /* Pre malé bloky sekvenčné spracovanie zvyšku */
            for (size_t i = 0; i < (gridSize * gridSize * gridSize); ++i){
                Vec3_t<float> newCubeOffset(pos.x + i % gridSize,
                                    pos.y + (i / gridSize) % gridSize,
                                    pos.z + i / (gridSize*gridSize));

                cubeTriangles += buildCube(newCubeOffset, field);
            }
            return cubeTriangles;
        } 
        /* 2.2. Inak rozdeľ blok na TREE_CHILDS (8) menších častí */
        else {
            unsigned totalTriangles = 0; // Triangle counter
            /* 3. Pre každý podstrom vytvor samostatný task */
            for (const auto &cube : sc_vertexNormPos){
                #pragma omp task shared(field, totalTriangles, pos, newGridSize)
                {
                    const Vec3_t<float> nextCubePos {
                        pos.x + cube.x * newGridSize,
                        pos.y + cube.y * newGridSize,
                        pos.z + cube.z * newGridSize
                    };

                    /* Do totalTriangles sa musí pričítavať atomicky aby bol na konci správny výsledok */
                    #pragma omp atomic update
                    totalTriangles += decomposeOctree(newGridSize, nextCubePos, field);
                }
            }
            /* 4. Čakanie kým skončia všetky tasky */
            #pragma omp taskwait
            return totalTriangles;
        }
    } else {
        return 0;
    }
}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    unsigned totalTriangles = 0; // Triangle counter
    #pragma omp parallel shared(field, totalTriangles)
    {
        #pragma omp master
        {
            totalTriangles = decomposeOctree(mGridSize, Vec3_t<float>(), field);
        }
    }
    return totalTriangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    for(unsigned i = 0; i < count; ++i)
    {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        value = std::min(value, distanceSquared);
    }

    return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    #pragma omp critical
    mTriangles.push_back(triangle);
}
