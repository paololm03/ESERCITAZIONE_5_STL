#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace PolygonalLibrary
{
bool ImportMesh(PolygonalMesh& mesh)
{
    if(!ImportCell0Ds(mesh))
        return false;

    if(!ImportCell1Ds(mesh))
        return false;

    if(!ImportCell2Ds(mesh))
        return false;

    return true;
}
// ***************************************************************************
bool ImportCell0Ds(PolygonalMesh& mesh)
{
    std::ifstream file("Cell0Ds.csv");
    if (!file.is_open())
    {
        std::cerr << "Impossibile aprire il file Cell0Ds.csv" << std::endl;
        return false;
    }

    std::vector<std::string> listLines;
    std::string line;

    std::getline(file, line); // salta intestazione

    while (std::getline(file, line))
        listLines.push_back(line);

    file.close();

    mesh.NumCell0Ds = listLines.size();
    mesh.Cell0DsCoordinates = MatrixXd::Zero(3, mesh.NumCell0Ds);

    unsigned int i = 0;
    for (const std::string& line : listLines)
    {
        std::istringstream converter(line);
        unsigned int id, marker;
        double x, y;

        char delimiter;
        converter >> id >> delimiter >> marker >> delimiter >> x >> delimiter >> y;

        mesh.Cell0DsId.push_back(id);
        mesh.Cell0DMarkers[marker].push_back(id);
        mesh.Cell0DIdToIndex[id] = i;
        mesh.Cell0DsCoordinates(0, i) = x;
        mesh.Cell0DsCoordinates(1, i) = y;
        mesh.Cell0DsCoordinates(2, i) = 0;

        ++i;
    }

    return true;
}

// ***************************************************************************
bool ImportCell1Ds(PolygonalMesh& mesh)
{
    std::ifstream file("./Cell1Ds.csv");

    if (file.fail())
        return false;

    std::list<std::string> listLines;
    std::string line;
    while (std::getline(file, line))
        listLines.push_back(line);

    file.close();
    listLines.pop_front();  // remove header

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        std::cerr << "There is no cell 1D" << std::endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    unsigned int i = 0;
    for (const std::string& line : listLines)
    {
        std::string temp = line;
        std::replace(temp.begin(), temp.end(), ';', ' ');
        std::istringstream converter(temp);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;

        converter >> id >> marker >> vertices[0] >> vertices[1];
        mesh.Cell1DsId.push_back(id);
        mesh.Cell1DsExtrema(0, i) = vertices[0];
        mesh.Cell1DsExtrema(1, i) = vertices[1];

        mesh.Cell1DMarkers[marker].push_back(id);
        ++i;
    }

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(PolygonalMesh& mesh)
{
    std::ifstream file("./Cell2Ds.csv");

    if (file.fail())
        return false;

    std::list<std::string> listLines;
    std::string line;
    while (std::getline(file, line))
        listLines.push_back(line);

    file.close();
    if (!listLines.empty())
        listLines.pop_front();  // remove header

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        std::cerr << "There is no cell 2D" << std::endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.reserve(mesh.NumCell2Ds);

    for (const std::string& line : listLines)
    {
        std::string temp = line;
        std::replace(temp.begin(), temp.end(), ';', ' ');
        std::istringstream stream(temp);

        unsigned int id, marker, numVertices, numEdges;
        stream >> id >> marker >> numVertices;

        std::vector<unsigned int> vertices(numVertices);
        for (unsigned int i = 0; i < numVertices; ++i)
            stream >> vertices[i];

        stream >> numEdges;
        std::vector<unsigned int> edges(numEdges);
        for (unsigned int i = 0; i < numEdges; ++i)
            stream >> edges[i];

        mesh.Cell2DsId.push_back(id);
        mesh.Cell2DsVertices.push_back(vertices);
        mesh.Cell2DsEdges.push_back(edges);

        mesh.Cell2DMarkers[marker].push_back(id);
    }

    return true;
}

}