#include <iostream>
#include <limits>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

// Min time needed
double minTime = numeric_limits<double>::max();

//Node: the edges of each match, has x,y coordinates, keeps track if it is burned,
// isNew is for if it is newly burned during the current round. Each node has a vector of nodes that are
// connected to the node.
struct Node {
    double X{};
    double Y{};
    bool isBurnt{};
    bool isNew{};
    vector<Node> nodesConnected;
};

//Match: keeps the index of both nodes(edges) of the match, you save the amount of time needed to burn
// for each match, the amount of time already burned, a bool that is used to keep track if the cur match has already
//burned this round and an bool to keep track if the current match is a diagonal match or a regular match
struct Match {
    int node1Index;
    int node2Index;
    double timeNeededToBurn;
    double timeAlreadyBurnt;
    bool hasBurnedDuringRound;
    bool isDiagonal;
};

//returns the double that is smaller between two doubles
double min(double x, double x1) {
    if (x < x1)
        return x;
    else
        return x1;
}

//implementation of geometry alogrithm for finding if two lines intersect
// link to website: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
int orientation(const Node &p, const Node &q, const Node &r) {
    double val = (q.Y - p.Y) * (r.X - q.X) -
                 (q.X - p.X) * (r.Y - q.Y);

    if (val == 0) return 0;  // colinear

    return (val > 0) ? 1 : 2; // clock or counterclock wise
}

//implementation of geometry alogrithm for finding if two lines intersect
// link to website: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool doIntersect(const Node &p1, const Node &q1, const Node &p2, const Node &q2) {
    // Find the four orientations needed for general cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    return o1 != o2 && o3 != o4;

}

//Method that checks if by the given cooridinates of a match, does it overlap with any of the matches already
// saved to the figure
bool checkIfMatchOverlaps(double x1, double y1, double x2, double y2, const vector<Match> &figure, vector<Node> nodes) {
    for (Match match: figure) {
        if (nodes[match.node1Index].X == x1 && nodes[match.node1Index].Y == y1 && nodes[match.node2Index].X == x2 &&
            nodes[match.node2Index].Y == y2) {
            return true;
        }
        if (nodes[match.node2Index].X == x1 && nodes[match.node2Index].Y == y1 && nodes[match.node1Index].X == x2 &&
            nodes[match.node1Index].Y == y2) {
            return true;
        }
    }
    return false;
}

//Finds the index of a node in with the given coordinates in a vector
int findNodeIndex(double x, double y, const vector<Node> &nodes) {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].X == x && nodes[i].Y == y) {
            return i;
        }
    }
    return -1;
}

//Finds the index of a match in with the given node indexes in a vector
int findMatchIndex(int node1Index, int node2Index, vector<Match> figure) {
    for (int i = 0; i < figure.size(); i++) {
        if (figure[i].node1Index == node1Index && figure[i].node2Index == node2Index)
            return i;
    }
    return -1;
}

//Method that sets the minimal time for this round
void findMinTime(const vector<Match> &matchList, vector<Node> nodes) {
    for (Match m : matchList) {
        //checks if match has already burned out (m.timeNeededToBurn == m.timeAlreadyBurnt)
        //if it has it skips the match
        if (m.timeNeededToBurn > m.timeAlreadyBurnt) {
            //checks if one or both ends of the match is burned
            //If both ends are burned, the time to burn the rest of the
            //match is cut in half
            //sets it to minimal time if the calculated time is less than the current minimal time
            if (nodes[m.node1Index].isBurnt && nodes[m.node2Index].isBurnt) {
                if (((m.timeNeededToBurn - m.timeAlreadyBurnt) / 2) < minTime) {
                    minTime = (m.timeNeededToBurn - m.timeAlreadyBurnt) / 2;
                }
            } else {
                if ((m.timeNeededToBurn - m.timeAlreadyBurnt) < minTime) {
                    minTime = m.timeNeededToBurn - m.timeAlreadyBurnt;
                }
            }
        }
    }
}

//Creates a match list which contains all matches connected to a node.
void initializeMatchList(const vector<Match> &figure, const Node &startingNode, vector<Match> *matchList,
                         vector<Node> nodes) {
    Node node1, node2;
    for (Match match: figure) {
        node1 = nodes[match.node1Index];
        node2 = nodes[match.node2Index];
        if ((startingNode.X == node1.X && startingNode.Y == node1.Y) ||
            (startingNode.X == node2.X && startingNode.Y == node2.Y)) {
            matchList->push_back(match);
        }
    }
}

