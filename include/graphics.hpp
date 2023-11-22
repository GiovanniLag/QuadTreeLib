#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include "MeshGeneration.hpp"

template <typename uT, typename cT> class sim::Quadtree;

template <typename uT, typename cT>
sim::Quadtree<uT, cT>* selectNode(float x, float y, sim::Quadtree<uT, cT>* qt); // Select a node from the quadtree based on x and y coordinates

void drawPoint(const sim::Point* pt, sf::Color color, double pt_size, sf::RenderWindow& window); // Draw a point

template <typename uT, typename cT>
void drawLeafs(sim::Quadtree<uT, cT>* qt, sf::RenderWindow& window); // Draw the leafs of the quadtree

template <typename uT, typename cT>
void drawQuadtree(const sim::Quadtree<uT, cT>& qt, sf::RenderWindow& window); // Draw the quadtree

void drawMesh(const sim::Mesh& mesh, sf::RenderWindow& window); // Draw the mesh

void rwToImage(const sf::RenderWindow& window, const std::string& filename); // Save the contents of a render window to an image

template <typename uT, typename cT>
void quadtreeToImage(const sim::Quadtree<uT, cT>& qt, const std::string& filename); // Save the quadtree to an image

template <typename uT, typename cT>
void quadtreeLive(sim::Quadtree<uT, cT>& qt, sim::Mesh& mesh); // Live quadtree

template <typename uT, typename cT>
void quadtreeLive(sim::Quadtree<uT, cT>& qt) { // Live quadtree
	sim::Mesh mesh;
	quadtreeLive(qt, mesh);
}

template <typename uT, typename cT>
void drawNeighbour(sim::Quadtree<uT, cT>* neighbour, sf::RenderWindow& window, const sf::Color& color); // Helper function for drawNeighbours

template <typename uT, typename cT>
void drawNeighbours(sim::Quadtree<uT, cT>* nodes[], sf::RenderWindow& window); // Draw the neighbours of a node

template <typename uT, typename cT>
void getNeighbours(sim::Quadtree<uT, cT>* node, sim::Quadtree<uT, cT>* nodes[]); // Get the neighbours of a node (helper function for drawNeighbours)

#include "graphics_impl.tpp"

#endif GRAPHICS_HPP