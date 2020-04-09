#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);

}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for(auto v:current_node->neighbors) {
        v->g_value = current_node->g_value + current_node->distance(*v);
        v->h_value = this->CalculateHValue(v);
        v->parent = current_node;
        open_list.push_back(v);
        v->visited = true;
    }

}

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort(open_list.begin(), open_list.end(), [](const RouteModel::Node* lhs, const RouteModel::Node* rhs) {
        return (lhs->g_value + lhs->h_value) < (rhs->g_value + rhs->h_value);
    });

    RouteModel::Node* ptr = new RouteModel::Node();
    ptr = open_list.front();
    open_list.erase(open_list.begin());
    return ptr;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    while (current_node->parent != nullptr)
    {
        distance += current_node->distance(*current_node->parent);
        path_found.insert(path_found.begin(), *current_node);
        current_node = current_node->parent;
    }
    
    path_found.insert(path_found.begin(), *current_node);

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    start_node->visited = true;
    open_list.push_back(start_node);
    while (open_list.size() != 0)
    {
        current_node = NextNode();
        if(current_node == end_node){
            m_Model.path = ConstructFinalPath(current_node);
            return;
        }
        else
        {
            AddNeighbors(current_node);
        }
        
    }    
    std::cout << "No Path Found" << std::endl;
    
}