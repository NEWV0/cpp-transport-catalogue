#include "request_handler.h"
 
namespace request_handler {
    
Node RequestHandler::execute_make_node_stop(int id_request, StopQueryResult stop_info){
    Dict result;
    Array buses;
    std::string str_not_found = "not found";
    
    if (stop_info.not_found) {
        result.emplace("request_id", Node(id_request));
        result.emplace("error_message", Node(str_not_found));
        
    } else {
        result.emplace("request_id", Node(id_request));
        
        for (std::string bus_name : stop_info.buses_name) {
            buses.push_back(Node(bus_name));
        }
        
        result.emplace("buses", Node(buses));
    }
    
    return Node(result);
}
 
Node RequestHandler::execute_make_node_bus(int id_request, BusQueryResult bus_info){
    Dict result;
    std::string str_not_found = "not found";
    
    if (bus_info.not_found) {
        result.emplace("request_id", Node(id_request));
        result.emplace("error_message", Node(str_not_found));
    } else {
        result.emplace("request_id", Node(id_request));        
        result.emplace("curvature", Node(bus_info.curvature));
        result.emplace("route_length", Node(bus_info.route_length));
        result.emplace("stop_count", Node(bus_info.stops_on_route));
        result.emplace("unique_stop_count", Node(bus_info.unique_stops));
    }
    
    return Node(result);
}    
      
Node RequestHandler::execute_make_node_map(int id_request, TransportCatalogue& catalogue_, RenderSettings render_settings){
    Dict result;
    std::ostringstream map_stream;
    std::string map_str;
 
    MapRenderer map_catalogue(render_settings);      
    map_catalogue.init_sphere_projector(get_stops_coordinates(catalogue_));
    execute_render_map(map_catalogue, catalogue_);
    map_catalogue.get_stream_map(map_stream);
    map_str = map_stream.str();
 
    result.emplace("request_id", Node(id_request));
    result.emplace("map", Node(map_str));
    
    return Node(result);
}
    
void RequestHandler::execute_queries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests, RenderSettings& render_settings){
    std::vector<Node> result_request;
    
    for (StatRequest req : stat_requests) {
        
        if (req.type == "Stop") {
            result_request.push_back(execute_make_node_stop(req.id, stop_query(catalogue, req.name))); 
        } else if (req.type == "Bus") {
            result_request.push_back(execute_make_node_bus(req.id, bus_query(catalogue, req.name)));
        } else if(req.type == "Map") {            
            result_request.push_back(execute_make_node_map(req.id, catalogue, render_settings));
        }  
        
    }
    
    doc_out = Document{Node(result_request)};
}
 
   
std::vector<geo::Coordinates> RequestHandler::get_stops_coordinates(TransportCatalogue& catalogue_) const {
    
    std::vector<geo::Coordinates> stops_coordinates; 
    auto buses = catalogue_.get_busname_to_bus();
    
    for (auto& [busname, bus] : buses) {
        for (auto& stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;
            
            stops_coordinates.push_back(coordinates);
        }
    }
    return stops_coordinates;
}
    
std::vector<std::string_view> RequestHandler::get_sort_buses_names(TransportCatalogue& catalogue_) const {
    std::vector<std::string_view> buses_names;
    
    auto buses = catalogue_.get_busname_to_bus();
    if (buses.size() > 0) {
        
        for (auto& [busname, bus] : buses) {
            buses_names.push_back(busname);
        }   
 
        std::sort(buses_names.begin(), buses_names.end());
        
        return buses_names;
        
    } else {
        return {};
    }
}
 
BusQueryResult RequestHandler::bus_query(TransportCatalogue& catalogue, std::string_view bus_name) {
    Bus* bus = catalogue.get_bus(bus_name);

    if (bus != nullptr) {
        return bus->query_result; 
    } else {
        return { bus_name, true }; 
    }
} 
    
StopQueryResult RequestHandler::stop_query(TransportCatalogue& catalogue, std::string_view stop_name) {
    std::unordered_set<const Bus*> unique_buses;
    StopQueryResult stop_info;
    Stop* stop = catalogue.get_stop(stop_name);
 
    if (stop != NULL) {
        
        stop_info.name = stop->name;
        stop_info.not_found = false;
        unique_buses = catalogue.stop_get_uniq_buses(stop);
        
        if (unique_buses.size() > 0){
            for (const Bus* bus : unique_buses) {
                stop_info.buses_name.push_back(bus->name);
            }
            
            std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
        }   
        
    } else {    
        stop_info.name = stop_name;
        stop_info.not_found = true;
    }
    
    return stop_info;
}
    
const Document& RequestHandler::get_document(){
    return doc_out;
}
    
}