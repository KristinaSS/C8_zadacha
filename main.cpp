#include <iostream>
#include <limits>
#include <iomanip>
#include <vector>

using namespace std;

// Number of nodes
#define N 3
float minTime = numeric_limits<float>::max();

struct Node {
    float X;
    float Y;
    bool isBurning;
    vector<Node> nodesConnected;
};

struct Match {
    Node node1;
    Node node2;
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

vector<Match> step(const vector<Match> &figure, const vector<Node> &nodes) {
    Node node1, node2;
    Node startingNode = nodes[2];
    vector<Node> tempList;
    vector<Match> matchList;

    //cout << minTime << endl;

    for (const Node &node : nodes) {
        if (node.X == startingNode.X && node.Y == startingNode.Y) {
            tempList = node.nodesConnected;
        }
    }

    if (!tempList.empty()) {
        for (const Node &node: tempList) {
            for (const Match &match: figure) {
                node1 = match.node1;
                node2 = match.node2;
                if ((node.X == node1.X && node.Y == node1.Y) || (node.X == node2.X && node.Y == node2.Y)) {
                    matchList.push_back(match);
                }
            }
        }

        for (const Match &m : matchList) {
            if (m.timeToBurn < minTime) {
                minTime = m.timeToBurn;
            }
        }
    }
   // cout << minTime;

    return matchList;
}

// main function
int main() {
    vector<Node> nodes{
            {0, 0, false},
            {2, 0, false},
            {1, 2, false},
            {3, 2, false}
    };

    vector<Match> figure =
            {
                    {nodes[0], nodes[1], 3,   0, 0},
                    {nodes[0], nodes[2], 1,   0, 0},
                    {nodes[2], nodes[1], 1,   0, 0},
                    {nodes[3], nodes[2], 0.5, 0, 0}
            };

    Node node1, node2;
    float X, Y;


    for (int i = 0; i < nodes.size(); i++) {
        vector<Node> nodesConnected;
        X = nodes[i].X;
        Y = nodes[i].Y;
        for (const Match &match: figure) {
            node1 = match.node1;
            node2 = match.node2;
            if (seeIfContainsNode(nodes[i].nodesConnected, node1) ||
                seeIfContainsNode(nodes[i].nodesConnected, node2)) {
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
        nodes[i].nodesConnected = nodesConnected;
    }

    float totalTime = 0;
    vector<Match> tempMatchList;

    tempMatchList = step(figure, nodes);
    if(totalTime == 0) {
        totalTime = totalTime + minTime;
        for(Match match: tempMatchList){
            for(int i = 0; i < figure.size(); i++){
                if(match.node1.X == figure[i].node1.X && match.node1.Y == figure[i].node1.Y){
                    figure[i].timeAlreadyBurnt = minTime;
                    figure[i].percentBurned = (minTime/figure[i].timeToBurn);
                }
            }
        }
    }
    minTime = numeric_limits<float>::max();

    for(Match match1: figure){
        cout<<match1.percentBurned<<endl;
    }

    /*for (const Node &n: nodes) {
        cout << "NODE (X, Y): ( " << n.X << ", " << n.Y << " )";
        cout << "NODES CONNECTED:" << endl;
        cout << "List size " << n.nodesConnected.size() << endl;
        for (const Node &np: n.nodesConnected) {
            cout << "       NODE (X, Y): ( " << np.X << ", " << np.Y << " )" << endl;
        }
    }*/
    return 0;
}