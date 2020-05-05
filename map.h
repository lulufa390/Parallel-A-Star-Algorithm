//
// Created by Jikai Lu on 4/24/20.
//

#ifndef FINAL_MAP_H
#define FINAL_MAP_H

#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <fstream>

using namespace std;

class TestResult
{
public:
    int shortest;
    int* thread_explore;
    TestResult (int thread_count) {
        thread_explore = new int[thread_count];
    }
    ~TestResult() {
        delete thread_explore;
    }
};

class Node
{
public:
    int node_id;

    // for bidirectional
    // int g2_value[2];
    // int f2_value[2];

    vector<pair<Node *, int>> adjacent_list;

    // Node *path_parent;

    // extra information
public:
    int x;
    int y;

public:
    Node(int id)
    {
        node_id = id;
        // path_parent = nullptr;
        x = y = -1;
    }

    int compute_heuristic(Node *node)
    {
        return abs(node->x - this->x) + abs(node->y - this->y);
    }
};

class Map
{
public:
    Node *start;
    Node *goal;

    vector<Node *> node_set;

    int width, height;

public:
    Map(ifstream& input, bool block_goal = false)
    {
        // ifstream input(fileName, ifstream::in);
        // int height, width;
        input >> width >> height;

        node_set = vector<Node *>(width * height);

        int startX, startY, destX, destY;
        input >> startX >> startY >> destX >> destY;

        vector<vector<Node *>> node_matrix(height, vector<Node *>(width, nullptr));

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                Node *node = new Node(width * i + j);
                node_set[node->node_id] = node;
                node_matrix[i][j] = node;

                node->x = i;
                node->y = j;
            }
        }

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                int x, y;
                input >> x >> y;
                int count;
                input >> count;
                while (count--)
                {
                    char dir;
                    input >> dir;
                    int newX, newY;
                    switch (dir)
                    {
                    case 'N':
                        newX = x;
                        newY = y - 1;
                        break;
                    case 'S':
                        newX = x;
                        newY = y + 1;
                        break;
                    case 'E':
                        newX = x + 1;
                        newY = y;
                        break;
                    case 'W':
                        newX = x - 1;
                        newY = y;
                        break;
                    default:
                        throw string("unknow direction");
                    }
                    if (!block_goal || ((newY != destY || newX != destX) && (y != destY || x != destX)))
                    {
                        node_matrix[y][x]->adjacent_list.push_back({node_matrix[newY][newX], 1});
                    }
                }
            }
        }

        start = node_matrix[startY][startX];

        goal = node_matrix[destY][destX];
    }

    // Map(const Map &map)
    // {
    //     start = map.start;
    //     goal = map.goal;

    //     for (Node *node : map.node_set)
    //     {
    //         node_set.insert(node);
    //     }
    // }

    // void reverse()
    // {
    //     Node *start_tmp = start;
    //     start = goal;
    //     goal = start_tmp;
    //     is_reverse = true;
    // }

    // int compute_heuristic(Node *node)
    // {
    //     return abs(node->x - goal->x) + abs(node->y - goal->y);
    // }

    // int compute_heuristic_reverse(Node *node)
    // {
    //     return abs(node->x - start->x) + abs(node->y - start->y);
    // }
};

#endif //FINAL_MAP_H
