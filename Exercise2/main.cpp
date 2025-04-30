#include <iostream>
#include <cmath>
#include <algorithm>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;

// Test 1: Markers

bool TestMarkersStoredCorrectly(PolygonalMesh& mesh)
{
    for (const auto& [marker, ids] : mesh.Cell0DMarkers)
        for (auto id : ids)
            if (find(mesh.Cell0DsId.begin(), mesh.Cell0DsId.end(), id) == mesh.Cell0DsId.end())
                return false;

    for (const auto& [marker, ids] : mesh.Cell1DMarkers)
        for (auto id : ids)
            if (find(mesh.Cell1DsId.begin(), mesh.Cell1DsId.end(), id) == mesh.Cell1DsId.end())
                return false;

    for (const auto& [marker, ids] : mesh.Cell2DMarkers)
        for (auto id : ids)
            if (find(mesh.Cell2DsId.begin(), mesh.Cell2DsId.end(), id) == mesh.Cell2DsId.end())
                return false;

    cout << "Marker Registrati:" << endl;

    for (const auto& [marker, list_id] : mesh.Cell0DMarkers)
    {
        cout << "Marker0D: " << marker << "  IDs = [";
        for (auto& id : list_id)
            cout << ' ' << id;
        cout << " ]" << endl;
    }

    cout << endl;
    for (const auto& [marker, list_id] : mesh.Cell1DMarkers)
    {
        cout << "Marker1D: " << marker << "  IDs = [";
        for (auto& id : list_id)
            cout << ' ' << id;
        cout << " ]" << endl;
    }

    cout << endl;
    for (const auto& [marker, list_id] : mesh.Cell2DMarkers)
    {
        cout << "Marker2D: " << marker << "  IDs = [";
        for (auto& id : list_id)
            cout << ' ' << id;
        cout << " ]" << endl;
    }

    return true;
}

// Test 2: Edge length

bool TestEdgesHaveNonZeroLength(PolygonalMesh& mesh)
{
    for (unsigned int i = 0; i < mesh.NumCell1Ds; ++i)
    {
        unsigned int id0 = mesh.Cell1DsExtrema(0, i);
        unsigned int id1 = mesh.Cell1DsExtrema(1, i);

        if (id0 >= mesh.Cell0DsCoordinates.cols() || id1 >= mesh.Cell0DsCoordinates.cols())
        {
            cerr << "Invalid extrema id: id0=" << id0 << ", id1=" << id1 << endl;
            return false;
        }

        unsigned int idx0 = mesh.Cell0DIdToIndex.at(id0);
        Vector2d p0 = mesh.Cell0DsCoordinates.block<2, 1>(0, idx0);
        unsigned int idx1 = mesh.Cell0DIdToIndex.at(id1);
        Vector2d p1 = mesh.Cell0DsCoordinates.block<2, 1>(0, idx1);

        if ((id0 == 18 && id1 == 11) || (id0 == 11 && id1 == 18)) {
            cout << "DEBUG: p0 (id=" << id0 << ") = [" << p0.transpose() << "]\n";
            cout << "DEBUG: p1 (id=" << id1 << ") = [" << p1.transpose() << "]\n";
        }

        if ((p1 - p0).norm() < 1e-16)
            return false;
    }

    return true;
}

// Test 3: Polygon area

bool TestPolygonsHaveNonZeroArea(PolygonalMesh& mesh)
{
    for (unsigned int i = 0; i < mesh.NumCell2Ds; ++i)
    {
        const auto& vertexIds = mesh.Cell2DsVertices[i];

        if (vertexIds.size() < 3)
            return false;

        double area = 0.0;

        for (size_t j = 0; j < vertexIds.size(); ++j)
        {
            unsigned int id0 = vertexIds[j];
            unsigned int id1 = vertexIds[(j + 1) % vertexIds.size()];

            if (id0 >= mesh.Cell0DsCoordinates.cols() || id1 >= mesh.Cell0DsCoordinates.cols())
            {
                cerr << "Invalid polygon vertex id: id0=" << id0 << ", id1=" << id1 << endl;
                return false;
            }

            unsigned int idx0 = mesh.Cell0DIdToIndex.at(id0);
        Vector2d p0 = mesh.Cell0DsCoordinates.block<2, 1>(0, idx0);
            unsigned int idx1 = mesh.Cell0DIdToIndex.at(id1);
        Vector2d p1 = mesh.Cell0DsCoordinates.block<2, 1>(0, idx1);

        if ((id0 == 18 && id1 == 11) || (id0 == 11 && id1 == 18)) {
            cout << "DEBUG: p0 (id=" << id0 << ") = [" << p0.transpose() << "]\n";
            cout << "DEBUG: p1 (id=" << id1 << ") = [" << p1.transpose() << "]\n";
        }

            area += p0.x() * p1.y() - p1.x() * p0.y();
        }

        area = fabs(area) * 0.5;

        if (area == 0.0)
            return false;
    }

    return true;
}

int main()
{
    PolygonalMesh mesh;

    if (!ImportMesh(mesh)) {
        cerr << "Error: file not found" << endl;
        return 1;
    }

    if (!TestMarkersStoredCorrectly(mesh))
    {
        cerr << "Error: markers not stored correctly." << endl;
        return 2;
    }

    if (!TestEdgesHaveNonZeroLength(mesh))
    {
        cerr << "Error: there are edges with zero length." << endl;
        return 3;
    }

    if (!TestPolygonsHaveNonZeroArea(mesh))
    {
        cerr << "Error: there are polygons with zero area" << endl;
        return 4;
    }

    Gedim::UCDUtilities utilities;
    utilities.ExportPoints("./Cell0Ds.inp",
                           mesh.Cell0DsCoordinates);

    utilities.ExportSegments("./Cell1Ds.inp",
                             mesh.Cell0DsCoordinates,
                             mesh.Cell1DsExtrema);

    return 0;
}

// ho aggiunto i debug perchè mi dava errore e volevo verificare 