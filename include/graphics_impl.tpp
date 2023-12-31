template <typename uT, typename cT>
sim::Quadtree<uT, cT>* selectNode(float x, float y, sim::Quadtree<uT, cT>* qt)
{
    sim::Point point(x, y);
    if (qt->isDivided()) {
        if (qt->getNorthWest()->getBoundary().contains(point)) {
			return selectNode(x, y, qt->getNorthWest());
		}
        else if (qt->getNorthEast()->getBoundary().contains(point)) {
			return selectNode(x, y, qt->getNorthEast());
		}
        else if (qt->getSouthWest()->getBoundary().contains(point)) {
			return selectNode(x, y, qt->getSouthWest());
		}
        else if (qt->getSouthEast()->getBoundary().contains(point)) {
			return selectNode(x, y, qt->getSouthEast());
		}
	}
    std::cout << "Selected node (x,y): " << qt->getBoundary().topLeft.x << ", " << qt->getBoundary().topLeft.y << std::endl;
    return qt;
}

template <typename uT, typename cT>
void drawLeafs(sim::Quadtree<uT, cT>* qt, sf::RenderWindow& window)
{
    std::queue<sim::Quadtree<uT, cT>*> leafs;
    qt->getLeafs(&leafs);
    // Draw the boundary in red
    while (!leafs.empty()) {
        auto leaf = leafs.front();
        leafs.pop();
        sf::RectangleShape rectangle(sf::Vector2f(leaf->getBoundary().getWidth(), leaf->getBoundary().getHeight()));
        // Make retangle 2 pixels smaller in each direction to make it visible
        rectangle.setSize(sf::Vector2f(leaf->getBoundary().getWidth() - 8, leaf->getBoundary().getHeight() - 8));
        // Set color based on depth of the leaf
        rectangle.setOutlineColor(sf::Color(255-leaf->getDepth()*30, 0, 255+leaf->getDepth()*35, 255));
        rectangle.setOutlineThickness(2);
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setPosition(leaf->getBoundary().topLeft.x+4, leaf->getBoundary().topLeft.y+4);
        window.draw(rectangle);
    }
}

void drawPoint(const sim::Point* pt, sf::Color color, double pt_size, sf::RenderWindow& window)
{
	sf::CircleShape circle(pt_size);
	circle.setFillColor(color);
    circle.setOrigin(pt_size, pt_size);
	circle.setPosition(pt->x, pt->y);
	window.draw(circle);
}

template <typename uT, typename cT>
void drawQuadtree(sim::Quadtree<uT, cT>& qt, sf::RenderWindow& window) {
    // Draw the boundary
    sf::RectangleShape rectangle(sf::Vector2f(qt.getBoundary().getWidth(), qt.getBoundary().getHeight()));
    rectangle.setOutlineColor(sf::Color::Green);
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setPosition(qt.getBoundary().topLeft.x, qt.getBoundary().topLeft.y);
    window.draw(rectangle);

    // Draw points
    for (const auto point : qt.getPoints()) {
        drawPoint(&point, sf::Color::White, 2, window);
    }

    // Draw child quadtrees
    if (qt.isDivided()) {
        drawQuadtree(*qt.getNorthWest(), window);
        drawQuadtree(*qt.getNorthEast(), window);
        drawQuadtree(*qt.getSouthWest(), window);
        drawQuadtree(*qt.getSouthEast(), window);
    }
}

void drawMesh(const sim::Mesh& mesh, sf::RenderWindow& window)
{
    // Just draw the links in mesh as white lines
    for (const auto& link : mesh.links) {
		sf::Vertex line[] =
        {
			sf::Vertex(sf::Vector2f(link.p1->x, link.p1->y)),
			sf::Vertex(sf::Vector2f(link.p2->x, link.p2->y))
		};
		window.draw(line, 2, sf::Lines);
	}
}

void rwToImage(const sf::RenderWindow& window, const std::string& filename) 
{
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    sf::Image screenshot = texture.copyToImage();
    screenshot.saveToFile(filename);
}

