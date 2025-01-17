#include "json_reader.h"

namespace transport_catalogue {
namespace detail {
namespace json {

JSONReader::JSONReader(const Document& doc) : document_(doc) {}
JSONReader::JSONReader(std::istream& input) : document_(json::load(input)) {}

Stop JSONReader::parse_node_stop(Node& node) {
    Stop stop;
    Dict stop_node;

    if (node.is_map()) {
        stop_node = node.as_map();
        stop.name = stop_node.at("name").as_string();
        stop.latitude = stop_node.at("latitude").as_double();
        stop.longitude = stop_node.at("longitude").as_double();
    }

    return stop;
}

std::vector<Distance> JSONReader::parse_node_distances(Node& node, TransportCatalogue& catalogue) {
    std::vector<Distance> distances;
    Dict stop_node;
    Dict stop_road_map;
    std::string begin_name;
    std::string last_name;
    int distance;

    if (node.is_map()) {
        stop_node = node.as_map();
        begin_name = stop_node.at("name").as_string();

        try {
            stop_road_map = stop_node.at("road_distances").as_map();

            for (auto [key, value] : stop_road_map) {
                last_name = key;
                distance = value.as_int();
                distances.push_back({catalogue.get_stop(begin_name), 
                                     catalogue.get_stop(last_name), distance});
            }

        } catch(...) {
            std::cout << "invalid road" << std::endl;
        }
    }

    return distances;
}

Bus JSONReader::parse_node_bus(Node& node, TransportCatalogue& catalogue) {
    Bus bus;
    Dict bus_node;
    Array bus_stops;

    if (node.is_map()) {
        bus_node = node.as_map();
        bus.name = bus_node.at("name").as_string();
        bus.is_roundtrip = bus_node.at("is_roundtrip").as_bool();

        try {
            bus_stops = bus_node.at("stops").as_array();

            for (Node stop : bus_stops) {
                bus.stops.push_back(catalogue.get_stop(stop.as_string()));
            }

            if (!bus.is_roundtrip) {
                size_t size = bus.stops.size() - 1;

                for (size_t i = size; i > 0; i--) {
                    bus.stops.push_back(bus.stops[i-1]);
                }
            }

        } catch(...) {
            std::cout << "base_requests: bus: stops is empty" << std::endl;
        }
    }

    return bus;
}

void JSONReader::parse_node_base(const Node& root, TransportCatalogue& catalogue, std::ostream& out) {
    Array base_requests;
    Dict req_map;
    Node req_node;

    std::vector<Node> buses;
    std::vector<Node> stops;

    if (root.is_array()) {
        base_requests = root.as_array();

        for (Node& node : base_requests) {
            if (node.is_map()) {
                req_map = node.as_map();

                try {
                    req_node = req_map.at("type");

                    if (req_node.is_string()) {
                        if (req_node.as_string() == "Bus") {
                            buses.push_back(req_map);
                        } else if (req_node.as_string() == "Stop") {
                            stops.push_back(req_map);
                        } else {
                            out << "base_requests are invalid" << std::endl;
                        }
                    }

                } catch(...) {
                    out << "base_requests does not have type value" << std::endl;
                }
            }
        }

        for (auto stop : stops) {
            catalogue.add_stop(parse_node_stop(stop));
        }

        for (auto stop : stops) {
            catalogue.add_distance(parse_node_distances(stop, catalogue));
        }

        for (auto bus : buses) {
            catalogue.add_bus(parse_node_bus(bus, catalogue));
        }

    } else {
        out << "base_requests is not an array" << std::endl;
    }
}

void JSONReader::parse_node_stat(const Node& node, std::vector<StatRequest>& stat_request, std::ostream& out) {
    Array stat_requests;
    Dict req_map;
    StatRequest req;

    if (node.is_array()) {
        stat_requests = node.as_array();

        for (Node req_node : stat_requests) {
            if (req_node.is_map()) {
                req_map = req_node.as_map();
                req.id = req_map.at("id").as_int();
                req.type = req_map.at("type").as_string();

                if (req.type != "Map") {
                    req.name = req_map.at("name").as_string();
                } else {
                    req.name = "";
                }

                stat_request.push_back(req);
            }
        }

    } else {
        out << "stat_requests is not an array" << std::endl;
    }
}

void JSONReader::parse_node_render(const Node& node, map_renderer::RenderSettings& rend_set, std::ostream& out) {
    Dict rend_map;
    Array bus_lab_offset;
    Array stop_lab_offset;
    Array arr_color;
    Array arr_palette;
    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
    double opacity_;

    if (node.is_map()) {
        rend_map = node.as_map();

        try {
            rend_set.width_ = rend_map.at("width").as_double();
            rend_set.height_ = rend_map.at("height").as_double();
            rend_set.padding_ = rend_map.at("padding").as_double();
            rend_set.line_width_ = rend_map.at("line_width").as_double();
            rend_set.stop_radius_ = rend_map.at("stop_radius").as_double();

            rend_set.bus_label_font_size_ = rend_map.at("bus_label_font_size").as_int();

            if (rend_map.at("bus_label_offset").is_array()) {
                bus_lab_offset = rend_map.at("bus_label_offset").as_array();
                rend_set.bus_label_offset_ = std::make_pair(bus_lab_offset[0].as_double(),
                                                            bus_lab_offset[1].as_double());
            }

            rend_set.stop_label_font_size_ = rend_map.at("stop_label_font_size").as_int();

            if (rend_map.at("stop_label_offset").is_array()) {
                stop_lab_offset = rend_map.at("stop_label_offset").as_array();
                rend_set.stop_label_offset_ = std::make_pair(stop_lab_offset[0].as_double(),
                                                             stop_lab_offset[1].as_double());
            }

            if (rend_map.at("underlayer_color").is_string()) {
                rend_set.underlayer_color_ = svg::Color(rend_map.at("underlayer_color").as_string());
            } else if (rend_map.at("underlayer_color").is_array()) {
                arr_color = rend_map.at("underlayer_color").as_array();
                red_ = arr_color[0].as_int();
                green_ = arr_color[1].as_int();
                blue_ = arr_color[2].as_int();

                if(arr_color.size() == 4){
                    opacity_ = arr_color[3].as_double();
                    rend_set.underlayer_color_ = svg::Color(svg::Rgba(red_,
                                                                      green_,
                                                                      blue_,
                                                                      opacity_));
                } else if (arr_color.size() == 3) {
                    rend_set.underlayer_color_ = svg::Color(svg::Rgb(red_,
                                                                     green_,
                                                                     blue_));
                }
            }

            rend_set.underlayer_width_ = rend_map.at("underlayer_width").as_double();

            if (rend_map.at("color_palette").is_array()) {
                arr_palette = rend_map.at("color_palette").as_array();

                for (Node color_palette : arr_palette) {
                    if (color_palette.is_string()) {
                        rend_set.color_palette_.push_back(svg::Color(color_palette.as_string()));
                    } else if (color_palette.is_array()) {
                        arr_color = color_palette.as_array();
                        red_ = arr_color[0].as_int();
                        green_ = arr_color[1].as_int();
                        blue_ = arr_color[2].as_int();

                        if (arr_color.size() == 4) {
                            opacity_ = arr_color[3].as_double();
                            rend_set.color_palette_.push_back(svg::Color(svg::Rgba(red_,
                                                                                   green_,
                                                                                   blue_,
                                                                                   opacity_)));
                        } else if (arr_color.size() == 3) {
                            rend_set.color_palette_.push_back(svg::Color(svg::Rgb(red_,
                                                                                  green_,
                                                                                  blue_)));
                        }
                    }
                }
            }
        } catch(...) {
            out << "unable to parse init settings" << std::endl;
        }

    } else {
        out << "render_settings is not a map" << std::endl;
    }
}

void JSONReader::parse_node(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings, std::ostream& out) {
    Dict root_dictionary;

    if (root.is_map()) {
        root_dictionary = root.as_map();

        try {
            parse_node_base(root_dictionary.at("base_requests"), catalogue, out);
        } catch(...) {
            out << "base_requests is empty" << std::endl;
        }

        try {
            parse_node_stat(root_dictionary.at("stat_requests"), stat_request, out);
        } catch(...) {
            out << "stat_requests is empty" << std::endl;
        }

        try {
            parse_node_render(root_dictionary.at("render_settings"), render_settings, out);
        } catch(...) {
            out << "render_settings is empty" << std::endl;
        }

    } else {
        out << "root is not a map" << std::endl;
    }
}

void JSONReader::parse(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings) {
    parse_node(document_.get_root(),
               catalogue,
               stat_request,
               render_settings,
               std::cout);
}

}
}
}
