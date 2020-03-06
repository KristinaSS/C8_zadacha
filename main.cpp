#include <iostream>
#include <limits>
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

struct Match {
    Node *node1;
    Node *node2;
    float timeNeededToBurn;
    float timeAlreadyBurnt;
    bool isFullyBurnt;
};

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
/*        cout << m.timeNeededToBurn <<endl;
        cout << m.timeAlreadyBurnt <<endl;
        cout << "--------------------" <<endl;*/
        if (m.timeNeededToBurn != m.timeAlreadyBurnt) {
            if (m.node1->isBurnt && m.node2->isBurnt) {
                if (((m.timeNeededToBurn - m.timeAlreadyBurnt) / 2) < minTime) {
                    minTime = (m.timeNeededToBurn - m.timeAlreadyBurnt) / 2;
                }
            } else {
                if ((m.timeNeededToBurn - m.timeAlreadyBurnt) < minTime) {
                    minTime = m.timeNeededToBurn - m.timeAlreadyBurnt;
                }
            }
          //  cout << minTime << endl;
        }
    }

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
            if (m.timeAlreadyBurnt == m.timeNeededToBurn)
                continue;
            if (match.node1->X == m.node1->X && match.node1->Y == m.node1->Y) {
                if (m.node1->isBurnt && m.node2->isBurnt) {
                    cout << "enter" <<endl;
                    cout << " node 1 (x,y): " << m.node1->X << ", " << m.node1->Y << endl;
                    cout << " node 2 (x,y): " << m.node2->X << ", " << m.node2->Y << endl;
                    cout << minTime << endl;
                    m.timeAlreadyBurnt += (minTime * 2);
                    continue;
                }

                m.timeAlreadyBurnt += minTime;
            }
        }
    }
}

void burnNodes(vector<Match> *figure, vector<Node *> *burningNodes) {
    for (auto &m : *figure) {
/*        if (m.timeAlreadyBurnt > m.timeNeededToBurn) {
            cout << "BIGGER THAN ONE!!!" << m.timeAlreadyBurnt << endl;
            cout << " node 1 (x,y): " << m.node1->X << ", " << m.node1->Y << endl;
            cout << " node 2 (x,y): " << m.node2->X << ", " << m.node2->Y << endl;
        }*/
        if (m.timeAlreadyBurnt == m.timeNeededToBurn) {
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
                    {&nodes[0], &nodes[1], 1,   0, false},
                    {&nodes[0], &nodes[2], 3,   0, false},
                    {&nodes[2], &nodes[1], 1,   0, false},
                    {&nodes[3], &nodes[2], 0.5, 0, false}
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
    tempMatchList = step(figure,
                         startingNode); //-> finds min Time of next step returns matches connected to 'startingNode'
    totalTime = totalTime + minTime;

    //burns percent of matches
    burnMatches(&tempMatchList, &figure);

    //burns nodes
    burnNodes(&figure, &burningNodes);

   // cout << minTime << endl;
    cout << "start"<< endl;
    int i = 0;
    while (!areAllMatchesBurnt(figure)) {
        for (Node *node: burningNodes) {
            nodeVectorMap.insert({node, step(figure, node)});
        }
        //cout << nodeVectorMap.size() << endl;
        cout << "end" << endl;
        cout << figure[1].timeAlreadyBurnt << endl;
        cout << "--------------------" <<endl;

        for (auto &nvm: nodeVectorMap) {
            burnMatches(&nvm.second, &figure);
            burnNodes(&figure, &burningNodes);
        }
        totalTime += minTime;

        //cout << totalTime << endl;

        minTime = numeric_limits<float>::max();
        nodeVectorMap.clear();

    }
    cout << totalTime;

    return 0;
}
