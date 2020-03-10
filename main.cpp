#include <iostream>
#include <limits>
#include <vector>
#include <map>
#include <string>
#include <sstream>

using namespace std;

// Number of nodes
double minTime = numeric_limits<double>::max();

struct Node {
    double X{};
    double Y{};
    bool isBurnt{};
    bool isNew{};
    vector<Node> nodesConnected;
};

struct Match {
    int node1Index;
    int node2Index;
    double timeNeededToBurn;
    double timeAlreadyBurnt;
    bool hasBurnedDuringRound;
    bool isNotDiagonal;
};

bool checkIfMatchOverlaps(double x1, double y1, double x2, double y2, const vector<Match> &figure, vector<Node> nodes) {
    for (Match match: figure) {
        if (nodes[match.node1Index].X == x1 && nodes[match.node1Index].Y == y1 && nodes[match.node2Index].X == x2 &&
            nodes[match.node2Index].Y == y2) {
            return true;
        }
    }
    return false;
}

int findNodeIndex(double x, double y, const vector<Node> &nodes) {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].X == x && nodes[i].Y == y) {
            return i;
        }
    }
    return -1;
}

void findMinTime(const vector<Match> &matchList, vector<Node> nodes) {
    for (Match m : matchList) {
        if (m.timeNeededToBurn > m.timeAlreadyBurnt) {

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

vector<Match> step(const vector<Match> &figure, Node *pStartingNode, const vector<Node> &nodes) {
    Node startingNode = *pStartingNode;
    vector<Match> matchList;

    initializeMatchList(figure, startingNode, &matchList, nodes);
    findMinTime(matchList, nodes);

    return matchList;
}

void initalizeNodeVectorMap(vector<Node *> *burningNodes, map<Node *, vector<Match >> *nodeVectorMap,
                            const vector<Match> &figure, const vector<Node> &nodes) {
    for (Node *node: *burningNodes) {
        nodeVectorMap->insert({node, step(figure, node, nodes)});
    }
}

void resetingForBegOFCycle(vector<Match> *figure, vector<Node> *nodes) {
    minTime = numeric_limits<double>::max();

    for (auto &i : *figure) {
        i.hasBurnedDuringRound = false;
    }
    for (auto &i : *nodes) {
        i.isNew = false;
    }
}

void printFigure(vector<Match> figure, double totalTime) {
    cout << "--------------------" << endl;
    cout << "+ min time: " << minTime << endl;
    for (int i = 0; i < figure.size(); i++) {
        cout << "fig [" << i << "]: " << figure[i].timeAlreadyBurnt
             << endl;
    }
    cout << "total time: " << totalTime << endl;
}

bool areAllMatchesBurnt(const vector<Match> &figure) {
    for (Match match : figure) {
        if (match.timeNeededToBurn != match.timeAlreadyBurnt) {
            return false;
        }
    }
    return true;
}

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

bool seeIfContainsNode(const vector<Node> &nodes, double x, double y) {
    for (const Node &n: nodes) {
        if (x == n.X && y == n.Y) {
            return true;
        }
    }
    return false;
}

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

void addIfDoesntNodeExists(double x, double y, vector<Node> *nodes) {
    if (!seeIfContainsNode(*nodes, x, y)) {
        nodes->push_back({x, y, false, false});
    }
}

void fillNodesConnected(vector<Node> *nodes, const vector<Match> &figure) {
    for (auto &node : *nodes) {
        for (const Match &match: figure) {
            if (seeIfContainsNode(node.nodesConnected, nodes->at(match.node1Index)) ||
                seeIfContainsNode(node.nodesConnected, nodes->at(match.node2Index))) {
                continue;
            }
            if (node.X == nodes->at(match.node1Index).X && node.Y == nodes->at(match.node1Index).Y) {
                node.nodesConnected.push_back(nodes->at(match.node2Index));
                continue;
            }
            if (node.X == nodes->at(match.node2Index).X && node.Y == nodes->at(match.node2Index).Y) {
                node.nodesConnected.push_back(nodes->at(match.node1Index));
            }
        }
    }
}

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


            //check if there is that node in the list and add it
            if (!seeIfContainsNode(*burningNodes, nodes->at(m.node1Index)))
                burningNodes->push_back(&nodes->at(m.node1Index));
            if (!seeIfContainsNode(*burningNodes, nodes->at(m.node2Index)))
                burningNodes->push_back(&nodes->at(m.node2Index));
        }
    }
}

