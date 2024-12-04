<template>
  <div>
    <v-network-graph
    :selected-nodes="selectedNodes"
    :nodes="filteredData.nodes"
    :edges="filteredData.edges"
    :layouts="filteredData.layouts"
    :configs="configs"
    class="network-graph"
  />
  <div class="control-panel">
      <input v-model="rangeInput" placeholder="Enter range (e.g., 0-2)" />
      <button @click="filterNodesByRange">Filter Nodes</button>
      <button @click="resetNodes">Reset</button>
    </div>

      <!-- Modal/Dialogue for showing selected node details -->
      <div v-if="selectedNode" class="modal">
      <div class="modal-content">
        <span class="close" @click="selectedNode = null">&times;</span>
        <h3>Node Details</h3>
        <p><strong>Node Name:</strong> {{ selectedNode.name }}</p>
        <p><strong>Node Color:</strong> {{ data.colors[selectedNode.name] ? 'Red' : 'Black' }}</p>
      </div>
      </div>
  </div>
</template>

<script>
// MOST CODE IS FROM V-NETWORK-GRAPH AND VUE DOCUMENTATION
import { ref, reactive } from "vue";
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

    let filteredData = reactive({
      nodes: {},
      edges: {},
      colors: {},
      layouts: { nodes: {} },
    });

    const COLOR_RED = "#ff0000";
    const COLOR_BLACK = "#000000";

    let rangeInput = ref("");
    const selectedNode = ref(null);

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
              return filteredData.colors[n.name] ? COLOR_RED : COLOR_BLACK;
            },
          },
        },
      })
    );
    configs.node.selectable = true
    configs.node.label.visible = true

    function parseRange(range) {
      const [start, end] = range.split("-").map(Number);
      if (isNaN(start) || isNaN(end)) {
        return [];
      }
      const min = Math.min(start, end);
      const max = Math.max(start, end);
      return Array.from({ length: max - min + 1 }, (_, i) => (min + i).toString());
    }

    function filterNodesByRange() {
      const range = parseRange(rangeInput.value);

      //Filters nodes based on their name (timestamp) based on the range
      filteredData.nodes = Object.fromEntries(
        Object.entries(data.nodes).filter(([, node]) =>
          range.includes(node.name)
        )
      );

      //Filters edges based on their source and target nodes that match those in the range
      filteredData.edges = Object.fromEntries(
        Object.entries(data.edges).filter(([, edge]) =>
          range.includes(data.nodes[edge.source]?.name) && range.includes(data.nodes[edge.target]?.name)
        )
      );

      //Uses filteredData to get the colors because I honestly can't be bothered anymore
      filteredData.colors = Object.fromEntries(
      Object.entries(filteredData.nodes).map(([, node]) => {
        const nodeName = node.name;  // Get the name of the node
        const color = data.colors[nodeName];  // Get the color from data.colors using the node's name
        return [nodeName, color];  // Return an array of [nodeName, color]
      })
    );
    }

    function resetNodes() {
      rangeInput.value = "";
      console.log(data.nodes);
      filteredData.nodes = data.nodes;
      filteredData.edges = data.edges;
      filteredData.colors = data.colors;
    }

    function handleNodeSelected(node) {
      // When a node is selected, set it as the selected node
      selectedNode.value = node;
    }

    function buildNetwork() {
      // Assign values to the reactive objects
      data.nodes.node1 = { name: "1" };
      data.nodes.node2 = { name: "0" };
      data.nodes.node3 = { name: "2" };

      //False = black, True = red
      data.colors["1"] = false;
      data.colors["0"] = true;
      data.colors["2"] = true;

      data.edges.edge1 = { source: "node1", target: "node2" };
      // data.edges.edge2 = { source: "node2", target: "node3" };
      data.edges.edge3 = { source: "node3", target: "node1" };

      data.layouts.nodes.node1 = { x: 50, y: 0 };
      data.layouts.nodes.node2 = { x: 0, y: 75 };
      data.layouts.nodes.node3 = { x: 100, y: 75 };

      filteredData.nodes = data.nodes;
      filteredData.edges = data.edges;
      filteredData.colors = data.colors;
    }

    return {
      data,
      configs,
      rangeInput,
      filteredData,
      resetNodes,
      filterNodesByRange,
      handleNodeSelected,
      selectedNode
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

.modal {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
}

.modal-content {
  background-color: white;
  padding: 20px;
  border-radius: 10px;
  text-align: center;
}

.close {
  font-size: 30px;
  position: absolute;
  top: 10px;
  right: 10px;
  cursor: pointer;
}
</style>