//calls methods for first steps of a round
vector<Match> step(const vector<Match> &figure, Node *pStartingNode, const vector<Node> &nodes) {
    //creating staring node and match List objects
    Node startingNode = *pStartingNode;
    vector<Match> matchList;
    //calling method for finding min time and initalizing match list
    initializeMatchList(figure, startingNode, &matchList, nodes);
    findMinTime(matchList, nodes);

    //returns match list
    return matchList;
}

//fills node vector map
void initalizeNodeVectorMap(vector<Node *> *burningNodes, map<Node *, vector<Match >> *nodeVectorMap,
                            const vector<Match> &figure, const vector<Node> &nodes) {
    for (Node *node: *burningNodes) {
        //each node that is in burning node vector, the node is the key, and a vector of all the connected matches
        // to the node is the value
        nodeVectorMap->insert({node, step(figure, node, nodes)});
    }
}

//resets objects for a new iteration
void resetingForBegOFCycle(vector<Match> *figure, vector<Node> *nodes) {
    //min time is largest value for double
    minTime = numeric_limits<double>::max();

    //all matches in the figure will be reset to have not burned during round
    for (auto &i : *figure) {
        i.hasBurnedDuringRound = false;
    }
    //all nodes in the nodes vector will be reset to have not burned during round
    for (auto &i : *nodes) {
        i.isNew = false;
    }
}

//method used for during the creation of the source code to debugging because my debugger doesnt work
void printFigure(vector<Match> figure, double totalTime) {
    cout << "--------------------" << endl;
    cout << "+ min time: " << minTime << endl;
    for (int i = 0; i < figure.size(); i++) {
        cout << "fig [" << i << "]: " << figure[i].timeAlreadyBurnt << " " << figure[i].node1Index
             << figure[i].node2Index
             << endl;
    }
    cout << "total time: " << totalTime << endl;
}

//method used for checking if all the matches have completely burned (match.timeNeededToBurn == match.timeAlreadyBurnt)
bool areAllMatchesBurnt(const vector<Match> &figure) {
    for (Match match : figure) {
        if (match.timeNeededToBurn != match.timeAlreadyBurnt) {
            return false;
        }
    }
    return true;
}

//see if the nodes vector contains a node by the given node
bool seeIfContainsNode(const vector<Node> &nodes, const Node &node) {
    double X, Y;
    for (const Node &n: nodes) {
        X = node.X;
        Y = node.Y;

        if (X == n.X && Y == n.Y) {

            return true;
        }
    }
    return false;
}

//see if the nodes vector contains a node by the given node coordinates
bool seeIfContainsNode(const vector<Node> &nodes, double x, double y) {
    for (const Node &n: nodes) {
        if (x == n.X && y == n.Y) {
            return true;
        }
    }
    return false;
}
//see if the nodes vector of type Node* contains a node by the given node
bool seeIfContainsNode(const vector<Node *> &nodes, const Node &node) {
    double X, Y;
    for (const Node *n : nodes) {
        X = node.X;
        Y = node.Y;
        if (X == n->X && Y == n->Y) {
            return true;
        }
    }
    return false;
}

//Initalized nodes vector by adding nodes that dont exist
void addIfDoesntNodeExists(double x, double y, vector<Node> *nodes) {
    if (!seeIfContainsNode(*nodes, x, y)) {
        nodes->push_back({x, y, false, false});
    }
}

//fills nodes connected vector for each node by checking each match if one of its nodes is
//one of the nodes at the edge of the map
void fillNodesConnected(vector<Node> *nodes, const vector<Match> &figure) {
    for (auto &node : *nodes) {
        for (const Match &match: figure) {
            // skips match if the one of the nodes already exists in this list
            if (seeIfContainsNode(node.nodesConnected, nodes->at(match.node1Index)) ||
                seeIfContainsNode(node.nodesConnected, nodes->at(match.node2Index))) {
                continue;
            }
            //adds the node if the coorinates of the node match those at of node with node1Index
            if (node.X == nodes->at(match.node1Index).X && node.Y == nodes->at(match.node1Index).Y) {
                node.nodesConnected.push_back(nodes->at(match.node2Index));
                continue;
            }
            //adds the node if the coorinates of the node match those at of node with node2Index
            if (node.X == nodes->at(match.node2Index).X && node.Y == nodes->at(match.node2Index).Y) {
                node.nodesConnected.push_back(nodes->at(match.node1Index));
            }
        }
    }
}

