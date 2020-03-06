#include <iostream>
#include <limits>
#include <iomanip>
#include <vector>
#include <map>

using namespace std;

// Number of nodes
#define N 3
float minTime = numeric_limits<float>::max();

struct Node {
    float X;
    float Y;
    bool isBurnt;
    vector<Node> nodesConnected;
};

//todo consider deleting time already burnt
struct Match {
    Node *node1;
    Node *node2;
    float timeToBurn;
    float timeAlreadyBurnt;
    float percentBurned;
};

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

vector<Match> step(const vector<Match> &figure, Node *pStartingNode) {
    Node node1, node2;
    Node startingNode = *pStartingNode;
    vector<Node> tempList;
    vector<Match> matchList;

    for (const Match &match: figure) {
        node1 = *match.node1;
        node2 = *match.node2;
        if ((startingNode.X == node1.X && startingNode.Y == node1.Y) ||
            (startingNode.X == node2.X && startingNode.Y == node2.Y)) {
            matchList.push_back(match);
        }
    }

    for (const Match &m : matchList) {
        if (m.percentBurned != 1) {
            if (m.node1->isBurnt && m.node2->isBurnt) {
                if (((m.timeToBurn - (m.timeToBurn * m.percentBurned)) / 2) < minTime) {
                    minTime = (m.timeToBurn - (m.timeToBurn * m.percentBurned)) / 2;
                }
            } else {
                if ((m.timeToBurn - (m.timeToBurn * m.percentBurned)) < minTime) {
                    minTime = m.timeToBurn - (m.timeToBurn * m.percentBurned);
                }
            }
        }
    }
    //cout << minTime << endl;

    return matchList;
}

void fillNodesConnected(vector<Node> *nodes, const vector<Match> &figure) {
    Node node1, node2;
    float X, Y;

    for (auto &node : *nodes) {
        vector<Node> nodesConnected;
        X = node.X;
        Y = node.Y;
        for (const Match &match: figure) {
            node1 = *match.node1;
            node2 = *match.node2;
            if (seeIfContainsNode(node.nodesConnected, node1) ||
                seeIfContainsNode(node.nodesConnected, node2)) {
                continue;
            }
            if (X == node1.X && Y == node1.Y) {
                nodesConnected.push_back(node2);
                continue;
            }
            if (X == node2.X && Y == node2.Y) {
                nodesConnected.push_back(node1);
            }
        }
        node.nodesConnected = nodesConnected;
    }
}

void burnMatches(vector<Match> *tempMatchList, vector<Match> *figure) {
    for (Match match: *tempMatchList) {
        for (auto &m : *figure) {
            if (match.node1->X == m.node1->X && match.node1->Y == m.node1->Y) {
                m.timeAlreadyBurnt += minTime;
                m.percentBurned += (minTime / m.timeToBurn);
            }
        }
    }
}

void burnNodes(vector<Match> *figure, vector<Node *> *burningNodes) {
    for (auto &m : *figure) {
        //cout << m.percentBurned << endl;
        if (m.percentBurned > 1) {
            cout << "BIGGER THAN ONE!!!" << endl;
        }
        if (m.percentBurned == 1) {
            m.node1->isBurnt = true;
            m.node2->isBurnt = true;

            //check if there is that node in the list and add it
            if (!seeIfContainsNode(*burningNodes, *m.node1))
                burningNodes->push_back(m.node1);
            if (!seeIfContainsNode(*burningNodes, *m.node2))
                burningNodes->push_back(m.node2);
        }
    }
}

// main function
int main() {
    //initalizing vectors
    vector<Node> nodes{
            {0, 0, false},
            {2, 0, false},
            {1, 2, false},
            {3, 2, false}
    };

    vector<Match> figure =
            {
                    {&nodes[0], &nodes[1], 1,   0, 0},
                    {&nodes[0], &nodes[2], 3,   0, 0},
                    {&nodes[2], &nodes[1], 1,   0, 0},
                    {&nodes[3], &nodes[2], 0.5, 0, 0}
            };

    float totalTime = 0;
    vector<Match> tempMatchList;
    vector<Node *> burningNodes;
    Node *startingNode = &nodes[1];
    map<Node *, vector<Match>> nodeVectorMap;

    //connecting nodes
    fillNodesConnected(&nodes, figure);

    //burn starting node
    burningNodes.push_back(startingNode);
    startingNode->isBurnt = true;

    //starting burning from starting node
    tempMatchList = step(figure, startingNode); //-> finds min Time of next step returns matches connected to 'startingNode'
    totalTime = totalTime + minTime;

    //burns percent of matches
    burnMatches(&tempMatchList, &figure);

    //burns nodes
    burnNodes(&figure, &burningNodes);

    cout << burningNodes.size() << endl;

    for (Node *node: burningNodes) {
        nodeVectorMap.insert({node, step(figure, node)});
    }
    cout << minTime << endl;

    minTime = numeric_limits<float>::max();




/*    for (const Match &match1: figure) {
        cout << (match1.node1->isBurnt ? "true-1" : "false1");

        cout << " node 1 (x,y): " << match1.node1->X << ", " << match1.node1->Y << endl;

        cout << (match1.node2->isBurnt ? "true-2" : "false2");

        cout << " node 2 (x,y): " << match1.node2->X << ", " << match1.node2->Y << endl;

    }*/
    return 0;
}
