<template>
  <div>
    <v-network-graph
    :nodes="filteredData.nodes"
    :edges="filteredData.edges"
    :layouts="filteredData.layouts"
    :configs="configs"
    :event-handlers="click_event"
    class="network-graph"
  />
  <div class="control-panel">
      <input v-model="rangeInput" placeholder="Enter range (e.g., 0-2)" />
      <button @click="filterNodesByRange">Filter Nodes</button>
      <button @click="resetNodes">Reset</button>
    </div>

    <!-- Node Info Box -->
    <div v-if="selectedNode" class="node-info-box">
      <h3>Node Information</h3>
      <p><strong>Name:</strong> {{ selectedNode.name }}</p>
      <p><strong>Color:</strong> {{ selectedNodeColor }}</p>
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
    let data = reactive({
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
    const selectedNodeColor = ref('');

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
      // console.log(data.nodes);
      filteredData.nodes = data.nodes;
      filteredData.edges = data.edges;
      filteredData.colors = data.colors;
    }

    const click_event = {
      "node:click": ({ node }) => {
        selectedNode.value = filteredData.nodes[node];

        //console.log(filteredData.colors);
        selectedNodeColor.value = filteredData.colors[filteredData.nodes[node].name] ? "Red" : "Black";
      },
    };

    function buildNetwork() {
      // Read from file
      fetch("data_small.txt")
        .then(response => response.text())
        .then(fileContent => {
          const fileData = parseFileData(fileContent);
          data.nodes = fileData.nodes;
          data.edges = fileData.edges;
          data.colors = fileData.colors;

          console.log(fileData.colors);

          data = fileData;
        })
        .catch(error => {
          console.error("Error reading file:", error);
        });

        console.log(data)
    }

    function parseFileData(fileContent) {
      const lines = fileContent.trim().split("\n");
      //Format for the data
      const nodes = {};
      const edges = {};
      const colors = {};

      lines.forEach((line, ) => {
        const parts = line.split(' ');

        //Start ripping apart the data
        const timestamp = parts[0];
        const color = parts[1] === '1'; //Convert from string to boolean
        const leftNodeTime = parseFloat(parts[2]);
        const rightNodeTime = parseFloat(parts[3]);
        const current = parseFloat(parts[4]);
        const voltages = parts.slice(5).map(parseFloat);

        nodes[timestamp] = {
          name: timestamp,
          color: color,
          leftNodeTime: leftNodeTime,
          rightNodeTime: rightNodeTime,
          current: current,
          voltages: voltages,
        }

        colors[timestamp] = color;

        //Creating edges
        if (leftNodeTime !== "-1" && leftNodeTime in nodes) {
          edges[`${timestamp}-left`] = {
            source: timestamp,
            target: leftNodeTime.toString(),
          };
        }
        if (rightNodeTime !== "-1" && rightNodeTime in nodes) {
          edges[`${timestamp}-right`] = {
            source: timestamp,
            target: rightNodeTime.toString(),
          };
        }
      })

      return { nodes, edges, colors };
    }

    return {
      data,
      configs,
      rangeInput,
      filteredData,
      resetNodes,
      filterNodesByRange,
      click_event,
      selectedNode,
      selectedNodeColor,
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

.node-info-box {
  position: fixed;
  top: 10px;
  right: 10px;
  background-color: rgba(0, 0, 0, 0.7);
  color: white;
  padding: 10px;
  border-radius: 5px;
  font-size: 14px;
  z-index: 1000;
}
</style>