template <typename uT, typename cT>
void quadtreeToImage(sim::Quadtree<uT, cT>& qt, const std::string& filename) {
    sf::RenderWindow window(sf::VideoMode(qt.getBoundary().getWidth(), qt.getBoundary().getHeight()), "Quadtree");
    window.clear(sf::Color::Black);
    drawQuadtree(qt, window);
    rwToImage(window, filename);
}

template <typename uT, typename cT>
void quadtreeLive(sim::Quadtree<uT, cT>& qt, sim::Mesh& mesh) {
    sf::RenderWindow window(sf::VideoMode(qt.getBoundary().getWidth(), qt.getBoundary().getHeight()), "Quadtree");
    window.setVerticalSyncEnabled(true); // Ensure rendering is synced with the display's refresh rate to avoid tearing

    bool shouldDrawLeafs = false;
    bool shouldDrawNeighbours = false;
    bool shouldDrawMesh = false;
    bool shouldDrawQuadtree = true;
    sim::Quadtree<uT, cT>* nodeForNeighbours = nullptr;
    sim::Quadtree<uT, cT>* neighboursAndNodeList[5];
    bool selectionMode = false;
    int clickCount = 0;
    sf::Vector2f topLeft, bottomRight;
    sf::RectangleShape selectionRectangle;
    std::vector<sim::Point*> selectedPoints;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle left mouse button click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !selectionMode) {
                float x = static_cast<float>(event.mouseButton.x);
                float y = static_cast<float>(event.mouseButton.y);
                qt.insert(sim::Point(x, y)); // Insert the point into the Quadtree
            }

            // Handle "B" key press to balance the Quadtree
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
                qt.balance();
                std::cout << "Quadtree balanced" << std::endl;
            }

            // Handle "N" key press to toggle the drawing of neighbours
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
                shouldDrawNeighbours = !shouldDrawNeighbours;
                if (shouldDrawNeighbours) {
                    // Select the node under the current mouse position for neighbour drawing
                    float x = static_cast<float>(sf::Mouse::getPosition(window).x);
                    float y = static_cast<float>(sf::Mouse::getPosition(window).y);
                    nodeForNeighbours = selectNode(x, y, &qt);
                    getNeighbours(nodeForNeighbours, neighboursAndNodeList);
                }
            }

            // Handle "L" key press to toggle the drawing of leafs
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                shouldDrawLeafs = !shouldDrawLeafs;
            }

            // Handle "M" key press to toggle the drawing of the mesh
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
				shouldDrawMesh = !shouldDrawMesh;
			}
            // CTRL + M to regenerate the mesh
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                mesh = sim::generateMesh2(&qt);
                std::cout << "Mesh regenerated" << std::endl;
			}

            // Handle "Q" key press to toggle the drawing of the Quadtree
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                shouldDrawQuadtree = !shouldDrawQuadtree;
			}

            // Toggle selection mode on "P" key press
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                std::cout << "Selection mode " << (selectionMode ? "off" : "on") << std::endl;
                selectionMode = !selectionMode;
                clickCount = 0; // Reset click count
                // Reset selection rectangle
                selectionRectangle.setSize(sf::Vector2f(0, 0));
            }

            // Handle "S" key press to save the Quadtree as an image
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
				rwToImage(window, "quadtree_screenshot.png");
				std::cout << "Quadtree saved as image" << std::endl;
			}

            // Handle mouse clicks for selection
            if (selectionMode && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (clickCount == 0) {
                    // First click - set top-left corner
                    topLeft.x = static_cast<float>(event.mouseButton.x);
                    topLeft.y = static_cast<float>(event.mouseButton.y);
                    clickCount++;
                }
                else if (clickCount == 1) {
                    // Second click - set bottom-right corner and finalize the rectangle
                    bottomRight.x = static_cast<float>(event.mouseButton.x);
                    bottomRight.y = static_cast<float>(event.mouseButton.y);

                    // Safty check to ensure the rectangle is not inverted
                    if (bottomRight.x < topLeft.x) {
						float temp = topLeft.x;
						topLeft.x = bottomRight.x;
						bottomRight.x = temp;
					}
                    if (bottomRight.y < topLeft.y) {
                        float temp = topLeft.y;
                        topLeft.y = bottomRight.y;
                        bottomRight.y = temp;
                    }

                    selectionRectangle.setPosition(topLeft);

                    sf::Vector2f size(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
                    selectionRectangle.setSize(size);
                    selectionRectangle.setFillColor(sf::Color::Transparent);
                    selectionRectangle.setOutlineColor(sf::Color::Yellow);
                    selectionRectangle.setOutlineThickness(1);

                    // Query points in the selected region
                    sf::FloatRect rect(topLeft, size);
                    sim::BoundingBox rectBBox = sim::BoundingBox(sim::Point(rect.left, rect.top), sim::Point(rect.left + rect.width, rect.top + rect.height));
                    selectedPoints = qt.queryRange(rectBBox);
                    std::cout << "Points in region: " << selectedPoints.size() << std::endl;

                    clickCount = 0; // Reset click count
                }
            }
        }

        window.clear(sf::Color::Black); // Clear the window

        if (shouldDrawQuadtree) {
        drawQuadtree(qt, window); // Draw the entire Quadtree
        }

        // If "L" key has been pressed, draw the leaf nodes
        if (shouldDrawLeafs) {
            drawLeafs(&qt, window);
        }

        // If "N" key has been pressed, draw the neighbours of the selected node
        if (shouldDrawNeighbours && nodeForNeighbours) {
            drawNeighbours(neighboursAndNodeList, window);
        }

        // If "M" key has been pressed, draw the mesh
        if (shouldDrawMesh) {
			drawMesh(mesh, window);
		}

        // Draw selection rectangle if in selection mode
        if (selectionMode) {
            window.draw(selectionRectangle);
            // Hilight selected points
            for (const sim::Point* p : selectedPoints) {
                drawPoint(p, sf::Color::Yellow, 4, window);
            }
        }

        window.display(); // Display everything we have drawn
    }
}


