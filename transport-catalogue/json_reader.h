#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace transport_catalogue {
namespace detail {
namespace json {
class JSONReader {
public:
    JSONReader() = default;
    JSONReader(const Document& doc);
    JSONReader(std::istream& input);
    const Document& get_document() const;
    void parse(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);

private:
    void parse_node_base(const Node& root, TransportCatalogue& catalogue, std::ostream& out);
    void parse_node_stat(const Node& root, std::vector<StatRequest>& stat_request, std::ostream& out);
    void parse_node_render(const Node& node, map_renderer::RenderSettings& rend_set, std::ostream& out);
    void parse_node(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings, std::ostream& out);

    Stop parse_node_stop(Node& node);
    Bus parse_node_bus(Node& node, TransportCatalogue& catalogue);
    std::vector<Distance> parse_node_distances(Node& node, TransportCatalogue& catalogue);

    Document document_;
};

}
}
}