//burns nodes and marks them as pernamently burned and marks them as new if freshly burned
void burnNodes(vector<Match> *figure, vector<Node *> *burningNodes, vector<Node> *nodes) {
    for (auto &m : *figure) {
        if (m.timeAlreadyBurnt == m.timeNeededToBurn) {
            if (!(nodes->at(m.node1Index).isBurnt)) {
                nodes->at(m.node1Index).isNew = true;
            }
            nodes->at(m.node1Index).isBurnt = true;
            if (!nodes->at(m.node2Index).isBurnt) {
                nodes->at(m.node2Index).isNew = true;
            }
            nodes->at(m.node2Index).isBurnt = true;


            //check if there is that node in the list and add it to a vector of all pernamently burned nodes
            if (!seeIfContainsNode(*burningNodes, nodes->at(m.node1Index)))
                burningNodes->push_back(&nodes->at(m.node1Index));
            if (!seeIfContainsNode(*burningNodes, nodes->at(m.node2Index)))
                burningNodes->push_back(&nodes->at(m.node2Index));
        }
    }
}

//resets all properties of the figure and nodes that should be reset
void resetFigureAndNodes(vector<Match> *figure, vector<Node> *nodes) {
    for (auto &l : *figure) {
        l.timeAlreadyBurnt = 0;
    }
    for (auto &l : *nodes) {
        l.isBurnt = false;
        l.isNew = false;
    }
}

//Burns the matches of the declared minimal time for the round
void burnMatches(const vector<Match> &matchList, vector<Match> *figure, vector<Node> nodes) {
    for (auto &i : *figure) {
        for (Match match: matchList) {
            //skips match if fully burned
            if (i.timeAlreadyBurnt == i.timeNeededToBurn) {
                continue;
            }
            //finds the match from match list in figure and checks if the match has already burned this round to not
            //burn a second time
            if ((nodes[match.node1Index].X == nodes[i.node1Index].X &&
                 nodes[match.node1Index].Y == nodes[i.node1Index].Y)
                && (nodes[match.node2Index].X == nodes[i.node2Index].X &&
                    nodes[match.node2Index].Y == nodes[i.node2Index].Y)
                && !i.hasBurnedDuringRound) {

                //checks if nodes are newly burned from this round to check if to burn the match from both sides
                if (nodes[i.node1Index].isNew || nodes[i.node2Index].isNew) {
                    i.timeAlreadyBurnt += minTime;
                    i.hasBurnedDuringRound = true;
                    continue;
                }
                //burns match from both sides
                if (nodes[i.node1Index].isBurnt && nodes[i.node2Index].isBurnt) {
                    i.timeAlreadyBurnt += (2 * minTime);
                    i.hasBurnedDuringRound = true;
                    continue;
                }
                //burns regularly
                i.timeAlreadyBurnt += minTime;
                i.hasBurnedDuringRound = true;
            } else {
                continue;
            }
        }
    }
}

//calls to burn matches and nodes in node vector map
void burnMatchesAndNodesInMap(vector<Node *> *burningNodes, map<Node *, vector<Match >> *nodeVectorMap,
                              vector<Match> *figure, vector<Node> *nodes) {
    for (auto &nvm: *nodeVectorMap) {
        burnMatches(nvm.second, figure, *nodes);
        burnNodes(figure, burningNodes, nodes);
    }
}

//calls a method for finding index of a match and erases it
void eraseMatch(vector<Match> *figure, int nodeIndex1, int nodeIndex2) {
    int index;
    index = findMatchIndex(nodeIndex1, nodeIndex2, *figure);
    figure->erase(figure->begin() + index);
}

