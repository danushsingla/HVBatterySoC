<template>
  <div>
    <v-network-graph
    :selected-nodes="selectedNodes"
    :nodes="data.nodes"
    :edges="data.edges"
    :layouts="data.layouts"
    :configs="configs"
    class="network-graph"
  />
  </div>
</template>

<script>
import { reactive } from "vue";
import * as vNG from "v-network-graph";
import {
  ForceLayout
} from "v-network-graph/lib/force-layout"

export default {
  setup() {
    const data = reactive({
      nodes: {},
      edges: {},
      colors: {},
      layouts: { nodes: {} },
    });

    const COLOR_RED = "#ff0000";
    const COLOR_BLACK = "#000000";

    // initialize network
    buildNetwork();

    const configs = reactive(
      vNG.defineConfigs({
        view: {
          layoutHandler: new ForceLayout({
            positionFixedByDrag: false,
            positionFixedByClickWithAltKey: true,
            createSimulation: (d3, nodes, edges) => {
              // d3-force parameters
              const forceLink = d3.forceLink(edges).id(d => d.id);
              return d3
                .forceSimulation(nodes)
                .force("edge", forceLink.distance(40).strength(0.5))
                .force("charge", d3.forceManyBody().strength(-800))
                .force("center", d3.forceCenter().strength(0.05))
                .alphaMin(0.001);
            }
          }),
        },
        node: {
          label: {
            visible: false,
          },
          normal: {
            color: (n) => {
              return data.colors[n.name] ? COLOR_RED : COLOR_BLACK;
            },
          },
        },
      })
    );
    configs.node.selectable = true
    configs.node.label.visible = true

    function buildNetwork() {
      // Assign values to the reactive objects
      data.nodes.node1 = { name: "N1" };
      data.nodes.node2 = { name: "N2" };
      data.nodes.node3 = { name: "N3" };

      //False = black, True = red
      data.colors["N1"] = false;
      data.colors["N2"] = true;
      data.colors["N3"] = true;

      data.edges.edge1 = { source: "node1", target: "node2" };
      // data.edges.edge2 = { source: "node2", target: "node3" };
      data.edges.edge3 = { source: "node3", target: "node1" };

      data.layouts.nodes.node1 = { x: 50, y: 0 };
      data.layouts.nodes.node2 = { x: 0, y: 75 };
      data.layouts.nodes.node3 = { x: 100, y: 75 };
    }

    return {
      data,
      configs,
    };
  },
};
</script>

<style>
.demo-control-panel {
  padding: 10px;
}

.network-graph {
  width: 100%;
  height: 600px;
}
</style>