// Hilight the neighbours of a node
template <typename uT, typename cT>
void drawNeighbour(sim::Quadtree<uT, cT>* neighbour, sf::RenderWindow& window, const sf::Color& color) {
    if (neighbour) {
        auto boundary = neighbour->getBoundary();
        sf::RectangleShape rectangle(sf::Vector2f(boundary.getWidth(), boundary.getHeight())); // Reduced size
        rectangle.setOutlineColor(color);
        rectangle.setOutlineThickness(5);
        //fill color is same as outline color but with 50% opacity
        sf::Color fillColor = color;
        fillColor.a = 128;
        rectangle.setFillColor(fillColor);
        rectangle.setPosition(boundary.topLeft.x + 4, boundary.topLeft.y + 4); // Moved position inwards
        rectangle.setSize(sf::Vector2f(boundary.getWidth() - 8, boundary.getHeight() - 8));
        window.draw(rectangle);
    }
}

template <typename uT, typename cT>
void drawNeighbours(sim::Quadtree<uT, cT>* nodes[], sf::RenderWindow& window) {
    drawNeighbour(nodes[1], window, sf::Color::Blue); // North
    drawNeighbour(nodes[2], window, sf::Color::Yellow); // East
    drawNeighbour(nodes[3], window, sf::Color::Cyan); // South
    drawNeighbour(nodes[4], window, sf::Color::Magenta); // West
    // Highlight the node itself in white
    drawNeighbour(nodes[0], window, sf::Color::White);
}

template <typename uT, typename cT>
void getNeighbours(sim::Quadtree<uT, cT>* node, sim::Quadtree<uT, cT>* nodes[]) {
	if (node == nullptr) return;
	nodes[0] = node;
	nodes[1] = node->getNorthNeighbour();
	nodes[2] = node->getEastNeighbour();
	nodes[3] = node->getSouthNeighbour();
	nodes[4] = node->getWestNeighbour();
}