//initalzies figure vectoe and nodes vector
void initializeFigureAndNodes(vector<Match> *figure, vector<Node> *nodes) {
    int numberOfMatches;
    vector<double> splitLine;
    string line;
    double x1, x2, y1, y2, time;
    bool isDiagonal = true;

    //sees how many times to add match to figure and nodes
    cout << "Kolko kebriteni klechki shte izpolzvate?" << endl;
    getline(cin, line);
    numberOfMatches = std::stoi(line);

    for (int a = 0; a < numberOfMatches; a++) {
        //enter data for match
        cout << "Vuvedi Klechka: " << endl;
        cin >> x1 >> y1 >> x2 >> y2 >> time;

        //see if match overlaps with enterd data
        if (checkIfMatchOverlaps(x1, y1, x2, y2, *figure, *nodes)) {
            cout << "Match overlaps " << endl;
            exit(-1);
        }
        //checks if match is a diagonal
        if (x1 == x2 || y1 == y2) {
            isDiagonal = false;
        }
        //adds to nodes if nodes dont exist by coordinates
        addIfDoesntNodeExists(x1, y1, nodes);
        addIfDoesntNodeExists(x2, y2, nodes);

        //finds both node indexes
        int nodeIndex1 = findNodeIndex(x1, y1, *nodes);
        int nodeIndex2 = findNodeIndex(x2, y2, *nodes);

        //adds match
        figure->push_back({nodeIndex1, nodeIndex2, time, 0, false, isDiagonal});

        //sets isDiagonal to our default value
        isDiagonal = true;
    }
}

//returns true or false for whether to skip depending on the ifs
bool ifSkipMatch(vector<Node> nodes, Match match, Match match1) {
    double x1, x2, y1, y2, x3, x4, y3, y4;

    //is the same match
    if (nodes[match.node1Index].X == nodes[match1.node1Index].X &&
        nodes[match.node1Index].Y == nodes[match1.node1Index].Y &&
        nodes[match.node2Index].X == nodes[match1.node2Index].X &&
        nodes[match.node2Index].Y == nodes[match1.node2Index].Y) {
        return true;
    }

    //is the same match
    if (nodes[match.node2Index].X == nodes[match1.node2Index].X &&
        nodes[match.node2Index].Y == nodes[match1.node2Index].Y &&
        nodes[match.node1Index].X == nodes[match1.node1Index].X &&
        nodes[match.node1Index].Y == nodes[match1.node1Index].Y) {
        return true;
    }

    x1 = nodes[match.node1Index].X;
    x2 = nodes[match.node2Index].X;
    x3 = nodes[match1.node1Index].X;
    x4 = nodes[match1.node2Index].X;
    y1 = nodes[match.node1Index].Y;
    y2 = nodes[match.node2Index].Y;
    y3 = nodes[match1.node1Index].Y;
    y4 = nodes[match1.node2Index].Y;

    //if diagonals intersect in one square with the range of  1 x 1
    return !(((x1 == x3 && y1 == y4 && (x1 - x4 == 1 || x1 - x4 == -1) && (y1 - y3 == 1 || y1 - y3 == -1)) ||
              (x1 == x4 && y1 == y3 && (x1 - x3 == 1 || x1 - x3 == -1) && (y1 - y4 == 1 || y1 - y4 == -1))) &&
             ((x2 == x3 && y2 == y4 && (x2 - x4 == 1 || x2 - x4 == -1) && (y2 - y3 == 1 || y2 - y3 == -1)) ||
              (x2 == x4 && y2 == y3 && (x2 - x3 == 1 || x2 - x3 == -1) && (y2 - y4 == 1 || y2 - y4 == -1))));
}

//fixes figure depending on how diagonals intesect
void remakeFigureIfItHasDioganals(vector<Match> *figure, vector<Node> *nodes) {
    double x, y;
    int nodeIndex;
    //creates a copy of figure
    vector<Match> copyFigure = *figure;

    //uses copy to iterate and makes changes in figure
    for (Match &match: copyFigure) {
        for (Match &match1 : copyFigure) {
            //checks whether to skip current match
            if (ifSkipMatch(*nodes, match, match1))
                continue;

            //checks if they intersect
            if (doIntersect(nodes->at(match.node1Index), nodes->at(match.node2Index), nodes->at(match1.node1Index),
                            nodes->at(match1.node2Index)) && match.isDiagonal && match1.isDiagonal) {
                //sets the s and y of the min beween both nodes +0.5
                x = min(nodes->at(match.node1Index).X, nodes->at(match.node2Index).X);
                x = x + 0.5;
                y = min(nodes->at(match.node1Index).Y, nodes->at(match.node2Index).Y);
                y = y + 0.5;

                //sees if there is already an nodes with values like these in nodes
                if (!seeIfContainsNode(*nodes, x, y)) {

                    //adds if not
                    nodes->push_back({x, y, false, false});

                    //finds node index
                    nodeIndex = findNodeIndex(x, y, *nodes);

                    //printFigure(*figure, -3);

                    //from 2 matches makes 4 mini matches with half of the time needed to burn
                    figure->push_back({match.node1Index, nodeIndex, match.timeNeededToBurn / 2, 0, false});
                    figure->push_back({match.node2Index, nodeIndex, match.timeNeededToBurn / 2, 0, false});
                    figure->push_back({match1.node1Index, nodeIndex, match1.timeNeededToBurn / 2, 0, false});
                    figure->push_back({match1.node2Index, nodeIndex, match1.timeNeededToBurn / 2, 0, false});

                    //printFigure(*figure, -2);

                    //erased old matches
                    eraseMatch(figure, match.node1Index, match.node2Index);
                    eraseMatch(figure, match1.node1Index, match1.node2Index);
                }
                // printFigure(*figure, -1);
            }
        }
    }
}