void resetFigureAndNodes(vector<Match> *figure, vector<Node> *nodes) {
    for (auto &l : *figure) {
        l.timeAlreadyBurnt = 0;
    }
    for (auto &l : *nodes) {
        l.isBurnt = false;
        l.isNew = false;
    }
}

void burnMatches(const vector<Match> &matchList, vector<Match> *figure, vector<Node> nodes) {
    for (auto &i : *figure) {
        for (Match match: matchList) {
            if (i.timeAlreadyBurnt == i.timeNeededToBurn) {
                continue;
            }
            if ((nodes[match.node1Index].X == nodes[i.node1Index].X &&
                 nodes[match.node1Index].Y == nodes[i.node1Index].Y)
                && (nodes[match.node2Index].X == nodes[i.node2Index].X &&
                    nodes[match.node2Index].Y == nodes[i.node2Index].Y)
                && !i.hasBurnedDuringRound) {

                if (nodes[i.node1Index].isNew || nodes[i.node2Index].isNew) {
                    i.timeAlreadyBurnt += minTime;
                    i.hasBurnedDuringRound = true;
                    continue;
                }
                if (nodes[i.node1Index].isBurnt && nodes[i.node2Index].isBurnt) {
                    i.timeAlreadyBurnt += (2 * minTime);
                    i.hasBurnedDuringRound = true;
                    continue;
                }
                i.timeAlreadyBurnt += minTime;
                i.hasBurnedDuringRound = true;
            } else {
                continue;
            }
        }
    }
}

void burnMatchesAndNodesInMap(vector<Node *> *burningNodes, map<Node *, vector<Match >> *nodeVectorMap,
                              vector<Match> *figure, vector<Node> *nodes) {
    for (auto &nvm: *nodeVectorMap) {
        burnMatches(nvm.second, figure, *nodes);
        burnNodes(figure, burningNodes, nodes);
    }
}

// main function
int main() {
    int numberOfMatches;
    vector<Match> figure;
    vector<Node> nodes;
    vector<double> splitLine;
    string line;
    map<Node *, double> nodeResultMap;
    bool isNotDiagonal;
    double x1, x2, y1, y2, time;

    //initalize vectors
    cout << "Kolko kebriteni klechki shte izpolzvate?" << endl;
    getline(cin, line);
    numberOfMatches = std::stoi(line);

    for (int a = 0; a < numberOfMatches; a++) {
        cout << "Vuvedi Klechka: " << endl;
        cin >> x1 >> y1 >> x2 >> y2 >> time;

        isNotDiagonal = (x1 == x2) || (y1 == y2);

        if (checkIfMatchOverlaps(x1, y1, x2, y2, figure, nodes)) {
            cout << "Match overlaps " << endl;
            return -2;
        }

        addIfDoesntNodeExists(x1, y1, &nodes);
        addIfDoesntNodeExists(x2, y2, &nodes);

        int nodeIndex1 = findNodeIndex(x1, y1, nodes);
        int nodeIndex2 = findNodeIndex(x2, y2, nodes);

        figure.push_back({nodeIndex1, nodeIndex2, time, 0, false, isNotDiagonal});
    }



    for (int k = 0; k < nodes.size(); k++) {

        double totalTime = 0;
        vector<Match> tempMatchList;
        vector<Node *> burningNodes;
        map<Node *, vector<Match >> nodeVectorMap;
        Node *startingNode = &nodes[k];

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

        int checkIfConnected = 0;
        while (!areAllMatchesBurnt(figure)) {

            resetingForBegOFCycle(&figure, &nodes);

            initalizeNodeVectorMap(&burningNodes, &nodeVectorMap, figure, nodes);

            burnMatchesAndNodesInMap(&burningNodes, &nodeVectorMap, &figure, &nodes);

            totalTime += minTime;

            nodeVectorMap.clear();

            //printFigure(figure, totalTime);
            checkIfConnected++;

            if (checkIfConnected == 5000) {
                cout << "Figurata ne e svurzana!" << endl;
                return -1;
            }
        }

        nodeResultMap.insert({&nodes.at(k), totalTime});

        resetFigureAndNodes(&figure, &nodes);
    }
    for (auto &nrm: nodeResultMap) {

        cout << "Node (x,y): (" << nrm.first->X << ", " << nrm.first->Y << ") -> " << nrm.second << " " << endl;
    }
    return 0;
}
