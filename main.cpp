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
    Node *node1;
    Node *node2;
    double timeNeededToBurn;
    double timeAlreadyBurnt;
    bool hasBurnedDuringRound;
};

Node *findNode(double x, double y, vector<Node> *nodes) {
    for (auto &node : *nodes) {
        if (node.X == x && node.Y == y)
            return &node;
    }
    return nullptr;
}

void findMinTime(const vector<Match> &matchList) {
    for (Match m : matchList) {

        if (m.timeNeededToBurn < m.timeAlreadyBurnt) {
            cout << "node 1  : (x,y) " << m.node1->X << ", " << m.node1->Y << endl;
            cout << "node 2  : (x,y) " << m.node2->X << ", " << m.node2->Y << endl;
            cout << "error" << endl;
        }

        if (m.timeNeededToBurn > m.timeAlreadyBurnt) {
            if (m.node1->isBurnt && m.node2->isBurnt) {
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

void initializeMatchList(const vector<Match> &figure, const Node &startingNode, vector<Match> *matchList) {
    Node node1, node2;
    for (Match match: figure) {
        node1 = *match.node1;
        node2 = *match.node2;
        if ((startingNode.X == node1.X && startingNode.Y == node1.Y) ||
            (startingNode.X == node2.X && startingNode.Y == node2.Y)) {
            matchList->push_back(match);
        }
    }
}

vector<Match> step(const vector<Match> &figure, Node *pStartingNode) {
    Node startingNode = *pStartingNode;
    vector<Match> matchList;

    initializeMatchList(figure, startingNode, &matchList);
    findMinTime(matchList);

    return matchList;
}

void initalizeNodeVectorMap(vector<Node *> *burningNodes, map<Node *, vector<Match >> *nodeVectorMap,
                            const vector<Match> &figure) {
    for (Node *node: *burningNodes) {
        nodeVectorMap->insert({node, step(figure, node)});
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
        << " cooridinates Node 1 (x,y): " << figure[i].node1->X << ", " << figure[i].node1->Y
        << " cooridinates Node 2 (x,y): " << figure[i].node2->X << ", " << figure[i].node2->Y << endl;
        cout << "addresss printFigure Node (x,y): " << &figure[i].node1 << ",  " <<&figure[i].node2 << endl;
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
    float X, Y;
    for (const Node &n: nodes) {
        X = node.X;
        Y = node.Y;
        if (X == n.X && Y == n.Y) {
            return true;
        }
    }
    return false;
}

bool seeIfContainsNode(const vector<Node *> &nodes, const Node &node) {
    float X, Y;
    for (const Node *n : nodes) {
        X = node.X;
        Y = node.Y;
        if (X == n->X && Y == n->Y) {
            return true;
        }
    }
    return false;
}

Node *addIfDoesntNodeExists(double x, double y, vector<Node> *nodes) {
    cout<< "x: "<< x   ;
    cout<< "  y: "<< y << endl;
    Node node = {x, y, false, false};
    cout<< "x: "<< node.X   ;
    cout<< "  y: "<< node.Y << endl;
    if (!seeIfContainsNode(*nodes, node)) {
        nodes->push_back(node);
        return &nodes->back();
    }
    return findNode(x, y, nodes);
}

void fillNodesConnected(vector<Node> *nodes, const vector<Match> &figure) {
    for (auto &node : *nodes) {
        for (const Match &match: figure) {
            if (seeIfContainsNode(node.nodesConnected, *match.node1) ||
                seeIfContainsNode(node.nodesConnected, *match.node2)) {
                continue;
            }
            if (node.X == match.node1->X && node.Y == match.node1->Y) {
                node.nodesConnected.push_back(*match.node2);
                continue;
            }
            if (node.X == match.node2->X && node.Y == match.node2->Y) {
                node.nodesConnected.push_back(*match.node1);
            }
        }
    }
}

void burnMatches(const vector<Match> &matchList, vector<Match> *figure) {
    for (auto &i : *figure) {
        for (Match match: matchList) {
            if (i.timeAlreadyBurnt == i.timeNeededToBurn) {
                continue;
            }
            if ((match.node1->X == i.node1->X && match.node1->Y == i.node1->Y)
                && (match.node2->X == i.node2->X && match.node2->Y == i.node2->Y)
                && !i.hasBurnedDuringRound) {
                if (i.node1->isNew || i.node2->isNew) {
                    i.timeAlreadyBurnt += minTime;
                    i.hasBurnedDuringRound = true;
                    continue;
                }
                if (i.node1->isBurnt && i.node2->isBurnt) {
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

void burnNodes(vector<Match> *figure, vector<Node *> *burningNodes) {
    for (auto &m : *figure) {
        if (m.timeAlreadyBurnt == m.timeNeededToBurn) {
            if (!m.node1->isBurnt) {
                m.node1->isNew = true;
            }
            m.node1->isBurnt = true;
            if (!m.node2->isBurnt) {
                m.node2->isNew = true;
            }
            m.node2->isBurnt = true;


            //check if there is that node in the list and add it
            if (!seeIfContainsNode(*burningNodes, *m.node1))
                burningNodes->push_back(m.node1);
            if (!seeIfContainsNode(*burningNodes, *m.node2))
                burningNodes->push_back(m.node2);
        }
    }
}

void burnMatchesAndNodesInMap(vector<Node *> *burningNodes, map<Node *, vector<Match >> *nodeVectorMap,
                              vector<Match> *figure) {
    for (auto &nvm: *nodeVectorMap) {
        burnMatches(nvm.second, figure);
        burnNodes(figure, burningNodes);
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

vector<double> split(const string& str, char delimiter) {
    vector<double> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while (getline(ss, tok, delimiter)) {
        internal.push_back(stod(tok));
    }

    return internal;
}

// main function
int main() {
    int numberOfMatches;
    vector<Match> figure;
    vector<Node> nodes;
    vector<double> splitLine;
    string line;
    map<Node *, double> nodeResultMap;
    Node *pNode1, *pNode2;

    //initalize vectors
    cout << "Kolko kebriteni klechki shte izpolzvate?" << endl;
    getline(cin, line);
    numberOfMatches = std::stoi(line);

    for (int a = 0; a < numberOfMatches; a++) {
        cout << "Vuvedi Klechka: " << endl;
        getline(cin, line);
        splitLine = split(line, ' ');
        pNode1 = addIfDoesntNodeExists(splitLine[0], splitLine[1], &nodes);
        pNode2 = addIfDoesntNodeExists(splitLine[2], splitLine[3], &nodes);
        cout << "addresss 0 Node (x,y): " << pNode1 << ",  " <<pNode2 << endl;
        pNode1 = findNode(splitLine[0], splitLine[1], &nodes);
        pNode2 = findNode(splitLine[0], splitLine[1], &nodes);
        cout << "addresss 0 Node (x,y): " << pNode1 << ",  " <<pNode2 << endl;

        figure.push_back({pNode1,pNode2,splitLine[4], 0, false});

        cout << "addresss 0 Node (x,y): " << &figure[0].node1 << ",  " <<&figure[0].node2 << endl;

    }
/*    for(const Node& node: nodes){
        cout << " cooridinates Node 1 (x,y): " << node.X << ", " << node.Y << endl;
    }*/

    printFigure(figure, 0);

/*    for (int k = 0; k < nodes.size(); k++) {
        double totalTime = 0;
        vector<Match> tempMatchList;
        vector<Node *> burningNodes;
        Node *startingNode = &nodes[k];
        map<Node *, vector<Match >> nodeVectorMap;

        //printing figure
        printFigure(figure, totalTime);

        //connecting nodes
        fillNodesConnected(&nodes, figure);

        //burn starting node
        burningNodes.push_back(startingNode);
        startingNode->isBurnt = true;

        //starting burning from starting node
        tempMatchList = step(figure,
                             startingNode); //-> finds min Time of next step returns matches connected to 'startingNode'
        totalTime = totalTime + minTime;

        //burns percent of matches
        burnMatches(tempMatchList, &figure);

        //burns nodes
        burnNodes(&figure, &burningNodes);

        printFigure(figure, totalTime);

        while (!areAllMatchesBurnt(figure)) {
            resetingForBegOFCycle(&figure, &nodes);

            initalizeNodeVectorMap(&burningNodes, &nodeVectorMap, figure);

            burnMatchesAndNodesInMap(&burningNodes, &nodeVectorMap, &figure);

            totalTime += minTime;

            nodeVectorMap.clear();

            printFigure(figure, totalTime);
        }
        cout << "--------------------" << endl;
        cout << " adding total time: " << totalTime << endl;
        nodeResultMap.insert({&nodes.at(k), totalTime});

        resetFigureAndNodes(&figure, &nodes);
    }
    cout << "--------------------" << endl;

    int j = 0;
    for (auto &nrm: nodeResultMap) {
        cout << j << ": " << nrm.second << " " << endl;
        j++;
    }*/
    return 0;
}
/*    vector<Node> nodes{
            {0, 0, false},//0
            {2, 2, false},//1
            {4, 0, false},//2
            {6, 2, false},//3
            {8, 2, false},//4
            {9, 3, false},//5
            {9, 2, false}//6
    };

    vector<Match> figure =
            {
                    {&nodes[0], &nodes[1], 4,  0, false},//0
                    {&nodes[0], &nodes[2], 4,  0, false},//1
                    {&nodes[2], &nodes[1], 5,  0, false},//2
                    {&nodes[3], &nodes[2], 1,  0, false},//3
                    {&nodes[3], &nodes[1], 7,  0, false},//4
                    {&nodes[3], &nodes[4], 3,  0, false},//5
                    {&nodes[4], &nodes[5], 3,  0, false},//6
                    {&nodes[4], &nodes[6], 2,  0, false},//7
                    {&nodes[5], &nodes[6], 10, 0, false} //8
            };*/