// main function
int main() {
    vector<Match> figure;
    vector<Node> nodes;
    map<Node *, double> nodeResultMap;

    //initalize vectors
    initializeFigureAndNodes(&figure, &nodes);

    //remake figure
    remakeFigureIfItHasDioganals(&figure, &nodes);
    int i = 0;

/*    for(const Node& node: nodes){
        cout <<i<< " " << node.X << " "<< node.Y << endl;
        i++;
    }*/

//finds min time for each node
//divides in two main steps
//first step from the starting node and first initalization for buning nodes vector
//second step from the rest  by constanly iterating burned nodes vector
    for (int k = 0; k < nodes.size(); k++) {
        //total time to burn from this starting point
        double totalTime = 0;
        //temp list of connected matches for first step
        vector<Match> tempMatchList;
        //vector of burning nodes
        vector<Node *> burningNodes;
        //list of connected matches for each node
        map<Node *, vector<Match >> nodeVectorMap;
        //starting point
        Node *startingNode = &nodes[k];

        //checks if starting node is a whole number
        if (floor(nodes[k].X) != nodes[k].X)
            continue;
        if (floor(nodes[k].Y) != nodes[k].Y)
            continue;

        //reseting stuff
        resetingForBegOFCycle(&figure, &nodes);
        resetFigureAndNodes(&figure, &nodes);
        minTime = numeric_limits<double>::max();

        //printing figure
        //printFigure(figure, totalTime);

        //connecting nodes
        fillNodesConnected(&nodes, figure);

        //burn starting node
        burningNodes.push_back(startingNode);
        startingNode->isBurnt = true;

        //starting burning from starting node
        tempMatchList = step(figure,
                             startingNode,
                             nodes); //-> finds min Time of next step returns matches connected to 'startingNode'
        totalTime = totalTime + minTime;

        //burns part of matches
        burnMatches(tempMatchList, &figure, nodes);

        //burns nodes
        burnNodes(&figure, &burningNodes, &nodes);

        //printFigure(figure, totalTime);

        while (!areAllMatchesBurnt(figure)) {
            //checks if further iteration is nessary because the figure is not connected and can therefore not burn
            if (totalTime >= numeric_limits<double>::max()) {
                cout << "Figurata ne e svurzana!" << endl;
                return -1;
            }
            //reseting for beginning of cycle
            resetingForBegOFCycle(&figure, &nodes);

            //initalizing node vector map
            initalizeNodeVectorMap(&burningNodes, &nodeVectorMap, figure, nodes);

            //burns matches and nodes
            burnMatchesAndNodesInMap(&burningNodes, &nodeVectorMap, &figure, &nodes);

            //adds min time to total time
            totalTime += minTime;

            //clears node vector map
            nodeVectorMap.clear();

            //printFigure(figure, totalTime);
        }
        //adds to result map to say from what node how much was the total time.
        nodeResultMap.insert({&nodes.at(k), totalTime});
    }
   // double minResult = numeric_limits<double>::max();
   //prints result map with the coordinates of the node and the time needed to burn the figure from that starting point
    for (auto &nrm: nodeResultMap) {
        cout << "Node (x,y): (" << nrm.first->X << ", " << nrm.first->Y << ") -> " << nrm.second << " " << endl;
        /*if (minResult > nrm.second)
            minResult = nrm.second;*/
    }
    //cout << "Min result: " << minResult;
    return 0;
}
